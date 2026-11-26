/* sound2.h - the native machine's sound for phase 6 (docs/phases/P6-enhance.md E6/E7, rulings R13-R15):
 * music_voices (a second sound machine that plays the music with no effect ever taking a channel),
 * fm_ladder and fm_filter (the synthesis), the audio counters, the stems. sound2.cpp.
 *
 * Machine 1 is machine.cpp's Z80 + YM2612 timers, exactly as the port has them: the 68000 reads only
 * it, and it alone is in the sync rows' z80_crc32. Machine 2 (music_voices) is a Z80 + YM2612 of its
 * own (its RAM, bank register, timers) fed every 68000 write to Z80 RAM and every bus-request and
 * reset line change of machine 1 except the effect window (Z80 $17EA-$180D: Snd_SendSfxBody's six
 * 6-byte slots), so its driver never starts an effect. Nothing of machine 2 reaches machine 1, work
 * RAM or the 68000. At synthesis machine 1 sounds only its effects: a music key-on (the driver's
 * key-on with IY at a channel record, not at its effect voice +$4A) is sent to its chip as a key-off,
 * so a channel stays audible from an effect's key-on until the music's next key-on or until its
 * envelope is silent (R14); machine 2 sounds the music.
 *
 * With music_voices=0, fm_ladder=1 and fm_filter=0 the sound is the port's: machine 1's chip writes
 * go to the engine's event queue and mixer as before. Any other setting synthesizes here (the
 * engine's ymfm as its own ym2612_ymfm.cpp does it, one chip per machine). */
#ifndef NATIVE_SOUND2_H
#define NATIVE_SOUND2_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Machine 1's Z80 side as machine.cpp keeps it (a copy: machine 2 started mid-run from it) */
typedef struct SndM1 {
    const void    *z80;          /* the core's struct z80 */
    const uint8_t *ram;          /* 8 KB */
    uint16_t bank;
    uint32_t zclk, stamp_base;
    int      zstate, zaccess, ei_shadow;
    uint32_t fm_count, fm_start;
    int      fm_ta, fm_tal, fm_tac, fm_tbl, fm_tbc;
    uint8_t  fm_mode, fm_status;
    uint16_t fm_addr;
    uint32_t vints;              /* V-ints taken at waits in this sync interval so far */
} SndM1;
void nm_sound_m1(SndM1 *o);      /* machine.cpp */

/* ---- called by machine.cpp ---- */
void    snd_power_on(void);
void    snd_frame_begin(void);                                   /* z80_frame's start: option changes */
void    snd_frame_end(void);                                     /* z80_frame's end: machine 2's frame */
void    snd_m1_ym(uint32_t stamp, unsigned port, uint8_t v, uint16_t iy);   /* machine 1's Z80 wrote the YM2612 */
void    snd_m1_ram(uint16_t off, uint8_t old, uint8_t v, uint16_t pc);      /* ... its RAM, off in [$1480,$180E) */
extern int g_snd_nosfx;                                          /* reference: machine 1's Z80 reads the window as 0 */
extern int g_snd_deaf;                                           /* the stuck-effect control (BR_SND_PLANT_DEAF): the same, from a frame on */
void    snd_68k_write(uint16_t off, uint8_t v);                  /* the 68000 wrote Z80 RAM (it had the bus) */
uint8_t snd_68k_read(uint16_t off, uint8_t v);                   /* ... read it: machine 1's byte v */
void    snd_busreq(int req);                                     /* after machine 1's line change */
void    snd_reset(int release);
size_t  snd_save(void *dst, size_t cap);                         /* branch snapshots (NULL: the size) */
int     snd_load(const void *src, size_t len);
/* quick saves and rewind (state.cpp's "sound"): machine 2, the views and the synthesis exactly (the
   ymfm chips' own state, the writes they carry), the engine's chips and their event queue (NULL: the size) */
size_t  snd_state_save(uint8_t *dst, size_t cap);
int     snd_state_load(const uint8_t *src, size_t len);

/* ---- called by the host (host_sdl.cpp) and the runner ---- */
int     snd_own_path(void);                                      /* the sound is synthesized here */
size_t  snd_mix(int16_t *fm, size_t cap);                        /* own path: the frame's FM (stereo) into fm */
void    snd_headless_frame(void);                                /* headless: rendered when asked, else dropped */
int     snd_counters_json(char *buf, int cap);                   /* the counters line's "audio" object */
void    snd_fx_in_flight(uint64_t *n, uint64_t *age);            /* effects sent, not started yet; the oldest's age (frames) */

#ifdef __cplusplus
}
#endif

#endif /* NATIVE_SOUND2_H */
