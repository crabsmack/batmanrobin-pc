#include "game.h"

/* The scene lists and section order tables Main_Start runs (ROM). */
static constexpr uint32_t Tbl_0155BC = 0x0155BCu;   /* the boot VDP registers (VDP_LoadRegs) */
static constexpr uint32_t Tbl_00A152 = 0x00A152u;   /* the attract sequence: $49DA6, $49F62, $48AD2, $48CBA */
static constexpr uint32_t Tbl_00A136 = 0x00A136u;   /* the attract story after the title */
static constexpr uint32_t Tbl_009FFC = 0x009FFCu;   /* the section order: (from, to) word pairs */
static constexpr uint32_t Tbl_00A05C = 0x00A05Cu;   /* each section to itself: the retry after a continue */

enum {
    MAIN_MUSIC_TITLE = 0x11,
    MAIN_MUSIC_ENDING = 0x12,
    MAIN_MUSIC_CONTINUE = 0x13,
    MAIN_RESULT_START = 1,             /* g_SceneResult: Start pressed / continue taken */
    MAIN_RESULT_GAME_OVER = 2,         /* SceneCb_CheckGameOver */
    MAIN_VDP_BOOT_DISPLAY = 0x8124,    /* reg 1: display off, V-int on */
    MAIN_VDP_DISPLAY_ON = 0x8164,
    MAIN_RAM_LONGS = 0x4000,           /* all 64 KB of work RAM */
};

/* a ROM table as the pointer Scene_RunList / Section_RunNext take */
static const u32 *main_table(uint32_t addr)
{
    PTR(const u32) at;
    at.v = addr;
    return at;
}

/* Main_Start - the game's main program after boot: clears all of work RAM, CRAM and VRAM, starts
 * the sound driver, seeds the RNG, loads the boot VDP registers, sets the new-game defaults, then
 * loops forever through the attract sequence, the title, player select, the sections in order,
 * the continue and game over screens and the ending. Its labels (Main_Attract, Main_Title,
 * Main_NewGame, Main_PlayLoop, Main_RunSection, Main_NoContinue, Main_Ending) are the gotos.
 * In:  the registers boot left (handed to the scenes: they pass d1/d3-d6/a1-a6 along)
 * Out: never returns
 * Evidence: ori.w #$700,sr / move.w #$8124 / 64 KB of move.l d0,(a0)+ from $FF0000 /
 *           VDP_ClearCRAM / VDP_ClearVRAM / Snd_Init / RNG $79, $4B, $D3 / VDP_LoadRegs
 *           Tbl_0155BC / addq.b #1,g_VIntOff / $8164 / andi.w #$F8FF,sr / the defaults; then the
 *           labels' asm; entered by jmp from Boot_InitIOPorts at TAS f3; never returns, so D2
 *           (not verify) checks it
 */
