#include "game.h"

/* Spawn_SetTimer0FromEntry - spawner tail: sets the new object's timer 0 from the next word of
 * the trigger/wave entry
 * In:  entry = the entry's words after the callback (a1), vm = the new object's VM block (a4,
 *      left by Obj_SpawnEnemyOrQuit)
 * Out: entry past the word; Z set (moveq #0,d0: the spawn succeeded; d0 itself is never read)
 * Evidence: move.w (a1)+,$1A(a4) / moveq #0,d0 / rts; jmp operands at $03130A, $03179E, $0317F6
 *           in inline spawners (lea Scr2_x / Obj_SpawnEnemyOrQuit); TAS f67325-f75173 (37). The
 *           prototype gains &ccr for the measured ccout(Z) (registers unchanged)
 */
void Spawn_SetTimer0FromEntry(const u16 **entry, Vm2Block *vm, dk::Flags *ccr)
{
    vm->timer[0] = *(*entry)++;
    *ccr = dk::Flags::test(0, 4, ccr->x);   /* moveq #0,d0 */
}

/* Spawn_SetTimer1FromEntry - spawner tail: sets the new object's timer 1 from the next word of
 * the entry (the flying-section body $027CA2 gosubs vector 1 when it runs out: the attack delay)
 * In:  entry = the entry's words after the callback (a1), vm = the new object's VM block (a4)
 * Out: entry past the word; Z set (moveq #0,d0; d0 never read)
 * Evidence: move.w (a1)+,$1C(a4) / moveq #0,d0 / rts; jmp from 30+ flying-section spawners
 *           ($029EFC, $029F7A, $02A146 ... and the jmp operands at $0312A8, $03159C, $0315FC).
 *           The prototype gains &ccr for the measured ccout(Z)
 */
void Spawn_SetTimer1FromEntry(const u16 **entry, Vm2Block *vm, dk::Flags *ccr)
{
    vm->timer[1] = *(*entry)++;
    *ccr = dk::Flags::test(0, 4, ccr->x);   /* moveq #0,d0 */
}
