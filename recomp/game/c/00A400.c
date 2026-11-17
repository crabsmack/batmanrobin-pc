#include "game.h"

/* Pad_ReadPort - reads a 3-button pad with the TH toggle: Start/A with TH low, C B Right Left Down Up
 * with TH high; active-high, and opposite directions held together both dropped.
 * In:  port (a4) = the pad's data register
 * Out: the buttons: bit 7 Start, 6 A, 5 C, 4 B, 3 Right, 2 Left, 1 Down, 0 Up (d0's low byte); TH
 *      left high
 * Evidence: move.b #0,(a4) / nop / nop / move.b (a4),d0 / lsl.b #2 / move.b #$40,(a4) / andi.b #$C0 /
 *           move.b (a4),d1 / andi.b #$3F / or.b / not.b; eor of a pair when both bits are set; only
 *           caller Pad_Update
 */
uint8_t Pad_ReadPort(uint32_t port)
{
    hw::port_write8(port, 0x00);                  /* TH low (the two nops: the line settles) */
    uint8_t b = (uint8_t)(hw::port_read8(port) << 2);
    hw::port_write8(port, 0x40);                  /* TH high */
    b &= 0xC0;                                    /* Start, A */
    b |= (uint8_t)(hw::port_read8(port) & 0x3F);  /* C B R L D U */
    b = (uint8_t)~b;
    if ((b & 0x03) == 0x03)
        b ^= 0x03;                                /* up and down: neither */
    if ((b & 0x0C) == 0x0C)
        b ^= 0x0C;                                /* left and right: neither */
    return b;
}
