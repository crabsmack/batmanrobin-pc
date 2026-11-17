#include "game.h"

/* the leaping thug's a2 script (ROM data right after its spawner's code; batmanrobin.asm) */
static constexpr uint32_t Scr2_025570 = 0x025570u;

/* Stage2_SetSpawnVarsFromEntry - spawner tail: gives the new object its script variables 0 and
 * 1 from the two words that follow the callback long in the trigger entry
 * In:  entry = the trigger entry's words after the callback (a1), vm = the new object's script
 *      block (a4, left by Obj_SpawnEnemyOrQuit)
 * Out: entry advanced past the two words; Z set (moveq #0,d0: the spawn succeeded; d0 itself
 *      is never read)
 * Evidence: move.w (a1)+,$22(a4) / move.w (a1)+,$24(a4) / moveq #0,d0 / rts; jmp'd to by
 *           Stage2_SpawnLeapingThug and Enemy_PoleThug_SpawnA-D; TAS f21270-f30019. The prototype
 *           gains &ccr for the measured ccout(Z) (registers unchanged)
 */
void Stage2_SetSpawnVarsFromEntry(const u16 **entry, Vm2Block *vm, dk::Flags *ccr)
{
    vm->var[0] = *(*entry)++;
    vm->var[1] = *(*entry)++;
    *ccr = dk::Flags::test(0, 4, ccr->x);   /* moveq #0,d0 */
}

/* Stage2_SpawnLeapingThug - lift trigger callback: spawns the thug Scr2_025570 (score $64,
 * energy $30/$C0) that leaps in at the top of the shaft, script variables 0/1 from the entry
 * In:  x/y = where the trigger entry places it, a1 = the entry's words after the callback long
 * Out: Z set, a6 = the thug, a1 past the two words; a short pool returns past this to the
 *      trigger loop (Obj_SpawnEnemyOrQuit)
 * Evidence: lea Scr2_025570,a2 / jsr Obj_SpawnEnemyOrQuit / jmp Stage2_SetSpawnVarsFromEntry;
 *           dc.l at $0214A0, $0214AC, $021556 ... in Tbl_02105A; RAM f22080 and f25380: a
 *           Scr2_025570 object (vector 1 = $025596) at x 32 above the screen
 */
void Stage2_SpawnLeapingThug(uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *a1, uint32_t *script, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    /* Obj_Spawn sets a0 (the run list) and a5 itself: the values going in are not read */
    uint32_t d0 = 0, list = 0, sprite = 0;
    *script = Scr2_025570;
    Obj_SpawnEnemyOrQuit(&d0, d1, x, y, d7, &list, *script, a3, a4, &sprite, a6);
    dk::jump(at_Stage2_SetSpawnVarsFromEntry);   /* its a1 = the entry, a4 = the new thug's block */
}
