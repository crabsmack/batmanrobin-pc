#include "game.h"

/* Op4_Fork / Op4_ForkAbs / Op4_ForkAbsAndEnd ($00609E-$0060BB): fork a child a4 object
 * (VM4_Fork) running a relative or absolute script; the child ops after them then edit a5. */

/* Op4_Fork - a4 op: forks a child a4 object running the script at the operand's address plus its
 * value, and continues with the next op; a full pool ends this run.
 * In:  script = word child-script offset; d2-d5 = the spawn radius / heading / x / y; a6 = the
 *      spawner
 * Out: a3 = the child's script; a5 = the child; VM4_Fork's d0 / a0 / a1 and condition codes;
 *      VM4_Next, or rts (the end of the run) when the pool is full
 * Evidence: movea.l a4,a3 / adda.w (a4)+,a3 / bsr.w VM4_Fork / beq.w VM4_Next / rts; 161 script
 *           uses, e.g. skid dust $10D9A (`609E 0022 5E3A FFCD FFCD`: then Op4_ChildRangeVelY),
 *           $14BD2; 151 corpus runs
 */
void Op4_Fork(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1,
              uint32_t *a3, const u16 **script, uint32_t *a5, const Vm4Obj *self, dk::Flags *ccr)
{
    *a3 = dk::addr(*script) + (uint32_t)(int32_t)(int16_t)(uint16_t)**script;
    (*script)++;
    VM4_Fork(d0, d2, d3, d4, d5, a0, a1, *a3, a5, self, ccr);
    if (ccr->z)
        dk::jump(at_VM4_Next);
}

/* Op4_ForkAbs - a4 op: as Op4_Fork with the child's script at an absolute address.
 * In:  script = long child-script address; d2-d5; a6 = the spawner
 * Out: as Op4_Fork
 * Evidence: movea.l (a4)+,a3 / bsr.w VM4_Fork / beq.w VM4_Next / rts; script uses $28F74
 *           (`60AC 0001 4B98`), $28716; 40 corpus runs
 */
void Op4_ForkAbs(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0, uint32_t *a1,
                 uint32_t *a3, const u16 **script, uint32_t *a5, const Vm4Obj *self, dk::Flags *ccr)
{
    *a3 = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    VM4_Fork(d0, d2, d3, d4, d5, a0, a1, *a3, a5, self, ccr);
    if (ccr->z)
        dk::jump(at_VM4_Next);
}

/* Op4_ForkAbsAndEnd - a4 op: forks a child running the script at an absolute address and ends
 * this run (the parent resumes next frame from its saved pc).
 * In:  script = long child-script address; d2-d5; a6 = the spawner
 * Out: a3 = the child's script; then VM4_Fork's (its rts ends the run)
 * Evidence: movea.l (a4)+,a3 / bra.s VM4_Fork; script use $3B75A (`60B8 0001 4A20`); 3 corpus runs
 */
void Op4_ForkAbsAndEnd(uint32_t *d0, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t *a0,
                       uint32_t *a1, uint32_t *a3, const u16 **script, uint32_t *a5, uint32_t a6)
{
    (void)d0, (void)d2, (void)d3, (void)d4, (void)d5, (void)a0, (void)a1, (void)a5, (void)a6;
    *a3 = (uint32_t)(uint16_t)(*script)[0] << 16 | (uint16_t)(*script)[1];
    *script += 2;
    dk::jump(at_VM4_Fork);
}
