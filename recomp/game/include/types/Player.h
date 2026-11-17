/* types/Player.h - the player records g_Player1 / g_Player2 ($40 bytes each; docs/NOTES.md
 * "Player records and the HUD", "Weapons and hits"). Fields not used by C yet stay unk_XX. */
#pragma once
#include "dk_mem.h"

typedef struct PlayerRec {
    u32 score;             /* +$00 */
    u16 health;            /* +$04: $100 full */
    u16 lives;             /* +$06 */
    u16 weapon_charge;     /* +$08: byte >= $2B = a charged shot */
    s16 hud_x;             /* +$0A: the HUD panel's home x (Scene_InitStage: the section header's + 8, player 2 + $D4) */
    s16 hud_y;             /* +$0C: its home y (the header's + 8) */
    s32 panel_x;           /* +$0E: the HUD panel's x, 16.16 (HUD_StepPanelSlide; its high word is the sprites' x) */
    s32 panel_speed;       /* +$12: its slide speed, 16.16 (0 = at rest; HudOp_WaitSlide polls it) */
    s32 panel_accel;       /* +$16: the slide's per-frame speed change, 16.16 (HUD_StartSlideIn / HUD_StartSlideOut) */
    u32 panel_text;        /* +$1A: the HUD panel's text (cleared by HUD_InitPanel) */
    u16 panel_timer;       /* +$1E: frames before the panel script's next step (1 = the next frame) */
    u32 panel_script;      /* +$20: the HUD panel script (ROM; HUD_RunPanelScript) */
    u8  ring_anim;         /* +$24: the life ring's frame counter (HUD_DrawLifeRing: bit 2 picks the frame); cleared by HUD_InitPanel */
    u8  flags;             /* +$25: PLAYER_* (g_P1Flags / g_P2Flags) */
    u16 rapid_charge;      /* +$26: frames of rapid weapon charge left (Item_CollectRapidCharge; -1 a frame at $103A6) */
    u16 weapon_type;       /* +$28: 0-2 */
    u16 weapon_level;      /* +$2A: 0-6 */
    s16 object;            /* +$2C: handle of the player's object (Vm2Obj; its a4 +$24 = script variable 1) */
    s16 x;                 /* +$2E: the player object's x, 16.16 (copied by the player handler $102D6 each frame; aimed at by Op2_AimAtTracked) */
    u16 x_frac;
    s16 y;                 /* +$32: its y, 16.16 */
    u16 y_frac;
    u16 attackers_last;    /* +$36: enemies that targeted the player last frame: latched from +$38 while script var 1 bit $40 (Player_RunScript); 0 out of play (the a2 target pickers compare it) */
    u16 attackers;         /* +$38: enemies targeting the player this frame (each a2 target op adds 1); cleared every frame after the latch */
    u8  targetable;        /* +$3A: $FF while var 1 bit $40 (enemies may target the player), else 0 */
    u8  unk_3B;
    u16 serial;            /* +$3C: the player object's serial ($56) */
    u16 pad;               /* +$3E: the pad number, 1 or 2 (Player_InitRecords); also the player's attacker id */
} PlayerRec;

/* PlayerRec.flags (+$25) */
enum {
    PLAYER_IN_PLAY  = 1 << 0,   /* on screen or about to spawn */
    PLAYER_RESPAWN  = 1 << 1,   /* spawn at the saved +$2E/+$32 instead of the join point */
    PLAYER_HUD_RIGHT = 1 << 2,  /* the right-hand HUD panel (player 2) */
    PLAYER_ROBIN    = 1 << 3,   /* plays Robin (Scr2_04D80C), else Batman */
    PLAYER_BAR_FLASH = 1 << 4,  /* toggled each panel draw while the charge is full: the bar's flash frame */
    PLAYER_BAR_BLINK = 1 << 5,  /* toggled each panel draw in the rapid charge's last 150 frames: the bar's blink */
    PLAYER_SLIDING_OUT = 1 << 6, /* the HUD panel is sliding off its screen edge (HUD_StartSlideOut) */
    PLAYER_PANEL_TEXT = 1 << 7, /* the HUD panel shows text */
};
static_assert(sizeof(PlayerRec) == 0x40, "PlayerRec");
static_assert(offsetof(PlayerRec, weapon_charge) == 0x08, "PlayerRec.weapon_charge");
static_assert(offsetof(PlayerRec, rapid_charge) == 0x26, "PlayerRec.rapid_charge");
static_assert(offsetof(PlayerRec, weapon_type) == 0x28, "PlayerRec.weapon_type");
static_assert(offsetof(PlayerRec, x) == 0x2E, "PlayerRec.x");
static_assert(offsetof(PlayerRec, y) == 0x32, "PlayerRec.y");
static_assert(offsetof(PlayerRec, pad) == 0x3E, "PlayerRec.pad");
static_assert(offsetof(PlayerRec, panel_text) == 0x1A, "PlayerRec.panel_text");
static_assert(offsetof(PlayerRec, panel_timer) == 0x1E, "PlayerRec.panel_timer");
static_assert(offsetof(PlayerRec, panel_script) == 0x20, "PlayerRec.panel_script");
static_assert(offsetof(PlayerRec, flags) == 0x25, "PlayerRec.flags");
static_assert(offsetof(PlayerRec, object) == 0x2C, "PlayerRec.object");
static_assert(offsetof(PlayerRec, attackers_last) == 0x36, "PlayerRec.attackers_last");
static_assert(offsetof(PlayerRec, targetable) == 0x3A, "PlayerRec.targetable");
static_assert(offsetof(PlayerRec, serial) == 0x3C, "PlayerRec.serial");
static_assert(offsetof(PlayerRec, weapon_level) == 0x2A, "PlayerRec.weapon_level");
static_assert(offsetof(PlayerRec, flags) == 0x25, "PlayerRec.flags");
static_assert(offsetof(PlayerRec, object) == 0x2C, "PlayerRec.object");
static_assert(offsetof(PlayerRec, hud_x) == 0x0A, "PlayerRec.hud_x");
static_assert(offsetof(PlayerRec, panel_x) == 0x0E, "PlayerRec.panel_x");
static_assert(offsetof(PlayerRec, panel_speed) == 0x12, "PlayerRec.panel_speed");
static_assert(offsetof(PlayerRec, panel_accel) == 0x16, "PlayerRec.panel_accel");
static_assert(offsetof(PlayerRec, ring_anim) == 0x24, "PlayerRec.ring_anim");
