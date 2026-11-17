#include "game.h"

enum {
    MONITORS_A_OFFSET = 0x50,   /* plane A's V-scroll is this much less than plane B's */
};

/* the 16.16 shake's integer part (`add.w (g_ShakeY).w`) */
static uint16_t shake_px(void) { return (uint16_t)((uint32_t)g_ShakeY >> 16); }

/* Story_MonitorsAlarmScroll - the monitors object's slot-2 native once the alarm goes: the red
 * tint of all four palette lines toggles between 0 and +4 every 8 frames, and both planes'
 * V-scroll = y + the shake.
 * In:  self = the object
 * Out: self->counter + 1; g_Pal0-3TintR; g_VScrollA/B
 * Evidence: addq.w #1 on $2A(a6) / lsr.w #2 / andi.w #2 / add.b d0,d0 into g_Pal0/2/3/1TintR;
 *           set as coroutine slot 2 by Op2_StartCoroutine2Rel at $0488DA, called by
 *           Op2_CallSlot2Native; scenario-ending 140822-145128
 */
void Story_MonitorsAlarmScroll(StoryCamObj *self)
{
    uint16_t count = (uint16_t)(self->counter + 1);
    self->counter = count;
    uint8_t red = (uint8_t)(((count >> 2) & 2) * 2);   /* 0 or 4: on for 8 frames, off for 8 */
    g_Pal0TintR = red;
    g_Pal2TintR = red;
    g_Pal3TintR = red;
    g_Pal1TintR = red;
    uint16_t v = (uint16_t)(self->y + shake_px());
    g_VScrollA = v;
    g_VScrollB = v;
}

/* Story_MonitorsScroll - the monitors object's first slot-2 native (the pan down): plane B's
 * V-scroll = y + the shake, plane A's $50 px less.
 * In:  self = the object
 * Out: g_VScrollA/B
 * Evidence: move.w $42(a6),d0 / add.w g_ShakeY / move.w d0,g_VScrollB / subi.w #$50 / move.w
 *           d0,g_VScrollA; set as coroutine slot 2 by Op2_StartCoroutine2Rel at $048890, called by
 *           Op2_CallSlot2Native at $04889C
 */
void Story_MonitorsScroll(const StoryCamObj *self)
{
    uint16_t v = (uint16_t)(self->y + shake_px());
    g_VScrollB = v;
    g_VScrollA = (uint16_t)(v - MONITORS_A_OFFSET);
}
