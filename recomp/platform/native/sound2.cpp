/* sound2.cpp - the native machine's sound for phase 6 (sound2.h): music_voices (R13, R14), fm_ladder
 * and fm_filter (R15), the audio counters (E6), the stems and the PCM hash (E7).
 *
 * The driver (the Zyrinx player, Z80 RAM from Snd_1E6104; NOTES "Phase 6: music_voices"): six
 * channel records at $1480+$67n, channel n = FM n+1. A record's +$00..+$49 are the music voice, its
 * +$4A.. the effect voice. Effects come only through the window $17EA-$180D (slot n = record n, 6
 * bytes: the kept priority, the hold, the data pointer), which Snd_SendSfxBody writes once per full
 * V-int; the driver takes a slot at $17A0 (record +$00 = $FF, +$01 = the hold), starts it at $0B6F
 * (+$00 back to 0, the effect voice keyed with IY = record + $4A), and while the hold (+$01) counts
 * down plays the effect voice on the channel; the music's notes on it are not keyed (the sequencer
 * keeps going: its note flag +$3F is set, the key at $0BAF takes it with IY = the record, and a
 * second note set before that replaces the first). Every key-on is the one write at $0CD0: $28 =
 * channel | $F0, so IY tells whose note it is.
 *
 * Machine 2 is machine.cpp's Z80 side rule for rule (z80_frame, the bus-request slice, the reset
 * line, the YM2612 timers) on a state of its own; its synthesis and machine 1's are a ymfm chip each,
 * as the engine's ym2612_ymfm.cpp (gain 195/256, the cartridge's output low-pass) and its mixer.c
 * drain them (the address/data pair guard, the stable stamp sort, events past the frame carried). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <vector>

extern "C" {
#include "video/genesis_machine.h"
#include "audio/event_queue.h"
#include "audio/mixer.h"
}
#include "ymfm_opn.h"

#include "enhance.h"
#include "native_machine.h"
#include "options.h"
#include "sound2.h"

extern GenesisMachine g_machine;

#define MPL            3420u
#define LINES          262u
#define MASTER_PER_Z80 15u
#define FM_SAMPLE      1008u
#define FRAME_END      895780u   /* host_audio's drain end */

#define WIN_LO   0x17EAu         /* the effect window: 6 slots of 6 bytes */
#define WIN_N    36u
#define REC      0x1480u         /* the channel records */
#define REC_SZ   0x67u
#define NREC     6u
#define REC_END  (REC + NREC * REC_SZ)
#define FX_VOICE 0x4Au           /* a record's effect voice */
#define NOTE     0x3Fu           /* a record's note flag */
#define FADE     0x0E67u
#define PAUSE    0x180Eu
/* the Z80's PC at a RAM write: the instruction's end (superzazu fetches every operand first) */
#define PC_FX_START 0x0B7Eu      /* $0B7B LD (IY+0),A: an effect started */
#define PC_NOTE_KEY 0x0BB9u      /* $0BB6 LD (IY+$3F),A: a note keyed */

static inline int in_window(uint32_t off) { return off - WIN_LO < WIN_N; }

/* ======================================================================== */
/* The synthesis: one ymfm chip (the YM3438 object, whose ym2612 generate is the cartridge's ladder) */

struct Ev { uint32_t stamp; uint8_t port, v; };

namespace {
struct YmfmIface : public ymfm::ymfm_interface {};

inline int16_t clamp16(int32_t v) { return (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v); }

struct FmSynth {
    YmfmIface iface;
    ymfm::ym3438 *chip = nullptr;
    uint32_t accum = 0;
    int32_t prev[2] = {0, 0}, out[2] = {0, 0};
    std::vector<Ev> q;            /* chip writes not applied yet (carried ones first) */
    std::vector<int16_t> buf;     /* the frame's stereo samples */

    void reset()
    {
        if (!chip) chip = new ymfm::ym3438(iface);
        chip->reset();
        accum = 0;
        prev[0] = prev[1] = out[0] = out[1] = 0;
        q.clear();
        buf.clear();
    }

    int32_t lpf(int c, int32_t in)
    {
        int64_t o;
        if (!g_opt.fm_filter)   /* the cartridge's (ym2612_ymfm.cpp: 6.910 / 4.910) */
            o = ((int64_t)(in + prev[c]) * 9484 + (int64_t)out[c] * 46568) / 65536;
        else                    /* the cleaner one (fm_shadow.cpp's relaxed: 0.90 / 0.10) */
            o = ((int64_t)in * 58982 + (int64_t)out[c] * 6554) / 65536;
        prev[c] = in;
        out[c] = clamp16((int32_t)o);
        return out[c];
    }

    void one()
    {
        ymfm::ym2612::output_data o;
        if (g_opt.fm_ladder) chip->ym2612::generate(&o, 1);
        else chip->generate(&o, 1);
        buf.push_back((int16_t)lpf(0, o.data[0] * 195 / 256));
        buf.push_back((int16_t)lpf(1, o.data[1] * 195 / 256));
    }

    void advance(uint32_t c)
    {
        accum += c;
        while (accum >= FM_SAMPLE) { accum -= FM_SAMPLE; one(); }
    }

