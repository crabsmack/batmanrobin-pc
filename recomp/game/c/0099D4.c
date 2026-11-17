#include "game.h"

enum {
    STAGE_HUD_OFS = 8,                 /* both HUD panels' home: the header's x/y + 8 ... */
    STAGE_HUD2_OFS_X = 0xD4,           /* ... and player 2's x + $D4 (the right-hand panel) */
};

/* the high (integer) word of a 16.16 long (move.w to the long's address) */
static void set_int_part(u32 &v, uint16_t w)
{
    v = (uint32_t)w << 16 | ((uint32_t)v & 0xFFFFu);
}

/* a word moved into the low word of a data register (the rest kept) */
static void set_reg_word(uint32_t *r, uint16_t w)
{
    *r = (*r & 0xFFFF0000u) | w;
}

/* one player's record: its HUD home, start offset and slot object; a player still in play keeps
   its place (PLAYER_RESPAWN: spawn at +$2E/+$32, not the join point) */
static void stage_setup_player(PlayerRec *rec, uint16_t hud_x, uint16_t hud_y, uint16_t start_x,
                               uint16_t start_y, uint32_t slot)
{
    if (rec->flags & PLAYER_IN_PLAY)
        rec->flags = (uint8_t)(rec->flags | PLAYER_RESPAWN);
    rec->hud_x = (int16_t)hud_x;
    rec->hud_y = (int16_t)hud_y;
    rec->x = (int16_t)start_x;
    rec->y = (int16_t)start_y;
    rec->object = (int16_t)(uint16_t)slot;        /* move.w a3,$2C(a2): the object's handle */
    PTR(Vm2Obj) obj;
    obj.v = slot;
    obj->player = ADDR68(rec);
}

/* Scene_InitStage - starts a gameplay section: the common scene init, then the stage header: the
 * movement area, two player-slot objects (Player_JoinOrSpawn), both players' HUD homes and start
 * positions, the join offset, play mode, terrain routines and camera start; the game-over check
 * as the frame callback, the HUD, and the fade in.
 * In:  header (a6) = a SceneHeader, then a StageHeader
 * Out: header (a6) past both; g_MoveArea*, the player records (+$A/+$C, +$2E/+$32, +$2C, +$25
 *      bit 1 when in play), the slot objects' +$1E; g_FrameCallback = SceneCb_CheckGameOver; d0-d7
 *      = the header words as the movem.w / addi.w leave them (d0's low byte then Fade_StartIn's),
 *      a2 = g_Player2, a3 / a4 = the two slot objects
 * Evidence: bsr Scene_InitCommon / movem.w (a6)+,d0-d3 / sub.w / move.l a6,-(a7) / 2x lea
 *           Player_JoinOrSpawn,a0 / bsr Obj_NewClear / movea.l (a7)+,a6 / movem.w (a6)+,d0-d7 /
 *           addi.w #8,#8,#$D4,#8 / per record btst #0,$25 / bset #1 / ... / bsr HUD_Init / bra
 *           Fade_StartIn; 16 section entry points; Scene_InitCommon's calls are interrupted, so
 *           D2 checks it
 */
void Scene_InitStage(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5,
                     uint32_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6)
{
    uint16_t w1 = (uint16_t)*d1, w6 = (uint16_t)*d6;
    Scene_InitCommon(d0, &w1, d2, &w6, d7, a2, a3, a4, a6);
    set_reg_word(d1, w1);
    set_reg_word(d6, w6);

    PTR(const StageHeader) at;
    at.v = *a6;
    const StageHeader *stage = at;
    /* movem.w (a6)+,d0-d3: sign-extended */
    *d0 = (uint32_t)(int32_t)stage->move_left;
    *d1 = (uint32_t)(int32_t)stage->move_right;
    *d2 = (uint32_t)(int32_t)stage->move_top;
    *d3 = (uint32_t)(int32_t)stage->move_bottom;
    set_int_part(g_MoveAreaOfsX, (uint16_t)*d0);
    set_reg_word(d1, (uint16_t)(*d1 - *d0));
    g_MoveAreaW = (uint16_t)*d1;
    set_int_part(g_MoveAreaOfsY, (uint16_t)*d2);
    set_reg_word(d3, (uint16_t)(*d3 - *d2));
    g_MoveAreaH = (uint16_t)*d3;

    /* the two player-slot objects; a6 saved on the stack around them as the asm does */
    uint32_t header = *a6 + 8;
    dk::push_long(header);
    uint32_t obj = header;
    Obj_NewClear(d0, at_Player_JoinOrSpawn, &obj);
    *a3 = obj;
    Obj_NewClear(d0, at_Player_JoinOrSpawn, &obj);
    *a4 = obj;
    header = dk::pop_long();

    *d0 = (uint32_t)(int32_t)stage->hud[0].x;     /* movem.w (a6)+,d0-d7 */
    *d1 = (uint32_t)(int32_t)stage->hud[0].y;
    *d2 = (uint32_t)(int32_t)stage->hud[1].x;
    *d3 = (uint32_t)(int32_t)stage->hud[1].y;
    *d4 = (uint32_t)(int32_t)stage->start[0].x;
    *d5 = (uint32_t)(int32_t)stage->start[0].y;
    *d6 = (uint32_t)(int32_t)stage->start[1].x;
    *d7 = (uint32_t)(int32_t)stage->start[1].y;
    set_reg_word(d0, (uint16_t)(*d0 + STAGE_HUD_OFS));
    set_reg_word(d1, (uint16_t)(*d1 + STAGE_HUD_OFS));
    set_reg_word(d2, (uint16_t)(*d2 + STAGE_HUD2_OFS_X));
    set_reg_word(d3, (uint16_t)(*d3 + STAGE_HUD_OFS));
    stage_setup_player((PlayerRec *)g_Player1, (uint16_t)*d0, (uint16_t)*d1, (uint16_t)*d4, (uint16_t)*d5, *a3);
    PlayerRec *p2 = (PlayerRec *)g_Player2;
    stage_setup_player(p2, (uint16_t)*d2, (uint16_t)*d3, (uint16_t)*d6, (uint16_t)*d7, *a4);
    *a2 = ADDR68(p2);

    g_JoinOfsX = stage->join_x;
    g_JoinOfsY = stage->join_y;
    g_PlayMode = stage->play_mode;
    g_TerrainFn = stage->terrain_fn;
    g_TerrainFnAlt = stage->terrain_fn_alt;
    set_int_part(g_CameraX, stage->camera_x);
    set_int_part(g_CameraY, stage->camera_y);
    g_FrameCallback = at_SceneCb_CheckGameOver;
    *a6 = header - 8 + sizeof(StageHeader);
    uint32_t a0 = 0;
    HUD_Init(&a0);
    dk::jump(at_Fade_StartIn);                    /* bra: it sets d0's low byte */
}
