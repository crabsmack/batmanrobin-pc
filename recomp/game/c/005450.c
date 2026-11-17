#include "game.h"

/* The a4 VM's timer, counter and ticker ops ($005450-$0054F3): the object's timer word, counter
 * byte and ticker (an accumulator byte that fires when its per-step rate carries it past 255),
 * set, counted down, and branched or waited on. */

/* NEXT: the handler the script's next word names, jumped to */
static void op_next(uint32_t *handler, const u16 **script)
{
    *handler = dk::code((int16_t)(uint16_t)*(*script)++);
    dk::jump(*handler);
}

/* the low word of a data register, the upper word kept (a .w operation) */
static void set_low_word(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }

/* `add.b src,dst`: the sum, and its condition codes (X = C) */
static uint8_t add_byte(uint8_t dst, uint8_t src, dk::Flags *ccr)
{
    uint8_t sum = (uint8_t)(dst + src);
    bool carry = dst + src > 0xFF;
    *ccr = dk::Flags{ carry, (sum & 0x80) != 0, sum == 0, ((dst ^ sum) & (src ^ sum) & 0x80) != 0, carry };
    return sum;
}

/* Op4_SetTimer - a4 op: sets the object's timer word.
 * In:  script = word frames
 * Out: self->timer; NEXT
 * Evidence: move.w (a4)+,$16(a6); counted by Op4_TimerBranchOut / Op4_TimerLoop; 24 a4-script
 *           uses ($1504E); 54 corpus runs
 */