    /* mixer.c's FM drain, for this chip's events */
    void drain(uint32_t end)
    {
        struct S { Ev e; uint32_t i; };
        size_t n = q.size();
        std::vector<S> evs(n);
        for (size_t i = 0; i < n; i++) evs[i] = { q[i], (uint32_t)i };
        for (size_t i = 0; i < n; i++) {   /* the pair guard */
            uint8_t p = evs[i].e.port;
            if (p == 0 || p == 2)
                for (size_t j = i + 1; j < n; j++) {
                    uint8_t r = evs[j].e.port;
                    if (r == p + 1) { evs[i].e.stamp = evs[j].e.stamp; break; }
                    if (r == p) break;
                }
        }
        std::stable_sort(evs.begin(), evs.end(), [](const S &a, const S &b) { return a.e.stamp < b.e.stamp; });
        size_t k = n;
        while (k > 0 && evs[k - 1].e.stamp > end) k--;
        if (k > 0 && (evs[k - 1].e.port == 0 || evs[k - 1].e.port == 2)) k--;
        uint32_t prv = 0;
        for (size_t i = 0; i < k; i++) {
            Ev e = evs[i].e;
            if (e.stamp < prv) e.stamp = prv;
            if (e.stamp > prv) advance(e.stamp - prv);
            chip->write(e.port, e.v);
            prv = e.stamp;
        }
        if (end > prv) advance(end - prv);
        q.clear();
        for (size_t i = k; i < n; i++) {
            Ev e = evs[i].e;
            e.stamp = e.stamp > end ? e.stamp - end : 0;
            q.push_back(e);
        }
    }

    /* every register as the machine's chip has it now (a machine whose synthesis starts mid-run);
       a frequency's high byte before its low byte, which latches both */
    void load_regs(const uint8_t reg[2][256])
    {
        auto w = [&](int part, int r) {
            chip->write((uint32_t)(part * 2), (uint8_t)r);
            chip->write((uint32_t)(part * 2 + 1), reg[part][r]);
        };
        for (int part = 0; part < 2; part++) {
            for (int r = part ? 0x30 : 0x21; r < 0xB8; r++) {
                if (r == 0x28 || r == 0x2A || (r > 0x2B && r < 0x30) || (r >= 0xA0 && r < 0xB0)) continue;
                w(part, r);
            }
            for (int c = 0; c < 3; c++) { w(part, 0xA4 + c); w(part, 0xA0 + c); w(part, 0xAC + c); w(part, 0xA8 + c); }
        }
    }

    ~FmSynth() { delete chip; }
};
} // namespace

/* ======================================================================== */
/* State */

/* a machine's YM2612 as its Z80 wrote it: the address latches, the registers, whose note each
   channel sounds (1 = an effect's) */
struct YmView {
    uint8_t sel[2];
    uint8_t reg[2][256];
    uint8_t fx[6];
};

/* machine 2: machine.cpp's Z80 side (MState's Z80 and YM2612 timer fields, g_machine's core, RAM and
   bank register) */
struct M2 {
    z80      z;
    uint8_t  ram[0x2000];
    uint16_t bank;
    uint32_t zclk, znow, zstamp, stamp_base;
    int      zstate, zaccess, ei_shadow;
    uint32_t fm_count, fm_start;
    int      fm_ta, fm_tal, fm_tac, fm_tbl, fm_tbc;
    uint8_t  fm_mode, fm_status;
    uint16_t fm_addr;
};

/* machine 1's effects on their way (E6's effects in flight at a run's end): each slot's last send
   and the effect the driver last took from it, stamped with the frame (frames completed when it
   happened) and the V-int of its sync interval. A send is taken at the driver's next intake, about a
   frame later, and started at the record's next output a tick after that */
struct FxStamps {
    uint64_t frame;                     /* frames completed (snd_frame_end) */
    uint64_t sent[6], taken[6];         /* the frame of the slot's last send / of the taken effect's send */
    uint32_t sent_vint[6], taken_vint[6];
    uint8_t  taken_data[6][4];          /* the taken effect's data pointer (the slot's bytes +2..+5) */
};

struct State {          /* in a branch snapshot */
    int    m2_on;       /* machine 2 runs */
    int    own;         /* the sound is synthesized here */
    M2     m2;
    YmView v1, v2;
    FxStamps fx;
};
static State S;

struct MCount {
    uint64_t keyons[6], music[6], fx[6], other[6];
    uint64_t notes_set[6], notes_keyed[6], notes_lost[6];
};
struct Counters {       /* the run's (a branch restore keeps them: they sum every branch) */
    MCount m1, m2;
    uint64_t sfx_sent, sfx_overwritten, sfx_taken, sfx_replaced, sfx_started, sfx_cancelled, sfx_cancelled_68k;
    uint64_t sfx_overwritten_late_vint;
    uint64_t music_muted, m2_reads, win_dropped, m2_ram68_dropped, m2_psg, m2_bad_read, m2_sfx_started;
    uint64_t fadepause_mismatch, frames, m2_frames, pcm_frames, pcm_samples;
    uint32_t crc_mix, crc_music, crc_sfx;
};
static Counters C;

static FmSynth s_fm1, s_fm2;      /* machine 1's (effects; all of it without music_voices), machine 2's */
static int s_ready;
static int s_force_own, s_render, s_plant_at = -1, s_plant_n;
static int32_t s_dc[2] = {0, 0};  /* a silent chip's output (the ladder's offset): one in the mix */
static int s_dc_ladder = -1, s_dc_filter = -1;
int g_snd_nosfx;
int g_snd_deaf;
static int64_t s_deaf_at = -1;    /* BR_SND_PLANT_DEAF=N: machine 1's driver takes no effect from frame N on */

static FILE *s_wav[3];            /* mix, music, effects */
static uint32_t s_wav_bytes[3];
static void wav_close(void);
static std::vector<AudioEvent> s_held;   /* headless, the engine's mixer: its carried writes (snd_headless_frame) */

/* ======================================================================== */
/* CRC-32 (the PCM hash) */

static uint32_t s_crc_tab[256];
static void crc_init(void)
{
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int k = 0; k < 8; k++) c = (c & 1u) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
        s_crc_tab[i] = c;
    }
}
static uint32_t crc_pcm(uint32_t c, const int16_t *p, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        uint8_t b[2] = { (uint8_t)p[i], (uint8_t)((uint16_t)p[i] >> 8) };
        c = s_crc_tab[(c ^ b[0]) & 0xFFu] ^ (c >> 8);
        c = s_crc_tab[(c ^ b[1]) & 0xFFu] ^ (c >> 8);
    }
    return c;
}

/* ======================================================================== */
/* The YM2612 writes of either machine: the counters, machine 1's gate */

