/* input_script.cpp - the pads per runner frame for the native runner's --input-script (phase 5 P6):
 * the input-only part of the engine's script format, read and ticked exactly as the engine's
 * runner does, so a corpus run plays the same pads on the native machine as on stepped-gen.
 *
 *   PADS <p1> [<p2> ..]  every listed player's whole held mask (hex, GPAD_* bits)
 *   VPAD <p1> [<p2>]     one more reference frame's pads for this runner frame's later V-ints
 *   WAIT <n>             hold for n frames
 *   PLAYER N / HOLD B / RELEASE [B] / PRESS B n, EXIT [code]
 *   WRITE_RAM8/16/32 <addr> <value>   (hex) a work-RAM write where the tick reaches it, as the
 *                        engine's runner applies it: a scenario's `@poke` (kit 0.11.9, ruling R24),
 *                        stored at its own width through dk_mem.h as the C stores (game_mem.cpp)
 * Anything else that reads or writes game state (ASSERT_/WAIT_RAM*, states, dumps, screenshots) is
 * refused, and a branch's script must be input-only (runner.cpp branch_script_seek, as main.c's).
 *
 * Adapted from segagenesisrecomp (runner/input_script.c), PolyForm Noncommercial License 1.0.0,
 * Copyright (c) 2026 Matthew Stan (https://polyformproject.org/licenses/noncommercial/1.0.0).
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "native_machine.h"

namespace {

enum Op { OP_WAIT, OP_HOLD, OP_RELEASE, OP_PADS, OP_PRESS, OP_EXIT, OP_VPAD, OP_WRITE_RAM8, OP_WRITE_RAM16,
          OP_WRITE_RAM32 };

struct ScriptOp {
    Op op;
    uint32_t arg32, arg32b;
    uint16_t button_mask;
    uint16_t pads[4];
};

const int PLAYERS = 4, BUTTONS = 12;
std::vector<ScriptOp> s_ops;
size_t s_pc;
int s_active;
uint64_t s_wait_until;
uint16_t s_held[PLAYERS];
unsigned s_parse_player;
int s_exit_pending, s_exit_code;
uint64_t s_press_until[PLAYERS][BUTTONS];
std::vector<uint16_t> s_vq;   /* this frame's VPAD queue, two pads per entry */
uint64_t s_tick_frame;

int ieq(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
    return *a == *b;
}

int parse_button(const char *s, uint16_t *out)
{
    static const char *names[BUTTONS] = { "UP", "DOWN", "LEFT", "RIGHT", "B", "C", "A", "START", "X", "Y", "Z", "MODE" };
    for (int i = 0; i < BUTTONS; i++)
        if (ieq(s, names[i])) { *out = (uint16_t)(1u << i); return 1; }
    return 0;
}

int parse_line(char *line, int line_no)
{
    char *hash = strchr(line, '#');
    if (hash) *hash = '\0';
    char *end = line + strlen(line);
    while (end > line && isspace((unsigned char)end[-1])) *--end = '\0';
    char *p = line;
    while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) return 1;
    ScriptOp o;
    memset(&o, 0, sizeof o);
    char tok[64], a1[260], a2[260];
    int n = sscanf(p, "%63s %259s %259s", tok, a1, a2);
    if (n < 1) return 1;
    if (ieq(tok, "PLAYER")) {
        if (n != 2 || strlen(a1) != 1 || a1[0] < '1' || a1[0] > '4') goto bad;
        s_parse_player = (unsigned)(a1[0] - '1');
        return 1;
    }
    if (ieq(tok, "PADS")) {
        char pv[4][32];
        int np = sscanf(p, "%63s %31s %31s %31s %31s", tok, pv[0], pv[1], pv[2], pv[3]) - 1;
        if (np < 1) goto bad;
        o.op = OP_PADS;
        o.arg32 = (uint32_t)np;
        for (int i = 0; i < np; i++) {
            char *e;
            unsigned long v = strtoul(pv[i], &e, 16);
            if (*e || v > 0xFFFu) goto bad;
            o.pads[i] = (uint16_t)v;
        }
    } else if (ieq(tok, "VPAD")) {
        char pv[2][32];
        int np = sscanf(p, "%63s %31s %31s", tok, pv[0], pv[1]) - 1;
        if (np < 1) goto bad;
        o.op = OP_VPAD;
        for (int i = 0; i < 2; i++) {
            char *e = nullptr;
            unsigned long v = i < np ? strtoul(pv[i], &e, 16) : 0;
            if ((i < np && *e) || v > 0xFFFu) goto bad;
            o.pads[i] = (uint16_t)v;
        }
    } else if (ieq(tok, "WAIT") && n >= 2) {
        o.op = OP_WAIT;
        o.arg32 = (uint32_t)strtoul(a1, nullptr, 0);
    } else if (ieq(tok, "HOLD") && n >= 2) {
        if (!parse_button(a1, &o.button_mask)) goto bad;
        o.op = OP_HOLD;
        o.arg32 = s_parse_player;
    } else if (ieq(tok, "RELEASE")) {
        o.op = OP_RELEASE;
        o.arg32 = s_parse_player;
        if (n >= 2) {
            if (!parse_button(a1, &o.button_mask)) goto bad;
        } else {
            o.button_mask = 0xFFFF;
        }
    } else if (ieq(tok, "PRESS") && n >= 3) {
        if (!parse_button(a1, &o.button_mask)) goto bad;
        o.op = OP_PRESS;
        o.arg32 = s_parse_player;
        o.arg32b = (uint32_t)strtoul(a2, nullptr, 0);
    } else if (ieq(tok, "EXIT")) {
        o.op = OP_EXIT;
        o.arg32 = n >= 2 ? (uint32_t)strtoul(a1, nullptr, 0) : 0u;
    } else if ((ieq(tok, "WRITE_RAM8") || ieq(tok, "WRITE_RAM16") || ieq(tok, "WRITE_RAM32")) && n >= 3) {
        o.op = ieq(tok, "WRITE_RAM8") ? OP_WRITE_RAM8 : ieq(tok, "WRITE_RAM16") ? OP_WRITE_RAM16 : OP_WRITE_RAM32;
        char *e1, *e2;
        unsigned long a = strtoul(a1, &e1, 16), v = strtoul(a2, &e2, 16);
        unsigned width = o.op == OP_WRITE_RAM8 ? 1u : o.op == OP_WRITE_RAM16 ? 2u : 4u;
        /* work RAM only ($E00000-$FFFFFF, its mirrors): the native machine has no other writable state
           a script could reach the way the engine's bus does; a word or long at an even address */
        if (*e1 || *e2 || (a & 0xFFFFFFu) < 0xE00000u || (width > 1 && (a & 1u)) ||
            ((a & 0xFFFFu) + width > 0x10000u) || (width < 4 && v >> (8 * width)))
            goto bad;
        o.arg32 = (uint32_t)(0xFF0000u | (a & 0xFFFFu));
        o.arg32b = (uint32_t)v;
    } else {
        fprintf(stderr, "input_script: line %d: %s: the native runner takes input-only scripts\n", line_no, p);
        return 0;
    }
    s_ops.push_back(o);
    return 1;