void Op4_SetTimer(const u16 **script, Vm4Obj *self)
{
    self->timer = (int16_t)(uint16_t)*(*script)++;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_SetTimerRandom - a4 op: sets the object's timer to base + a random number below range.
 * In:  script = word range, word base
 * Out: d0 low word = the timer; self->timer; NEXT
 * Evidence: move.w (a4)+,d0 / bsr Rand_Below / add.w (a4)+,d0 / move.w d0,$16(a6); script use $46AC8;
 *           4 corpus runs
 */
void Op4_SetTimerRandom(uint32_t *d0, const u16 **script, Vm4Obj *self)
{
    set_low_word(d0, *(*script)++);
    Rand_Below(d0);
    set_low_word(d0, (uint16_t)(*d0 + *(*script)++));
    self->timer = (int16_t)(uint16_t)*d0;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_ChildSetTimer - a4 op: sets the timer word of the last spawned child.
 * In:  script = word frames; child = a5 (the object itself when none was spawned this run)
 * Out: child->timer; NEXT
 * Evidence: move.w (a4)+,$16(a5); script uses $25B7A, $25B98; 44 corpus runs
 */
void Op4_ChildSetTimer(const u16 **script, Vm4Obj *child)
{
    child->timer = (int16_t)(uint16_t)*(*script)++;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_ChildSetTimerRandom - a4 op: sets the child's timer to base + a random number below range.
 * In:  script = word range, word base; child = a5
 * Out: d0 low word = the timer; child->timer; NEXT (handler = the next op) with the condition
 *      codes of the store (an op after it that returns at once leaves them to the caller)
 * Evidence: Rand_Below(range) + base into $16(a5); script uses $36F9E, $36FC0; 29 corpus runs
 */
void Op4_ChildSetTimerRandom(uint32_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *child, dk::Flags *ccr)
{
    set_low_word(d0, *(*script)++);
    Rand_Below(d0);
    set_low_word(d0, (uint16_t)(*d0 + *(*script)++));
    child->timer = (int16_t)(uint16_t)*d0;
    *ccr = dk::Flags::test(*d0, 2, ccr->x);
    op_next(handler, script);
}

/* Op4_TimerBranchOut - a4 op: counts the timer down by one and branches when it runs out (the
 * count was 1 or less); otherwise continues past the offset.
 * In:  script = word offset
 * Out: self->timer - 1; Op4_Goto when out, else NEXT after the offset
 * Evidence: subq.w #1,$16(a6) / ble Op4_Goto / addq.l #2,a4 / NEXT; script uses $1514C, $26B8E;
 *           89 corpus runs
 */
void Op4_TimerBranchOut(uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    int16_t timer = self->timer;
    self->timer = (int16_t)(timer - 1);
    if (timer <= 1) {               /* ble after the subq: the exact count went to 0 or below */
        dk::jump(at_Op4_Goto);
        return;
    }
    (*script)++;
    op_next(handler, script);
}

/* Op4_TimerLoop - a4 op: counts the timer down by one and loops back (branches) while it has not
 * run out; once out continues past the offset.
 * In:  script = word offset (usually back to a motion/draw loop)
 * Out: self->timer - 1; Op4_SkipWord when out, else NEXT at the offset's target
 * Evidence: subq.w #1,$16(a6) / ble Op4_SkipWord / adda.w (a4),a4 / NEXT; script uses $1505A, $15098;
 *           48 corpus runs
 */
void Op4_TimerLoop(const u16 **script, Vm4Obj *self)
{
    int16_t timer = self->timer;
    self->timer = (int16_t)(timer - 1);
    if (timer <= 1) {
        dk::jump(at_Op4_SkipWord);
        return;
    }
    *script = (const u16 *)((const u8 *)*script + (int16_t)(uint16_t)**script);
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_SetCounterTicker - a4 op: sets the counter byte (the first word's low byte) and the ticker
 * (accumulator byte, rate byte).
 * In:  script = word count, word accumulator:rate
 * Out: d0 = the count word; self->counter, ticker, ticker_rate; NEXT
 * Evidence: move.w (a4)+,d0 / move.b d0,$19(a6) / move.w (a4)+,$1A(a6); script uses $14E40,
 *           $1A3FA; 6 corpus runs
 */
void Op4_SetCounterTicker(uint16_t *d0, const u16 **script, Vm4Obj *self)
{
    *d0 = *(*script)++;
    self->counter = (uint8_t)*d0;
    uint16_t ticker = *(*script)++;
    self->ticker = (uint8_t)(ticker >> 8);
    self->ticker_rate = (uint8_t)ticker;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_SetTicker - a4 op: sets the ticker: accumulator byte and per-step rate byte (it fires when
 * the byte sum carries: rate/256 per frame).
 * In:  script = word accumulator:rate
 * Out: self->ticker, ticker_rate; NEXT
 * Evidence: move.w (a4)+,$1A(a6); e.g. $26D52 sets $FF40 then Op4_TickerLoop; 71 corpus runs
 */
void Op4_SetTicker(const u16 **script, Vm4Obj *self)
{
    uint16_t ticker = *(*script)++;
    self->ticker = (uint8_t)(ticker >> 8);
    self->ticker_rate = (uint8_t)ticker;
    uint32_t handler;
    op_next(&handler, script);
}

/* Op4_CounterKillOut - a4 op: counts the counter byte down and deletes the object when it runs
 * out (it was 1 or 0); otherwise continues.
 * In:  script = the next op (no operand); d0/d2/a0/a1 as Op4_Kill takes them
 * Out: self->counter - 1; Op4_Kill when out, else NEXT
 * Evidence: subq.b #1,$19(a6) / bls Op4_Kill / NEXT; script uses $14E52, $1A412, $1A5C2 (an
 *           effect's life in frames); 6 corpus runs
 */
void Op4_CounterKillOut(uint16_t *d0, uint16_t *d2, uint32_t *a0, uint32_t *a1, const u16 **script, Vm4Obj *self)
{
    (void)d0, (void)d2, (void)a1;
    uint8_t counter = self->counter;
    self->counter = (uint8_t)(counter - 1);
    if (counter <= 1) {             /* bls: a borrow (it was 0) or zero (it was 1) */
        dk::jump(at_Op4_Kill);
        return;
    }
    op_next(a0, script);
}

/* Op4_CounterBranchOut - a4 op: counts the counter byte down and branches when it runs out;
 * otherwise continues past the offset.
 * In:  script = word offset
 * Out: self->counter - 1; Op4_Goto when out, else NEXT after the offset
 * Evidence: subq.b #1,$19(a6) / bls Op4_Goto / addq.l #2,a4 / NEXT; script use $359CA; 3 corpus runs
 */
void Op4_CounterBranchOut(uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    uint8_t counter = self->counter;
    self->counter = (uint8_t)(counter - 1);
    if (counter <= 1) {
        dk::jump(at_Op4_Goto);
        return;
    }
    (*script)++;
    op_next(handler, script);
}

/* Op4_WaitTicker - a4 op: waits on the ticker: adds the rate to the accumulator and continues on
 * the carry; otherwise ends the frame's run with this op as the resume pc (retried next frame).
 * In:  script = the next op (no operand)
 * Out: d0 = the rate; self->ticker += it; on the carry NEXT (the add.b's condition codes), else
 *      self->resume = this op and a return with the move.l's condition codes
 * Evidence: move.b $1B(a6),d0 / add.b d0,$1A(a6) / bcs VM4_Next / subq.l #2,a4 / move.l a4,$8(a6) /
 *           rts; script uses $14E46, $1A400, $1E262; 9 corpus runs
 */
void Op4_WaitTicker(uint8_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self, dk::Flags *ccr)
{
    *d0 = self->ticker_rate;
    self->ticker = add_byte(self->ticker, *d0, ccr);
    if (ccr->c) {
        op_next(handler, script);
        return;
    }
    (*script)--;
    self->resume = *script;
    *ccr = dk::Flags::test(dk::addr(*script), 4, ccr->x);
}

/* Op4_TickerLoop - a4 op: steps the ticker and loops back (branches) until it carries; on the
 * carry continues past the offset.
 * In:  script = word offset
 * Out: d0 = the rate; self->ticker += it; Op4_Goto while no carry, else NEXT after the offset
 * Evidence: move.b $1B(a6),d0 / add.b d0,$1A(a6) / bcc Op4_Goto / addq.l #2,a4 / NEXT; script uses
 *           $1A5B0, $1AA86, $1F220 (after Op4_SetTicker); 69 corpus runs
 */
void Op4_TickerLoop(uint8_t *d0, uint32_t *handler, const u16 **script, Vm4Obj *self)
{
    *d0 = self->ticker_rate;
    dk::Flags ccr;
    self->ticker = add_byte(self->ticker, *d0, &ccr);
    if (!ccr.c) {
        dk::jump(at_Op4_Goto);
        return;
    }
    (*script)++;
    op_next(handler, script);
}