static int chan_of(uint8_t v) { int c = v & 7; return c == 3 || c == 7 ? -1 : c < 3 ? c : c - 1; }

/* whose key-on: 0 the music (IY = record n), 1 an effect (IY = record n + $4A), 2 neither */
static int key_class(uint16_t iy, int ch)
{
    if (iy < REC || iy >= REC_END) return 2;
    unsigned off = iy - REC, r = off / REC_SZ, f = off % REC_SZ;
    if ((int)r != ch) return 2;
    return f == 0 ? 0 : f == FX_VOICE ? 1 : 2;
}

/* a chip write into the machine's view; a key-on is counted: returns its channel (else -1) */
static int ym_view(YmView &V, MCount &M, unsigned port, uint8_t v, uint16_t iy, int *kc)
{
    unsigned part = (port >> 1) & 1;
    if (!(port & 1)) { V.sel[part] = v; return -1; }
    uint8_t r = V.sel[part];
    V.reg[part][r] = v;
    if (part || r != 0x28) return -1;
    int ch = chan_of(v);
    if (ch < 0 || !(v & 0xF0)) return -1;
    int k = key_class(iy, ch);
    M.keyons[ch]++;
    (k == 0 ? M.music : k == 1 ? M.fx : M.other)[ch]++;
    V.fx[ch] = (uint8_t)(k == 1);
    *kc = k;
    return ch;
}

extern "C" void snd_m1_ym(uint32_t stamp, unsigned port, uint8_t v, uint16_t iy)
{
    int k = -1, ch = ym_view(S.v1, C.m1, port, v, iy, &k);
    uint8_t out = v;
    if (ch >= 0) {
        /* the contract's machine-1 key-ons are the music's (an effect's key-on is not a note of the
           music): the suppressed reference's minus these is the notes the effects took */
        if (k != 1) g_enh.audio.keyons_by_channel[ch]++;
        if (S.m2_on && k != 1) { C.music_muted++; out = (uint8_t)(v & 0x0F); }   /* the music's: a key-off here */
        if (S.m2_on && k != 1 && (out & 0xF0)) g_enh.audio.audible_outside_effect++;   /* never: the gate's check */
    }
    if (S.own) s_fm1.q.push_back({ stamp, (uint8_t)port, out });
    else audio_event_push(stamp, (uint8_t)port, v);
}

/* ======================================================================== */
/* RAM writes by either machine's Z80 in the records and the window: notes and effects */

static uint64_t sum6(const uint64_t *a) { uint64_t s = 0; for (int i = 0; i < 6; i++) s += a[i]; return s; }

/* machine 1's lost notes and the effects sent, by the song playing (g_MusicCur, $FFF622) and
   g_SectionId ($FFF602) at the time: key song << 16 | section */
static struct { uint32_t section; uint64_t lost, sent; } s_lost_sec[64];
static int s_nlost_sec;
static void count_in_section(uint32_t sec, int sent)
{
    for (int i = 0; i < s_nlost_sec; i++)
        if (s_lost_sec[i].section == sec) { (sent ? s_lost_sec[i].sent : s_lost_sec[i].lost)++; return; }
    if (s_nlost_sec < 64) s_lost_sec[s_nlost_sec++] = { sec, (uint64_t)!sent, (uint64_t)sent };
}
static uint32_t section_now(void)
{
    return (uint32_t)nm_ram_read8(0xFFF622u) << 24 | (uint32_t)nm_ram_read8(0xFFF623u) << 16 |
           (uint32_t)nm_ram_read8(0xFFF602u) << 8 | nm_ram_read8(0xFFF603u);
}
static void lost_in_section(uint32_t sec) { count_in_section(sec, 0); }

static void note_write(MCount &M, uint16_t off, uint8_t old, uint8_t v, uint16_t pc)
{
    unsigned r = (off - REC) / REC_SZ;
    if (v) {
        M.notes_set[r]++;
        if (old) M.notes_lost[r]++;            /* a note not keyed, replaced */
    } else if (old) {
        if (pc == PC_NOTE_KEY) M.notes_keyed[r]++;
        else M.notes_lost[r]++;                /* a note not keyed, cleared */
    }
}

extern "C" void snd_m1_ram(uint16_t off, uint8_t old, uint8_t v, uint16_t pc)
{
    if (off >= REC && off < REC_END) {
        unsigned f = (off - REC) % REC_SZ;
        if (f == NOTE) {
            uint64_t lost = sum6(C.m1.notes_lost);
            note_write(C.m1, off, old, v, pc);
            if (sum6(C.m1.notes_lost) != lost) {
                g_enh.audio.notes_lost++;
                lost_in_section(section_now());
            }
        } else if (f == 0) {
            if (v == 0xFF) { if (old == 0xFF) C.sfx_replaced++; }          /* taken twice, never started */
            else if (!v && old) {
                if (pc == PC_FX_START) {
                    C.sfx_started++;
                    g_enh.audio.sfx_started++;
                    uint64_t lat = S.fx.frame - S.fx.taken[(off - REC) / REC_SZ];   /* frames since its send */
                    if (lat > g_enh.audio.sfx_start_latency_max) g_enh.audio.sfx_start_latency_max = lat;
                }
                else C.sfx_cancelled++;
            }
        }
    } else if (in_window(off) && (off - WIN_LO) % 6u == 0 && !v && old) {
        C.sfx_taken++;
        unsigned s = (off - WIN_LO) / 6u;
        S.fx.taken[s] = S.fx.sent[s];
        S.fx.taken_vint[s] = S.fx.sent_vint[s];
        memcpy(S.fx.taken_data[s], g_machine.bus.z80_ram + off + 2, 4);
    }
}

/* ======================================================================== */
/* Machine 2: machine.cpp's Z80 side on its own state */

