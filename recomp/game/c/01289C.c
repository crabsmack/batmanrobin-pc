#include "game.h"

/* Layer_MoveX - moves a layer's x by some pixels without drawing: its block column steps by the
 * block edges crossed, wrapping at the map's width.
 * In:  dx = signed pixels; layer
 * Out: layer->x, block_col
 * Evidence: move.w (a6),d1 / add.w d0,d1 / move.w d1,(a6); the whole blocks (lsr.w #4, negated for a
 *           move left) plus one when the offset inside the block wraps (sub.w / bcc on the low 4
 *           bits, of ~x going left); add.w $26(a6) / bpl / add.w $2E(a6) / cmp / blt / sub; the
 *           scene-script ops $7396/$73D6 and $032F14 before Layer_DrawScreen (TAS 21156, 67239)
 */
void Layer_MoveX(uint16_t dx, MapLayer *layer)
{
    uint16_t x = (uint16_t)(layer->x + dx);
    layer->x = x;
    int16_t blocks;
    if (!(dx & 0x8000u)) {
        blocks = (int16_t)(dx >> 4);
        if ((x & 0xF) < (dx & 0xF))          /* the offset wrapped past a block edge */
            blocks++;
    } else {
        uint16_t left = (uint16_t)-(int16_t)dx;
        blocks = (int16_t)-(int16_t)(left >> 4);
        if ((~x & 0xF) < (left & 0xF))
            blocks--;
    }
    int16_t width = layer->width;
    int16_t col = (int16_t)(blocks + layer->block_col);
    if (col < 0)
        col = (int16_t)(col + width);
    if (!(col < width))
        col = (int16_t)(col - width);
    layer->block_col = col;
}

/* Layer_MoveYWithCamera - moves the camera's y and the layer's (Layer_MoveY, the next file) by
 * some pixels.
 * In:  dy = signed pixels; layer (its address: a plain register for the callers' C)
 * Out: g_CameraY's integer word += dy; then Layer_MoveY
 * Evidence: add.w d0,(g_CameraY).l falling into Layer_MoveY; the scene setups $01FF0C and $048AF2
 *           (10 runs: TAS, idle, options, level-skip)
 */
void Layer_MoveYWithCamera(uint16_t dy, uint32_t layer)
{
    g_CameraY = ((uint32_t)(uint16_t)((g_CameraY >> 16) + dy) << 16) | (g_CameraY & 0xFFFFu);
    dk::jump(at_Layer_MoveY);
}
