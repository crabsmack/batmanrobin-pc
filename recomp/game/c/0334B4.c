#include "game.h"

/* the sine table: 512 signed words over a full turn (the cosine $100 bytes on) (ROM) */
static constexpr uint32_t Tbl_059B22 = 0x059B22u;

/* Flight_BuildWobbleTable - builds the cloud bank's wobble table at g_SceneWorkBuf+$400: 256 words
 * = one sine period (every second word of Tbl_059B22, high byte / 16: +-8 px), then 224 words
 * repeating its start, so any 224-line window reads without wrapping
 * In:  -
 * Out: g_FlightWobble[0..479]; d7 = $FFFF, a0/a1 past the second copy (Flight_CopyWobbleWords)
 * Evidence: lea Tbl_059B22,a0 / lea g_SceneWorkBuf+$400,a1 / move.w #$FF,d7 / bsr.w
 *           Flight_CopyWobbleWords / lea Tbl_059B22,a0 / move.w #$DF,d7, then the fall-through
 *           into Flight_CopyWobbleWords (the next file); called once by Flight_StartClouds (TAS
 *           f46290)
 */
void Flight_BuildWobbleTable(uint16_t *d7, uint32_t *a0, uint32_t *a1)
{
    *a0 = Tbl_059B22;
    *a1 = dk::addr(g_FlightWobble);
    *d7 = FLIGHT_WOBBLE_PERIOD - 1;
    Flight_CopyWobbleWords(d7, a0, a1);
    *a0 = Tbl_059B22;
    *d7 = FLIGHT_WOBBLE_LINES - 1;
    dk::jump(at_Flight_CopyWobbleWords);   /* falls into it: the second copy */
}