static void m2_fm_update(uint32_t t)
{
    M2 &m = S.m2;
    if (t <= m.fm_count) return;
    uint32_t n = (t - m.fm_count + FM_SAMPLE - 1) / FM_SAMPLE;
    for (uint32_t i = 0; i < n; i++)
        if (m.fm_mode & 0x01) {
            if (--m.fm_tac <= 0) {
                if (m.fm_mode & 0x04) m.fm_status |= 0x01;
                m.fm_tac = m.fm_tal;
            }
        }
    if (m.fm_mode & 0x02) {
        m.fm_tbc -= (int)n;
        if (m.fm_tbc <= 0) {
            if (m.fm_mode & 0x08) m.fm_status |= 0x02;
            if (m.fm_tbl) m.fm_tbc += m.fm_tbl; else m.fm_tbc = m.fm_tbl;
        }
    }
    m.fm_count += n * FM_SAMPLE;
}

static void m2_fm_set_timers(uint8_t v)
{
    M2 &m = S.m2;
    if ((v & 1) && !(m.fm_mode & 1)) m.fm_tac = m.fm_tal;
    if ((v & 2) && !(m.fm_mode & 2)) m.fm_tbc = m.fm_tbl;
    m.fm_status &= (uint8_t)(~v >> 4);
    m.fm_mode = v;
}

static void m2_fm_reset_chip(uint32_t t)
{
    M2 &m = S.m2;
    m2_fm_update(t);
    m.fm_tac = m.fm_tbc = 0;
    m2_fm_set_timers(0x30);
    m.fm_tbl = 256 << 4;
    m.fm_ta = 0;
    m.fm_tal = 1024;
}

static void m2_ym_write(unsigned port, uint8_t v)
{
    M2 &m = S.m2;
    if (port & 1) m2_fm_update(m.znow);
    switch (port & 3) {
    case 0: m.fm_addr = v; break;
    case 2: m.fm_addr = (uint16_t)(v | 0x100); break;
    default:
        switch (m.fm_addr) {
        case 0x24: m.fm_ta = (m.fm_ta & 0x03) | (v << 2); m.fm_tal = 1024 - m.fm_ta; break;
        case 0x25: m.fm_ta = (m.fm_ta & 0x3FC) | (v & 3); m.fm_tal = 1024 - m.fm_ta; break;
        case 0x26: m.fm_tbl = (256 - v) << 4; break;
        case 0x27: m2_fm_set_timers(v); break;
        default: break;
        }
        break;
    }
    int k = -1, ch = ym_view(S.v2, C.m2, port, v, m.z.iy, &k);
    if (ch >= 0) g_enh.audio.music_keyons_by_channel[ch]++;
    s_fm2.q.push_back({ m.zstamp, (uint8_t)port, v });
}

static uint8_t m2_read(uint16_t a)
{
    M2 &m = S.m2;
    if (a < 0x4000u) return m.ram[a & 0x1FFFu];
    if (a < 0x6000u) { m2_fm_update(m.znow); return m.fm_status; }
    if (a < 0x8000u) return 0xFFu;
    uint32_t b = (((uint32_t)m.bank << 15) + (uint32_t)(a - 0x8000u)) & 0xFFFFFFu;
    if (b < 0x400000u) return g_rom[b];
    if (b >= 0xA00000u && b < 0xA02000u) return m.ram[b & 0x1FFFu];
    if (b >= 0xE00000u) return nm_ram_read8(0xFF0000u | (b & 0xFFFFu));   /* a read: nothing reaches the game */
    C.m2_bad_read++;
    return 0xFFu;
}

static void m2_write(uint16_t a, uint8_t v)
{
    M2 &m = S.m2;
    if (a < 0x4000u) {
        uint16_t off = a & 0x1FFFu;
        if (off >= REC && off < REC_END) {
            unsigned f = (off - REC) % REC_SZ;
            if (f == NOTE) note_write(C.m2, off, m.ram[off], v, m.z.pc);
            else if (f == 0 && !v && m.ram[off] && m.z.pc == PC_FX_START) C.m2_sfx_started++;
        }
        m.ram[off] = v;
        return;
    }
    if (a < 0x6000u) { m2_ym_write(a & 3u, v); return; }
    if (a < 0x6100u) { m.bank = (uint16_t)(((m.bank >> 1) | ((v & 1u) << 8)) & 0x1FFu); return; }
    if (a == 0x7F11u) { C.m2_psg++; return; }   /* the PSG: the driver writes none */
    if (a >= 0x8000u) {
        uint32_t b = (((uint32_t)m.bank << 15) + (uint32_t)(a - 0x8000u)) & 0xFFFFFFu;
        if (b >= 0xA00000u && b < 0xA02000u) { m.ram[b & 0x1FFFu] = v; return; }
        if (b >= 0x400000u) C.m2_ram68_dropped++;   /* never into the game's memory */
    }
}

static uint8_t z2_read(void *u, uint16_t a) { (void)u; return m2_read(a); }
static void z2_write(void *u, uint16_t a, uint8_t v) { (void)u; m2_write(a, v); }
static uint8_t z2_in(z80 *z, uint8_t p) { return m2_read((uint16_t)((z->b << 8) | p)); }
static void z2_out(z80 *z, uint8_t p, uint8_t v) { m2_write((uint16_t)((z->b << 8) | p), v); }

static void m2_wire(void)
{
    S.m2.z.read_byte = z2_read;
    S.m2.z.write_byte = z2_write;
    S.m2.z.port_in = z2_in;
    S.m2.z.port_out = z2_out;
    S.m2.z.userdata = nullptr;
}

static void m2_step(uint32_t target)
{
    M2 &m = S.m2;
    z80 *z = &m.z;
    while (m.zclk < target) {
        m.zstamp = m.stamp_base + m.zclk;
        if (!m.ei_shadow) {
            unsigned long c0 = z->cyc;
            if (z80_take_irq(z)) {
                m.zclk += (uint32_t)(z->cyc - c0) * MASTER_PER_Z80;
                if (m.zclk >= target) break;
            }
        }
        unsigned long c0 = z->cyc;
        m.znow = m.zclk + z80_base_cycles(z) * MASTER_PER_Z80;
        m.ei_shadow = z80_exec_one(z);
        m.zclk += (uint32_t)(z->cyc - c0) * MASTER_PER_Z80;
    }
}