bad:
    fprintf(stderr, "input_script: parse error at line %d: %s\n", line_no, p);
    return 0;
}

}  // namespace

extern "C" int ns_load(const char *path)
{
    s_ops.clear();
    s_pc = 0;
    s_active = 0;
    s_wait_until = 0;
    memset(s_held, 0, sizeof s_held);
    s_parse_player = 0;
    s_exit_pending = 0;
    memset(s_press_until, 0, sizeof s_press_until);
    s_vq.clear();
    if (!path) return 0;
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "input_script: cannot open '%s'\n", path);
        return 0;
    }
    char line[512];
    int line_no = 0;
    while (fgets(line, sizeof line, f)) {
        line_no++;
        if (!parse_line(line, line_no)) { fclose(f); return 0; }
    }
    fclose(f);
    s_active = 1;
    return 1;
}

extern "C" int ns_active(void) { return s_active; }

/* 1 when the loaded script writes RAM (input_script.c input_script_mutates_state): a branch's seek
   ticks its script through the shared frames, which would apply every earlier write at once. */
extern "C" int ns_mutates_state(void)
{
    for (const ScriptOp &o : s_ops)
        if (o.op == OP_WRITE_RAM8 || o.op == OP_WRITE_RAM16 || o.op == OP_WRITE_RAM32) return 1;
    return 0;
}

