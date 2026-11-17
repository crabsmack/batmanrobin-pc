#include "game.h"

/* The weapon / power-up pickup natives ($013C8A-$013D17): one function file with five more entry
 * points, each called by the pickup scripts' native-call op ($2CB2) or switch op ($2D30). */

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Item_WeaponItemGone - a weapon or power-up pickup ends (collected, or it left the screen): one
 * fewer counted in g_WeaponItemCount, not below 0.
 * In:  none
 * Out: g_WeaponItemCount - 1 (floor 0)
 * Evidence: subq.w #1 / bcc / clr.w; native (op $2CB2) at $13C48/$13C56 just before Op2_Kill;
 *           pairs with the increment in Item_WeaponItemInit; 67 TAS frames, 43 runs
 */
void Item_WeaponItemGone(void)
{
    uint16_t n = g_WeaponItemCount;
    g_WeaponItemCount = n ? (uint16_t)(n - 1) : 0;
}

/* Item_WeaponItemInit - weapon pickup setup native: counts the pickup and picks its value at
 * random from the script's value table (a count word, the values, then a cycle that loops on a
 * negative offset).
 * In:  script = a word offset to the value table (from the word itself); item = the pickup
 * Out: item->value = item->next_value = the value; item->cycle = the cycle after it; script past
 *      the operand; rand's low word = the value (its upper word Rand_Below's)
 * Evidence: adda.w (a2)+,a0 / move.w (a0)+,d0 / jsr Rand_Below / add.w d0,d0 / adda.w d0,a0; op
 *           $2CB2 in all 20 weapon spawn scripts ($1394C-$13B62), tables $13C5C-$13C7E
 */
void Item_WeaponItemInit(uint32_t *rand, const s16 **script, ItemObj *item)
{
    g_WeaponItemCount = (uint16_t)(g_WeaponItemCount + 1);
    const s16 *table = (const s16 *)((const u8 *)*script + (int16_t)**script);
    (*script)++;
    set_low_word(rand, (uint16_t)(int16_t)*table++);   /* the count */
    Rand_Below(rand);
    set_low_word(rand, (uint16_t)(*rand * 2));          /* the chosen value's byte offset */
    table = (const s16 *)((const u8 *)table + (int16_t)(uint16_t)*rand);
    uint16_t v = (uint16_t)(int16_t)*table++;
    set_low_word(rand, v);
    item->value = v;
    item->next_value = v;
    item->cycle = table;
}

/* Item_WeaponItemNextValue - weapon pickup native: steps the value cycle and prepares the next
 * value (a negative word rewinds the cycle by that many bytes).
 * In:  item->cycle
 * Out: item->next_value = the next value, also returned; item->cycle advanced
 * Evidence: move.w (a0)+,d0 / bpl / adda.w d0,a0; op $2CB2 at $13C28 (every $5A0 ticks) and $13C36
 */
uint16_t Item_WeaponItemNextValue(ItemObj *item)
{
    const s16 *c = item->cycle;
    int16_t v = *c++;
    if (v < 0) {
        c = (const s16 *)((const u8 *)c + v);
        v = *c++;
    }
    item->next_value = (uint16_t)v;
    item->cycle = c;
    return (uint16_t)v;
}

/* Item_WeaponItemValue - weapon pickup native for the op $2D30 switch: the pickup's current
 * value, so the icon script picks the matching animation.
 * In:  item->value
 * Out: the value
 * Evidence: move.w $2A(a6),d0; the target of nine op $2D30 switches in the icon scripts $13D42-$13DC2
 */
uint16_t Item_WeaponItemValue(const ItemObj *item)
{
    return item->value;
}

/* Item_WeaponItemShowNext - weapon pickup native: the prepared next value becomes the shown and
 * collected one.
 * In:  item->next_value
 * Out: item->value = it, also returned
 * Evidence: move.w $2C(a6),d0 / move.w d0,$2A(a6); op $2CB2 at $13C3A after Item_WeaponItemNextValue
 */
uint16_t Item_WeaponItemShowNext(ItemObj *item)
{
    uint16_t v = item->next_value;
    item->value = v;
    return v;
}

/* Item_CollectWeapon - touch native of the weapon pickups: value 1-3 selects weapon type value-1
 * (the same type: level +1 up to 6; a new type: keeps the level, clears the charge); value 0 lowers
 * the level by one (not below 0). Not a player's touch: Item_FoundBoxPlayer returns past this.
 * In:  g_FoundBox (through Item_FoundBoxPlayer), item->value
 * Out: the player record's weapon type, level and charge; type/level/value as the asm leaves d0-d2
 *      (type, level, value - 1); owner = the box owner
 * Evidence: op $2CB2 at $13C44 after sound $47 (the touch coroutine $13C40); record +$28/+$2A/+$8
 *           (NOTES Weapons)
 */
void Item_CollectWeapon(uint16_t *type, uint16_t *level, uint16_t *value, const void **owner, const ItemObj *item)
{
    PlayerRec *p = 0;
    Item_FoundBoxPlayer(&p, owner);
    *type = p->weapon_type;
    *level = p->weapon_level;
    *value = item->value;
    if (*value == 0) {
        *level = (uint16_t)(*level - 1);
        if ((int16_t)*level < 0)
            return;
    } else {
        *value = (uint16_t)(*value - 1);
        if (*value != *type) {
            p->weapon_charge = 0;
            *type = *value;
        } else {
            if (*level == 6)
                return;
            *level = (uint16_t)(*level + 1);
        }
    }
    p->weapon_type = *type;
    p->weapon_level = *level;
}