static void m2_own_reset(void)
{
    z80 *z = &S.m2.z;
    z->pc = 0;
    z->iff1 = z->iff2 = 0;
    z->halted = 0;
    z->interrupt_mode = 0;
    z->iff_delay = 0;
    z->int_pending = z->nmi_pending = 0;
    z->int_data = 0xFF;
}

static void m2_exact_reset(void) { m2_own_reset(); S.m2.z.i = 0; S.m2.z.r = 0; }

/* machine 1 as it is now (power-on: as nm_power_on made it) */
static void m2_clone(void)
{
    SndM1 o;
    nm_sound_m1(&o);
    M2 &m = S.m2;
    memcpy(&m.z, o.z80, sizeof m.z);
    memcpy(m.ram, o.ram, sizeof m.ram);
    m.bank = o.bank;
    m.zclk = o.zclk; m.znow = m.zstamp = 0; m.stamp_base = o.stamp_base;
    m.zstate = o.zstate; m.zaccess = o.zaccess; m.ei_shadow = o.ei_shadow;
    m.fm_count = o.fm_count; m.fm_start = o.fm_start;
    m.fm_ta = o.fm_ta; m.fm_tal = o.fm_tal; m.fm_tac = o.fm_tac; m.fm_tbl = o.fm_tbl; m.fm_tbc = o.fm_tbc;
    m.fm_mode = o.fm_mode; m.fm_status = o.fm_status; m.fm_addr = o.fm_addr;
    m2_wire();
    S.v2 = S.v1;
    memset(S.v2.fx, 0, sizeof S.v2.fx);
}

/* the 68000's side, mirrored */
extern "C" void snd_68k_write(uint16_t off, uint8_t v)
{
    off &= 0x1FFFu;
    uint8_t old = g_machine.bus.z80_ram[off];
    if (in_window(off) && (off - WIN_LO) % 6u == 0) {   /* a slot's first byte (the kept priority) */
        if (v) {
            C.sfx_sent++;                                /* an effect sent (Snd_SendSfxBody) */
            g_enh.audio.sfx_sent++;
            count_in_section(section_now(), 1);
            SndM1 m1;
            nm_sound_m1(&m1);
            S.fx.sent[(off - WIN_LO) / 6u] = S.fx.frame;
            S.fx.sent_vint[(off - WIN_LO) / 6u] = m1.vints;
            if (old) {                                   /* the last one not taken yet */
                C.sfx_overwritten++;
                g_enh.audio.sfx_overwritten++;
                if (m1.vints > 1) C.sfx_overwritten_late_vint++;   /* a later V-int of the same interval */
            }
        } else if (old) C.sfx_cancelled_68k++;           /* a slot cleared before the driver took it */
    } else if (off >= REC && off < REC_END && (off - REC) % REC_SZ == 0 && old == 0xFF && v != 0xFF) {
        C.sfx_cancelled_68k++;                           /* a taken effect's record rewritten (a song start) */
    }
    if (!S.m2_on) return;
    if (in_window(off)) { C.win_dropped++; return; }
    if (S.m2.zaccess) S.m2.ram[off] = v;
}

extern "C" uint8_t snd_68k_read(uint16_t off, uint8_t v)
{
    /* the negative control (BR_SND_PLANT=N): the first read from the Nth on where machine 2's byte
       differs is machine 2's, once */
    if (s_plant_at >= 0 && S.m2_on && s_plant_n++ >= s_plant_at && S.m2.ram[off & 0x1FFFu] != v) {
        s_plant_at = -1;
        C.m2_reads++;
        g_enh.audio.machine2_reads++;
        return S.m2.ram[off & 0x1FFFu];
    }
    return v;
}

extern "C" void snd_busreq(int req)
{
    if (!S.m2_on) return;
    M2 &m = S.m2;
    uint32_t now = m.zclk;
    if (req) {
        if (m.zstate == 1) m.zaccess = 1;
        m.zstate |= 2;
    } else {
        if (m.zstate == 3) { m.zclk = now; m.zaccess = 0; }
        m.zstate &= 1;
        if (m.zstate == 1) m2_step(m.zclk + 8u * MPL);
    }
}

extern "C" void snd_reset(int release)
{
    if (!S.m2_on) return;
    M2 &m = S.m2;
    uint32_t now = m.zclk;
    if (release) {
        if (m.zstate == 0) { m.zclk = now; m2_exact_reset(); m2_fm_reset_chip(now); }
        else if (m.zstate == 2) { m.zaccess = 1; m2_exact_reset(); m2_fm_reset_chip(now); }
        m.zstate |= 1;
    } else {
        if (m.zstate == 3) m.zaccess = 0;
        m2_fm_reset_chip(now);
        m.zstate &= 2;
    }
}

static void m2_frame(void)
{
    M2 &m = S.m2;
    const uint32_t F = LINES * MPL;
    m.stamp_base = 0;
    if (m.zstate == 1) {
        m.z.int_pending = 1;
        m2_step(m.zclk + MPL);
        m.z.int_pending = 0;
    }
    if (m.zstate == 1 && m.zclk < F) m2_step(F);
    if (m.zclk < F) m.zclk = F;
    m2_fm_update(F);
    uint32_t span = F > m.fm_start ? F - m.fm_start : 0;
    uint32_t k = (span + FM_SAMPLE - 1) / FM_SAMPLE;
    if (k == 0) k = 1;
    m.fm_count = m.fm_start = m.fm_start + k * FM_SAMPLE - F;
    m.zclk -= F;
    m.stamp_base = F;
    C.m2_frames++;
}

/* ======================================================================== */
/* The options, power-on, the frame */

