#include "game.h"

/* A layer's animated tiles and plane A's camera-follow tail ($0126D2-$0127CF). */

/* the low word of a data register, the upper word kept (a .w move) */
static inline void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* Layer_SetAnimTiles - sets a layer's animated tiles: the VRAM base from the tile word, the frame
 * list and art from a header of two self-relative longs (as the last two of Layer_Init's).
 * In:  tile = the tile word (its index the low 14 bits); header = the two longs (frame list: a
 *      count word, then 3 words a frame; tile art); layer
 * Out: layer->anim_vram = index x 32, anim_count, anim_list (past the count), anim_art
 * Evidence: andi.w #$3FFF,d0 / lsl.w #5 / move.w d0,$4A(a6) / movea.l a0,a1 / adda.l (a0)+,a1 /
 *           move.w (a1)+,$42(a6) / move.l a1,$1C(a6) / ... / move.l a1,$20(a6); the stage scene $047BD0
 *           (a0 = Tbl_0B29C6, d0 = $3C6); TAS 128791, scenario-level-skip 14046
 */
void Layer_SetAnimTiles(uint16_t tile, const s32 *header, MapLayer *layer)
{
    layer->anim_vram = (uint16_t)((tile & 0x3FFF) << 5);
    PTR(const u16) list;
    list.v = dk::addr(&header[0]) + (uint32_t)(int32_t)header[0];
    layer->anim_count = *list.get();
    layer->anim_list.v = list.v + 2;
    layer->anim_art.v = dk::addr(&header[1]) + (uint32_t)(int32_t)header[1];
}

/* Layer_AnimateTiles - a layer's per-frame tile animation: every anim_delay + 1 frames the next of
 * its frames, whose tiles get a DMA queue entry (the cost length / 32 + 1).
 * In:  layer (anim_list, anim_art, anim_vram, anim_count / frame / timer / delay)
 * Out: d1 = anim_count (0: nothing); on a new frame: anim_frame, anim_timer; art = its art,
 *      entry = past its 3 words, d2 = the VDP write command, d3 = the $93/$94 length registers, d1 =
 *      the cost; when DMA_Reserve takes it: d4 = the $97/$95 and d5 the $96 source registers, the
 *      queue entry, g_DMAQueuePtr
 * Evidence: subq.w #1,$46(a6) / bpl / move.w $48(a6),$46(a6); frame * 6 into +$1C; move.l a4,d4 /
 *           lsr.l #1 / andi.l #$7F00FF / ori.l #$97009500 ... ori.l #$40000080 / move.w a0,(g_DMAQueuePtr).w;
 *           the stage loops $01FECC, $033398 and $047BD0 every frame (25 corpus runs)
 */
void Layer_AnimateTiles(uint16_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint16_t *d5, uint32_t a0,
                        uint32_t *art, uint32_t *entry, MapLayer *layer)
{
    *d1 = layer->anim_count;
    if (*d1 == 0)
        return;
    layer->anim_timer -= 1;
    if (layer->anim_timer >= 0)
        return;
    layer->anim_timer = (int16_t)layer->anim_delay;
    uint16_t frame = (uint16_t)(layer->anim_frame + 1);
    if (!((int16_t)frame < (int16_t)*d1))
        frame = 0;
    layer->anim_frame = frame;
    const u16 *e = layer->anim_list.get() + 3 * frame;
    *entry = dk::addr(e + 3);
    *art = layer->anim_art.raw() + (uint32_t)(int32_t)(int16_t)e[0];
    set_low_word(d2, (uint16_t)(layer->anim_vram + e[1]));
    uint16_t bytes = e[2];
    set_low_word(d3, bytes);
    *d1 = (uint16_t)((bytes >> 5) + 1);
    uint32_t d0 = 1;
    DMA_Reserve(&d0, *d1, &a0);
    if (d0 != 0)                             /* bne: d0 = -1, nothing reserved */
        return;
    *d4 = *art >> 1;                         /* the source as a word address */
    *d5 = (uint16_t)*d4;
    *d4 = (*d4 & 0x7F00FFu) | 0x97009500u;
    *d5 = (uint16_t)((*d5 >> 8) | 0x9600);
    *d3 = (*d3 & 0xFFFFu) << 7;              /* the length in words, split into $94 / $93 */
    *d3 = (*d3 & 0xFFFF0000u) | ((*d3 & 0xFFFFu) >> 8);
    *d3 = (*d3 & 0xFF00FFu) | 0x94009300u;
    uint32_t cmd = (*d2 & 0xFFFFu) << 2;
    cmd = (cmd & 0xFFFF0000u) | ((cmd & 0xFFFFu) >> 2);
    *d2 = ((cmd << 16) | (cmd >> 16)) | 0x40000080u;   /* VRAM write + DMA */
    PTR(u16) at;
    at.v = a0;
    u16 *q = at.get();
    q[0] = (uint16_t)(*d4 >> 16);
    q[1] = (uint16_t)*d4;
    q[2] = *d5;
    q[3] = (uint16_t)(*d3 >> 16);
    q[4] = (uint16_t)*d3;
    q[5] = (uint16_t)(*d2 >> 16);
    q[6] = (uint16_t)*d2;
    g_DMAQueuePtr = (uint16_t)dk::addr(q + 7);
}

/* Camera_TrackPlayersXLayerA - Camera_TrackPlayersX, then plane A's pending scroll is the view
 * position minus the layer's (Layer_ScrollStep applies it).
 * In:  none
 * Out: Camera_TrackPlayersX's d1/d2; layer = g_LayerA; its pending_x / pending_y = g_ViewX /
 *      g_ViewY's integer word minus its x / y
 * Evidence: jsr Camera_TrackPlayersX / lea (g_LayerA).l,a6 / move.w (g_ViewX).l,d0 / sub.w (a6),d0 /
 *           move.w d0,$4(a6), the same for y; the tail of the gameplay frame variant $009BD4 (49 runs)
 */
void Camera_TrackPlayersXLayerA(uint32_t *d1, uint32_t *d2, uint32_t *layer)
{
    Camera_TrackPlayersX(d1, d2);
    MapLayer *a = (MapLayer *)g_LayerA;
    *layer = dk::addr(a);
    a->pending_x = (int16_t)((uint16_t)(g_ViewX >> 16) - a->x);
    a->pending_y = (int16_t)((uint16_t)(g_ViewY >> 16) - a->y);
}
