/* dk_verify_fns.h - the shadow tester's table of functions in C (the bridge writes it,
 * game/runtime/verify.cpp reads it). Written by the kit's recomp template. */
#ifndef DK_VERIFY_FNS_H
#define DK_VERIFY_FNS_H
#include <stdint.h>

/* out_mask: the registers the function hands back (bit r: d0-d7 = 0-7, a0-a6 = 8-14), each
 * compared as wide as out_bytes[r]; skip_mask: its clobbers() (never compared); flag_mask: the
 * condition codes it hands back (C 0, V 1, Z 2, N 3, X 4). Every register in neither mask must
 * come back as it went in: hand-written 68000 has no callee-saved set, so a register a function
 * leaves alone is one its callers may keep a value in. */
struct DkVerifyFn {
    uint32_t addr, end;
    void (*slot)(void);
    void (*gen)(void);
    uint16_t out_mask, skip_mask;
    uint8_t flag_mask;
    uint8_t out_bytes[16];
};
extern const DkVerifyFn dk_verify_fns[];
extern const int dk_verify_fn_count;

#endif /* DK_VERIFY_FNS_H */