static void print_sound_line(void)
{
    static char b[16000];
    snd_counters_json(b, (int)sizeof b);
    fprintf(stderr, "[dkit] sound %s\n", b);
    fflush(stderr);
}

/* The runner's flags (enhance.h: --suppress-sfx, --audio-hash) and sound2.cpp's own diagnostics:
   BR_SND_PLANT=N (the negative control), BR_SND_PLANT_DEAF=N (the stuck-effect control: machine 1's
   Z80 reads the window as 0 from frame N on, a driver that takes no more effects), BR_SND_OWNPATH=1 (the synthesis here at the cartridge's
   settings: E7's proof against the engine's mixer), BR_WAV=PREFIX (the stems PREFIX.mix/music/sfx.wav,
   53,267 Hz stereo, the FM: the PSG is silent in this game; headless with --audio-hash, or played) */
static void read_env(void)
{
    const char *e;
    s_force_own = (e = getenv("BR_SND_OWNPATH")) && *e && *e != '0';
    g_snd_nosfx = g_enh_flags.suppress_sfx;
    s_plant_at = (e = getenv("BR_SND_PLANT")) && *e ? atoi(e) : -1;
    s_deaf_at = (e = getenv("BR_SND_PLANT_DEAF")) && *e ? atoll(e) : -1;
    const char *wav = getenv("BR_WAV");
    s_render = g_enh_flags.audio_hash || (wav && *wav);
    atexit(print_sound_line);
    if (wav && *wav) {
        static const char *stem[3] = { "mix", "music", "sfx" };
        for (int i = 0; i < 3; i++) {
            char path[1024];
            snprintf(path, sizeof path, "%s.%s.wav", wav, stem[i]);
            s_wav[i] = fopen(path, "wb");
            if (!s_wav[i]) { fprintf(stderr, "[sound] cannot write %s\n", path); exit(2); }
            uint8_t hdr[44] = {0};
            fwrite(hdr, 1, sizeof hdr, s_wav[i]);
        }
        atexit(wav_close);
    }
}

static int want_own(void) { return S.m2_on || !g_opt.fm_ladder || g_opt.fm_filter || s_force_own; }

static void synth_start(FmSynth &f, const YmView &v)
{
    f.reset();
    f.load_regs(v.reg);
}

static void audio_hook(uint64_t frame) { (void)frame; snd_headless_frame(); }

extern "C" void snd_power_on(void)
{
    if (!s_ready) { read_env(); crc_init(); s_ready = 1; }
    memset(&S, 0, sizeof S);
    memset(&C, 0, sizeof C);
    s_nlost_sec = 0;
    C.crc_mix = C.crc_music = C.crc_sfx = 0xFFFFFFFFu;
    S.m2_on = g_opt.music_voices != 0;
    g_enh.audio.active = 1;
    g_enh.audio.music_machine = S.m2_on;
    g_enh.audio.pcm_hashed = g_enh_flags.audio_hash;
    g_enh.audio.pcm_fnv1a64 = 0xCBF29CE484222325ULL;
    g_enh_audio_frame = audio_hook;
    S.own = want_own();
    if (S.m2_on) m2_clone();
    s_fm1.reset();
    s_fm2.reset();
}

/* options changed between frames (a hotkey): machine 2 starts as a copy of machine 1, the
   synthesis moves with every register as the chips have them */
extern "C" void snd_frame_begin(void)
{
    for (const AudioEvent &e : s_held) audio_event_push(e.cycle_stamp, e.port, e.value);
    s_held.clear();
    int m2 = g_opt.music_voices != 0;
    if (m2 != S.m2_on) {
        S.m2_on = m2;
        if (m2) { m2_clone(); synth_start(s_fm2, S.v2); }
        else s_fm2.reset();
        memset(S.v1.fx, 0, sizeof S.v1.fx);
    }
    int own = want_own();
    if (own != S.own) {
        S.own = own;
        if (own) synth_start(s_fm1, S.v1);
    }
}

extern "C" void snd_frame_end(void)
{
    C.frames++;
    S.fx.frame++;
    g_snd_deaf = s_deaf_at >= 0 && S.fx.frame >= (uint64_t)s_deaf_at;
    if (!S.m2_on) return;
    m2_frame();
    const uint8_t *r1 = g_machine.bus.z80_ram;
    if (r1[FADE] != S.m2.ram[FADE] || r1[PAUSE] != S.m2.ram[PAUSE]) { C.fadepause_mismatch++; g_enh.audio.fade_pause_mismatch++; }
}

/* ======================================================================== */
/* The host's side: the mix, the stems */

static void dc_update(void)
{
    if (s_dc_ladder == g_opt.fm_ladder && s_dc_filter == g_opt.fm_filter) return;
    s_dc_ladder = g_opt.fm_ladder;
    s_dc_filter = g_opt.fm_filter;
    FmSynth f;
    f.reset();
    for (int i = 0; i < 4000; i++) f.one();
    s_dc[0] = f.buf[f.buf.size() - 2];
    s_dc[1] = f.buf[f.buf.size() - 1];
}

/* the frame's samples of both chips: fm1 = machine 1's (the effects), fm2 = machine 2's (the music) */
static size_t render_own(void)
{
    s_fm1.drain(FRAME_END);
    if (S.m2_on) s_fm2.drain(FRAME_END);
    return s_fm1.buf.size() / 2;
}

static void stems_out(const int16_t *mix, const int16_t *music, const int16_t *sfx, size_t n)
{
    C.pcm_frames++;
    C.pcm_samples += n;
    C.crc_mix = crc_pcm(C.crc_mix, mix, 2 * n);
    uint64_t h = g_enh.audio.pcm_fnv1a64;   /* enhance.h's: FNV-1a 64 of the mix, 16-bit little-endian */
    for (size_t i = 0; i < 2 * n; i++) {
        h = (h ^ (uint8_t)mix[i]) * 0x100000001B3ULL;
        h = (h ^ (uint8_t)((uint16_t)mix[i] >> 8)) * 0x100000001B3ULL;
    }
    g_enh.audio.pcm_fnv1a64 = h;
    g_enh.audio.pcm_samples += n;
    if (music) C.crc_music = crc_pcm(C.crc_music, music, 2 * n);
    if (sfx) C.crc_sfx = crc_pcm(C.crc_sfx, sfx, 2 * n);
    const int16_t *st[3] = { mix, music, sfx };
    for (int i = 0; i < 3; i++)
        if (s_wav[i] && st[i]) { fwrite(st[i], sizeof(int16_t), 2 * n, s_wav[i]); s_wav_bytes[i] += (uint32_t)(4 * n); }
}