void Main_Start(uint32_t d1, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t a1, uint32_t a2,
                uint32_t a3, uint32_t a4, uint32_t a6)
{
    uint32_t d0 = 0, d7 = 0, a5 = 0;
    dk::set_interrupt_mask(7);
    hw::port_write16(hw::VDP_CTRL, MAIN_VDP_BOOT_DISPLAY);
    u32 *ram = (u32 *)g_SceneBuf;
    for (int i = 0; i < MAIN_RAM_LONGS; i++)
        ram[i] = 0u;
    d7 = (d7 & 0xFFFF0000u) | 0xFFFFu;            /* the clear's dbra */
    VDP_ClearCRAM();
    VDP_ClearVRAM();
    a5 = Snd_Init();
    g_RandState = 0x79;
    g_RandState2 = 0x004B00D3u;                   /* the words $4B and $D3 at $F5FE / $F600 */
    uint16_t cmd = (uint16_t)d0;
    VDP_LoadRegs(&cmd, &d7, Tbl_0155BC);
    d0 = (d0 & 0xFFFF0000u) | cmd;
    g_VIntOff = (uint8_t)(g_VIntOff + 1);
    hw::port_write16(hw::VDP_CTRL, MAIN_VDP_DISPLAY_ON);
    dk::set_interrupt_mask(0);
    g_DebugPause = 0;
    g_UnkStart_F610 = 1;
    g_StartLives = 2;
    g_StartCredits = 6;
    g_StartWeaponType = 0;
    g_StartWeaponLvl = OPT_START_WEAPON_LVL(0);   /* 0; the native runner's weapon_level option (R24) */
    g_UnkStart_F61A = 1;
    g_UnkStart_F61C = 1;
    g_StartEnemyScale = 0x100;                    /* x1.0 */
    g_StartBossScale = 0x100;
    g_IntroNoSkip = 0xFF;                         /* the first attract pass can't be skipped */

    const u32 *order;
    dk::Flags ended{};

Main_Attract:
    d0 = (d0 & 0xFFFF0000u) | MAIN_MUSIC_TITLE;
    Sound_PlayMusic(&d0, &d1, &d7, a1, a2, a3, a4, a5, a6);
    g_MusicLock = 0xFF;
    Scene_RunList(&d1, &d3, &d4, &d5, &d6, main_table(Tbl_00A152), &a1, &a2, &a3, &a4, &a5, &a6);
    g_IntroNoSkip = 0;

Main_Title:
    d0 = (d0 & 0xFFFF0000u) | MAIN_MUSIC_TITLE;
    Sound_PlayMusic(&d0, &d1, &d7, a1, a2, a3, a4, a5, a6);
    g_MusicLock = 0xFF;
    Title_Run(&d1, &d3, &d4, &d5, &d6, &a1, &a2, &a3, &a4, &a5, &a6);
    if (g_SceneResult != MAIN_RESULT_START) {
        Scene_RunList(&d1, &d3, &d4, &d5, &d6, main_table(Tbl_00A136), &a1, &a2, &a3, &a4, &a5, &a6);
        if (g_SceneResult == MAIN_RESULT_START)
            goto Main_Title;
        goto Main_Attract;
    }

    /* Main_NewGame */
    PlayerSelect_Run(&d1, &d3, &d4, &d5, &d6, &a2, &a3, &a4, &a5, &a6);
    g_MusicLock = 0;
    g_StartPad = 0;
    g_SectionId = OPT_START_SECTION(0);           /* 0; the native runner's start_stage option */
    a1 = Player_NewGameSetup();

Main_PlayLoop:
    order = main_table(Tbl_009FFC);

Main_RunSection:
    Section_RunNext(&d1, &d3, &d4, &d5, &d6, order, &a1, &a2, &a3, &a4, &a5, &a6, &ended);
    if (ended.z)
        goto Main_Ending;                         /* no entry for this section: the game is won */
    if (g_SceneResult != MAIN_RESULT_GAME_OVER)
        goto Main_PlayLoop;
    d0 = (d0 & 0xFFFF0000u) | MAIN_MUSIC_CONTINUE;
    Sound_PlayMusic(&d0, &d1, &d7, a1, a2, a3, a4, a5, a6);
    {
        /* move.w g_SectionId,-(a7) / jsr Continue_Run / move.w (a7)+,g_SectionId: on the stack,
           so the continue screen runs at the asm's stack depth */
        dk::push_word(g_SectionId);
        Continue_Run(&d1, &d3, &d4, &d5, &d6, &a2, &a3, &a4, &a5, &a6);
        PTR(const u16) saved;
        saved.v = dk::stack_pointer();
        g_SectionId = *(const u16 *)saved;
        dk::set_stack_pointer(saved.v + 2);
    }
    if (g_SceneResult == MAIN_RESULT_START) {
        uint32_t a0 = 0;                          /* Player_InitRecords leaves g_Player2 there */
        a1 = Player_InitRecords(&a0);
        order = main_table(Tbl_00A05C);           /* the same section again */
        goto Main_RunSection;
    }

    /* Main_NoContinue */
    GameOver_Run(&d1, &d3, &d4, &d5, &d6, &a1, &a2, &a3, &a4, &a5, &a6);
    if (g_SceneResult == MAIN_RESULT_START)
        goto Main_Title;
    goto Main_Attract;

Main_Ending:
    d0 = (d0 & 0xFFFF0000u) | MAIN_MUSIC_ENDING;
    Sound_PlayMusic(&d0, &d1, &d7, a1, a2, a3, a4, a5, a6);
    Ending_AsylumGate(&d1, &d3, &d4, &d5, &d6, &a1, &a2, &a3, &a4, &a5, &a6);
    Ending_FreezeCell(&d1, &d3, &d4, &d5, &d6, &a1, &a2, &a3, &a4, &a5, &a6);
    Ending_Epilogue(&d1, &d3, &d4, &d5, &d6, &a1, &a2, &a3, &a4, &a5, &a6);
    goto Main_Attract;
}