extern "C" void ns_tick(uint64_t frame)
{
    if (!s_active) return;
    s_vq.clear();
    s_tick_frame = frame;
    for (unsigned p = 0; p < PLAYERS; ++p)
        for (unsigned b = 0; b < BUTTONS; ++b)
            if (s_press_until[p][b] && frame >= s_press_until[p][b]) {
                s_held[p] &= (uint16_t)~(1u << b);
                s_press_until[p][b] = 0;
            }
    if (frame < s_wait_until) return;
    while (s_pc < s_ops.size()) {
        const ScriptOp &o = s_ops[s_pc];
        switch (o.op) {
        case OP_WAIT:
            s_wait_until = frame + o.arg32;
            s_pc++;
            if (frame < s_wait_until) return;
            break;
        case OP_VPAD:
            s_vq.push_back(o.pads[0]);
            s_vq.push_back(o.pads[1]);
            s_pc++;
            break;
        case OP_PADS:
            for (uint32_t i = 0; i < o.arg32; ++i) {
                s_held[i] = o.pads[i];
                for (unsigned b = 0; b < BUTTONS; ++b) s_press_until[i][b] = 0;
            }
            s_pc++;
            break;
        case OP_HOLD:
            s_held[o.arg32] |= o.button_mask;
            for (unsigned b = 0; b < BUTTONS; ++b)
                if (o.button_mask & (1u << b)) s_press_until[o.arg32][b] = 0;
            s_pc++;
            break;
        case OP_RELEASE:
            s_held[o.arg32] &= (uint16_t)~o.button_mask;
            for (unsigned b = 0; b < BUTTONS; ++b)
                if (o.button_mask & (1u << b)) s_press_until[o.arg32][b] = 0;
            s_pc++;
            break;
        case OP_PRESS:
            for (unsigned b = 0; b < BUTTONS; ++b) {
                if (!(o.button_mask & (1u << b))) continue;
                if (o.arg32b) s_held[o.arg32] |= o.button_mask;
                else s_held[o.arg32] &= (uint16_t)~o.button_mask;
                s_press_until[o.arg32][b] = o.arg32b ? frame + o.arg32b : 0;
            }
            s_pc++;
            break;
        case OP_EXIT:
            s_exit_pending = 1;
            s_exit_code = (int)o.arg32;
            fprintf(stderr, "[input_script] EXIT %d at frame %llu\n", s_exit_code, (unsigned long long)frame);
            s_pc++;
            return;
        case OP_WRITE_RAM8:
        case OP_WRITE_RAM16:
        case OP_WRITE_RAM32:
            if (o.op == OP_WRITE_RAM8) nm_ram_write8(o.arg32, (uint8_t)o.arg32b);
            else if (o.op == OP_WRITE_RAM16) nm_ram_write16(o.arg32, (uint16_t)o.arg32b);
            else nm_ram_write32(o.arg32, o.arg32b);
            fprintf(stderr, "[input_script] WRITE_RAM%d at frame %llu ($%06X=$%0*X)\n",
                    o.op == OP_WRITE_RAM8 ? 8 : o.op == OP_WRITE_RAM16 ? 16 : 32, (unsigned long long)frame,
                    (unsigned)o.arg32, o.op == OP_WRITE_RAM8 ? 2 : o.op == OP_WRITE_RAM16 ? 4 : 8, (unsigned)o.arg32b);
            s_pc++;
            break;
        }
    }
}

extern "C" uint32_t ns_vpad_count(void) { return (uint32_t)(s_vq.size() / 2); }

extern "C" uint16_t ns_vpad(uint32_t i, int player)
{
    return i < ns_vpad_count() && player >= 0 && player < 2 ? s_vq[2 * i + (uint32_t)player] : 0;
}

/* The pads of the n-th reference frame after the current frame's own (its first and its VPAD
   queue), reading the script ahead without running it (input_script.c input_script_peek). */
extern "C" uint16_t ns_peek(uint32_t n, int player)
{
    if (player < 0 || player >= 2) return 0;
    uint16_t held = s_held[player];
    uint64_t rest = s_wait_until > s_tick_frame + 1 ? s_wait_until - s_tick_frame - 1 : 0;
    if (n < rest) return held;
    n -= (uint32_t)rest;
    int first_pending = 0;
    for (size_t pc = s_pc; pc < s_ops.size(); pc++) {
        const ScriptOp &o = s_ops[pc];
        switch (o.op) {
        case OP_PADS:
            if ((uint32_t)player < o.arg32) held = o.pads[player];
            first_pending = 1;
            break;
        case OP_VPAD:
            if (first_pending) {
                if (n == 0) return held;
                n--;
                first_pending = 0;
            }
            if (n == 0) return o.pads[player];
            n--;
            break;
        case OP_WAIT: {
            uint32_t w = o.arg32 ? o.arg32 : 1;
            uint32_t k = first_pending ? w : w - 1;
            if (n < k) return held;
            n -= k;
            first_pending = 1;
            break;
        }
        default:
            break;
        }
    }
    return 0;
}

/* How many reference frames ns_peek can still give (n below it reads the script; n at or past it
   is after the script's last frame). The same walk as ns_peek's, counted: the watchdog's bound on
   one sync interval (machine.cpp nm_wait_vint), since a run has no V-int past its input's end. */
extern "C" uint32_t ns_peek_frames(void)
{
    if (!s_active) return 0;
    uint64_t total = s_wait_until > s_tick_frame + 1 ? s_wait_until - s_tick_frame - 1 : 0;
    int first_pending = 0;
    for (size_t pc = s_pc; pc < s_ops.size(); pc++) {
        const ScriptOp &o = s_ops[pc];
        switch (o.op) {
        case OP_PADS:
            first_pending = 1;
            break;
        case OP_VPAD:
            total += first_pending ? 2u : 1u;
            first_pending = 0;
            break;
        case OP_WAIT: {
            uint32_t w = o.arg32 ? o.arg32 : 1;
            total += first_pending ? w : w - 1;
            first_pending = 1;
            break;
        }
        default:
            break;
        }
    }
    return total > 0xFFFFFFFFu ? 0xFFFFFFFFu : (uint32_t)total;
}

extern "C" uint16_t ns_player_mask(int player)
{
    return s_active && player >= 0 && player < PLAYERS ? s_held[player] : 0;
}

extern "C" int ns_should_exit(void) { return s_exit_pending; }
extern "C" int ns_exit_code(void) { return s_exit_code; }