/* own path: fm = the mix (machine 1's effects + machine 2's music, one chip's ladder offset taken off) */
static size_t mix_own(int16_t *fm, size_t cap, int stems)
{
    size_t n = render_own();
    if (n > cap) n = cap;
    if (S.m2_on) {
        dc_update();
        size_t n2 = s_fm2.buf.size() / 2;
        for (size_t i = 0; i < n; i++)
            for (int c = 0; c < 2; c++) {
                int32_t a = s_fm1.buf[2 * i + c], b = i < n2 ? s_fm2.buf[2 * i + c] : s_dc[c];
                fm[2 * i + c] = clamp16(a + b - s_dc[c]);
            }
        if (stems) {
            if (n2 < n) s_fm2.buf.resize(2 * n, 0);
            stems_out(fm, s_fm2.buf.data(), s_fm1.buf.data(), n);
        }
    } else {
        memcpy(fm, s_fm1.buf.data(), 2 * n * sizeof(int16_t));
        if (stems) stems_out(fm, nullptr, nullptr, n);
    }
    s_fm1.buf.clear();
    s_fm2.buf.clear();
    return n;
}

extern "C" int snd_own_path(void) { return S.own; }

extern "C" size_t snd_mix(int16_t *fm, size_t cap) { return mix_own(fm, cap, s_wav[0] != nullptr); }

extern "C" void snd_headless_frame(void)
{
    static int16_t fm[8192 * 2], psg[16384];
    if (!s_render) {
        audio_event_queue_reset();
        s_fm1.q.clear();
        s_fm2.q.clear();
        return;
    }
    if (S.own) {
        audio_event_queue_reset();   /* the PSG: the 68000's four boot writes, silent */
        mix_own(fm, 8192, 1);
        return;
    }
    static int inited;
    if (!inited) { audio_mixer_init(); inited = 1; }
    size_t fm_n = 0, psg_n = 0;
    audio_mixer_drain(FRAME_END, fm, 8192, &fm_n, psg, sizeof psg / sizeof psg[0], &psg_n);
    stems_out(fm, nullptr, nullptr, fm_n);
    /* the writes the mixer carried past the frame (a long load's bus releases) are kept as a played
       run keeps them: the runner empties the queue after this hook; they go back at the next frame's
       start, before its first write */
    AudioEvent e;
    while (audio_event_pop(&e)) s_held.push_back(e);
}

static void wav_close(void)
{
    for (int i = 0; i < 3; i++) {
        if (!s_wav[i]) continue;
        uint8_t h[44];
        uint32_t rate = 53267u, data = s_wav_bytes[i];
        auto le32 = [&](int o, uint32_t v) { h[o] = (uint8_t)v; h[o + 1] = (uint8_t)(v >> 8); h[o + 2] = (uint8_t)(v >> 16); h[o + 3] = (uint8_t)(v >> 24); };
        auto le16 = [&](int o, uint16_t v) { h[o] = (uint8_t)v; h[o + 1] = (uint8_t)(v >> 8); };
        memcpy(h, "RIFF", 4); le32(4, 36 + data); memcpy(h + 8, "WAVEfmt ", 8);
        le32(16, 16); le16(20, 1); le16(22, 2); le32(24, rate); le32(28, rate * 4); le16(32, 4); le16(34, 16);
        memcpy(h + 36, "data", 4); le32(40, data);
        fseek(s_wav[i], 0, SEEK_SET);
        fwrite(h, 1, sizeof h, s_wav[i]);
        fclose(s_wav[i]);
        s_wav[i] = nullptr;
    }
}

/* ======================================================================== */
/* Branch snapshots: machine 2, the views (the synthesis restarts from the registers) */

extern "C" size_t snd_save(void *dst, size_t cap)
{
    if (!dst) return sizeof S;
    if (cap < sizeof S) return 0;
    State c = S;
    c.m2.z.read_byte = nullptr; c.m2.z.write_byte = nullptr; c.m2.z.port_in = nullptr; c.m2.z.port_out = nullptr;
    memcpy(dst, &c, sizeof c);
    return sizeof c;
}

extern "C" int snd_load(const void *src, size_t len)
{
    if (len != sizeof S) return 0;
    memcpy(&S, src, sizeof S);
    s_held.clear();
    m2_wire();
    synth_start(s_fm1, S.v1);
    synth_start(s_fm2, S.v2);
    return 1;
}

/* ======================================================================== */
/* The counters line's "audio" */

static std::string arr(const uint64_t *a)
{
    char b[200];
    snprintf(b, sizeof b, "[%llu, %llu, %llu, %llu, %llu, %llu]", (unsigned long long)a[0], (unsigned long long)a[1],
             (unsigned long long)a[2], (unsigned long long)a[3], (unsigned long long)a[4], (unsigned long long)a[5]);
    return b;
}

static uint64_t sum(const uint64_t *a) { return sum6(a); }

/* machine 1's effects in flight now (a run's or a branch's end): sent, not started yet. In the window
   (the driver has not taken it: its next intake would) or taken (record +$00 = $FF: its next output
   tick starts it). Each listed with its slot, where it is, its data pointer and the frame of its send;
   *n the count, *age the frames since the oldest one's send */
