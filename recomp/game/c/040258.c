#include "game.h"

enum {
    SCROLL_LINES_COLUMNS = 0x2C,   /* g_ScrollMode: line H-scroll + 2-cell column V-scroll */
    ROOM_SWAY_START = 0xB00,       /* the room's wobble spring, displaced ... */
    ROOM_SWAY_SPEED = 0x20,        /* ... and moving */
};

/* Stage3_SpottedRoomInitScroll - starts the spotted room's scroll: scroll mode $2C and its wobble
 * spring (g_Stage3RoomSway $B00, speed $20), which Stage3_SpottedRoomScroll swings undamped
 * In:  -
 * Out: g_ScrollMode, g_Stage3RoomSway, g_Stage3RoomSwayVel
 * Evidence: move.w #$2C,(g_ScrollMode).l / move.w #$B00,(g_Stage3RoomSway).l / move.w
 *           #$20,(g_Stage3RoomSwayVel).l; jsr from Stage3_RunSpottedRoom only; TAS f101686
 */
void Stage3_SpottedRoomInitScroll(void)
{
    g_ScrollMode = SCROLL_LINES_COLUMNS;
    g_Stage3RoomSway = ROOM_SWAY_START;
    g_Stage3RoomSwayVel = ROOM_SWAY_SPEED;
}