static std::string fx_in_flight(uint64_t *n, uint64_t *age)
{
    *n = *age = 0;
    std::string o = "[";
    const uint8_t *zr = g_machine.bus.z80_ram;
    for (unsigned s = 0; s < 6; s++) {
        int win = zr[WIN_LO + 6 * s] != 0, taken = zr[REC + s * REC_SZ] == 0xFF;
        for (int k = 0; k < win + taken; k++) {
            int t = k < taken;
            const uint8_t *w = t ? S.fx.taken_data[s] : zr + WIN_LO + 6 * s + 2;
            uint64_t sent = t ? S.fx.taken[s] : S.fx.sent[s];
            char b[200];
            snprintf(b, sizeof b, "%s{\"slot\": %u, \"where\": \"%s\", \"data\": \"%02X%02X%02X%02X\", \"sent_frame\": %llu, "
                     "\"sent_vint\": %u}", *n ? ", " : "", s, t ? "taken" : "window", w[0], w[1], w[2], w[3],
                     (unsigned long long)sent, t ? S.fx.taken_vint[s] : S.fx.sent_vint[s]);
            o += b;
            (*n)++;
            if (S.fx.frame - sent > *age) *age = S.fx.frame - sent;
        }
    }
    return o + "]";
}

extern "C" void snd_fx_in_flight(uint64_t *n, uint64_t *age) { fx_in_flight(n, age); }

static std::string lost_sections(void)
{
    std::string o = "{";
    for (int i = 0; i < s_nlost_sec; i++) {
        char b[80];
        snprintf(b, sizeof b, "%s\"%04X/%04X\": [%llu, %llu]", i ? ", " : "", (unsigned)(s_lost_sec[i].section >> 16),
                 (unsigned)(s_lost_sec[i].section & 0xFFFFu), (unsigned long long)s_lost_sec[i].lost,
                 (unsigned long long)s_lost_sec[i].sent);
        o += b;
    }
    return o + "}";
}

static std::string mjson(const MCount &M)
{
    return "{\"keyons\": " + arr(M.keyons) + ", \"music_keyons\": " + arr(M.music) + ", \"sfx_keyons\": " + arr(M.fx) +
           ", \"other_keyons\": " + arr(M.other) + ", \"notes_set\": " + arr(M.notes_set) + ", \"notes_keyed\": " +
           arr(M.notes_keyed) + ", \"notes_lost\": " + arr(M.notes_lost) + "}";
}

extern "C" int snd_counters_json(char *buf, int cap)
{
    /* the music as heard: machine 2's key-ons with music_voices, else machine 1's music key-ons */
    const uint64_t *music = S.m2_on ? C.m2.keyons : g_snd_nosfx ? C.m1.keyons : C.m1.music;
    uint64_t pending, age;
    std::string inflight = fx_in_flight(&pending, &age);
    char pcm[200] = "null";
    if (C.pcm_frames)
        snprintf(pcm, sizeof pcm, "{\"frames\": %llu, \"samples\": %llu, \"mix_crc32\": \"%08x\", \"music_crc32\": \"%08x\", "
                 "\"sfx_crc32\": \"%08x\"}", (unsigned long long)C.pcm_frames, (unsigned long long)C.pcm_samples,
                 C.crc_mix ^ 0xFFFFFFFFu, C.crc_music ^ 0xFFFFFFFFu, C.crc_sfx ^ 0xFFFFFFFFu);
    std::string j = "{\"music_voices\": " + std::to_string(S.m2_on) + ", \"fm_ladder\": " + std::to_string(g_opt.fm_ladder) +
        ", \"fm_filter\": " + std::to_string(g_opt.fm_filter) + ", \"own_synthesis\": " + std::to_string(S.own) +
        ", \"reference\": \"" + (g_snd_nosfx ? "nosfx" : "none") + "\"" +
        ", \"music_keyons_by_channel\": " + arr(music) +
        ", \"notes_lost\": " + std::to_string(sum(C.m1.notes_lost)) + ", \"notes_lost_by_channel\": " + arr(C.m1.notes_lost) +
        ", \"by_song_section\": " + lost_sections() +   /* "song/section": [notes lost, effects sent] */
        ", \"sfx_sent\": " + std::to_string(C.sfx_sent) + ", \"sfx_started\": " + std::to_string(C.sfx_started) +
        ", \"sfx_overwritten\": " + std::to_string(C.sfx_overwritten) +
        ", \"sfx_overwritten_late_vint\": " + std::to_string(C.sfx_overwritten_late_vint) + ", \"sfx_taken\": " + std::to_string(C.sfx_taken) +
        ", \"sfx_replaced\": " + std::to_string(C.sfx_replaced) + ", \"sfx_cancelled\": " + std::to_string(C.sfx_cancelled) +
        ", \"sfx_cancelled_68k\": " + std::to_string(C.sfx_cancelled_68k) +
        ", \"sfx_pending\": " + std::to_string(pending) + ", \"sfx_in_flight\": " + inflight +
        ", \"sfx_in_flight_age\": " + std::to_string(age) +
        ", \"machine2_reads\": " + std::to_string(C.m2_reads) +
        ", \"music_keyons_muted\": " + std::to_string(C.music_muted) +
        ", \"window_writes_dropped\": " + std::to_string(C.win_dropped) +
        ", \"machine2_sfx_started\": " + std::to_string(C.m2_sfx_started) +
        ", \"machine2_game_writes\": " + std::to_string(C.m2_ram68_dropped) +
        ", \"machine2_psg_writes\": " + std::to_string(C.m2_psg) + ", \"machine2_bad_reads\": " + std::to_string(C.m2_bad_read) +
        ", \"fade_pause_mismatch\": " + std::to_string(C.fadepause_mismatch) +
        ", \"frames\": " + std::to_string(C.frames) + ", \"machine2_frames\": " + std::to_string(C.m2_frames) +
        ", \"m1\": " + mjson(C.m1) + ", \"m2\": " + mjson(C.m2) + ", \"pcm\": " + pcm + "}";
    return snprintf(buf, (size_t)cap, "%s", j.c_str());
}
