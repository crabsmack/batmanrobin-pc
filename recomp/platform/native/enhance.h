/* enhance.h - phase 6's counters and diagnostics (docs/phases/P6-enhance.md, "The contract" in
 * docs/NOTES.md "Phase 6: the contract"): what every native trace reports for the enhancement
 * checks (the kit's dkit.phases.p6_enhance reads them), as plain counters the option code fills.
 *
 * Each group has `active`: the piece that fills it sets it to 1 (each run, before its first frame
 * or when it first counts); a group left at 0 is reported as null and the criterion that reads it
 * fails naming the missing piece. Nothing here is game state: the counters are not in any sync
 * row, not in a snapshot, and are carried around a branch plan's branches by the runner (each
 * branch reports its own).
 *
 *   present  the present clock (enhance.cpp: --present-log, high_fps) and the interpolator
 *            (high_fps: sprites on gameplay frames outside the HUD, matched to their object and
 *            interpolated; per-line scroll by section), the render time per displayed frame
 *   ws       widescreen (R4, R5, R11, R12, R16): the widescreen code fills it
 *   sprites  the per-line limits and the display list (R10): the engine's VDP model and the game's
 *            native branches fill it, with the option on or off (the reference's drops)
 *   audio    the sound machines (R13-R15): the sound side fills it (machine 1 = the cartridge's,
 *            machine 2 = the music's own with music_voices)
 *
 * The runner prints them at exit as one line, `[dkit] enhance {...}` (and each branch's in its
 * GENESISRECOMP_BRANCH line), beside `[dkit] options {...}` (the effective option values).
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENH_SECTIONS 96       /* distinct g_SectionId values a run's counters keep, by first seen */
#define ENH_SCREENS  64       /* distinct screen ids (ws.screens) */
#define ENH_FM 6              /* FM1-FM6 */

typedef struct EnhSectionCount {
    uint16_t section;         /* g_SectionId */
    uint64_t frames;          /* interpolated present frames of the section using per-line scroll */
    uint64_t interpolated;    /* of them, frames whose per-line scroll the interpolator moved */
    uint64_t static_frames;   /* interpolated present frames with per-line scroll that does not move from
                                 R_(k-1) (not in `frames`, R19 amended: reported, E4 judges `frames`) */
} EnhSectionCount;

typedef struct EnhPresent {
    int      active;          /* the present clock ran (high_fps on, or --present-log) */
    int      hz;              /* high_fps's rate, 0 = off (the 59.92 Hz clock) */
    uint64_t frames;          /* displayed frames */
    uint64_t whole;           /* of them, whole logic frames */
    int      interpolator;    /* 1 once an interpolator is registered (enh_set_interpolator) */
    uint64_t sprites;         /* interpolated gameplay frames: sprites outside the HUD (the interpolator) */
    uint64_t sprites_matched; /* of them, drawn interpolated from their object's previous position */
    EnhSectionCount scroll[ENH_SECTIONS];   /* per-line scroll by section (enh_scroll_frame) */
    int      nscroll;
} EnhPresent;

typedef struct EnhScreen { char id[24]; char mode[8]; uint64_t frames; } EnhScreen;

/* a pinned HUD item inside a bar, on one screen: frames, opaque pixels in the bar, of them painted
   over, the widest run of bar columns its pixels reach (px from the bar's inner edge outward) */
#define ENH_HUD_BARS 32
typedef struct EnhHudBar { char id[24]; int item; uint64_t frames, pixels, hidden; int cols_max; } EnhHudBar;

typedef struct EnhWs {
    int      active;          /* the widescreen code ran this run (widescreen on) */
    uint64_t gameplay_frames;
    uint64_t terrain_checks;  /* plane cells checked against the block map / what the whole-drawn plane drew */
    uint64_t terrain_errors;
    uint64_t streamed_frames; /* plane checks of the streamed layer that saw a line showing it (E3's coverage) */
    uint64_t streamed_lines;          /* lines showing the layer at its place: every margin cell checked */
    uint64_t streamed_lines_apart;    /* lines scrolled apart: the margin cells in the streamed window */
    uint64_t streamed_cells_left, streamed_cells_right;   /* margin cells checked against the block map */
    uint64_t sprites_lost;   /* sprite entries in the wide view outside the VDP's 9-bit x */
    uint64_t hud_misplaced;   /* gameplay frames with a HUD sprite not pinned to its screen edge */
    uint64_t bars_unneeded;   /* gameplay frames with a bar where the map shows 400 px or more (R16) */
    uint64_t edge_clamped_columns;   /* margin V-scroll columns left at the edge column's value (R12) */
    uint64_t centre_mismatch; /* diagnostics of the widescreen code's own (the kit compares fb_centre) */
    uint64_t hud_frames[6];   /* wide gameplay frames each HUD item was checked on (widescreen.cpp hud_side:
                                 1/2 the panels, 4 the boss meter, 5 the GO! sign; ws JSON hud_frames) */
    uint64_t centre_rects_full;   /* HUD pieces past ENH_CENTRE_RECTS (not exempted from the centre check) */
    /* unlimited_sprites' host table (the sprites past the 80-entry table, gvdp_set_host_sat) on every
       frame drawn with widescreen on (widescreen.cpp check_host, ws-host 2026-10-30): */
    uint64_t host_sprites_checked;    /* host-table sprites checked */
    uint64_t host_sprites_misplaced;  /* of them, drawn at an x other than a table sprite's move (the
                                         render camera's -shift, the HUD's pin, x 0 kept) from the game's */
    uint64_t host_shift_frames;       /* frames checked with a host table and a nonzero render camera shift */
    /* the stage 1 street's margins against the facade model (widescreen.cpp check_street, ws-street
       2026-10-30; ws JSON `street`; its errors are terrain errors too): */
    uint64_t street_frames;           /* wide gameplay frames of section $1100 checked */
    uint64_t street_pixels;           /* margin pixels compared */
    uint64_t street_folded;           /* of them, where the model shows the facade one segment on or back */
    uint64_t street_sprite_pixels;    /* margin pixels left out: a sprite drew there (or an operator's box) */
    uint64_t street_errors;
    /* the street's building fire on plane A (widescreen.cpp fire_now / check_fire, ws-balloon
       2026-10-31; ws JSON `street.fire`; its errors are terrain errors too): */
    uint64_t fire_frames;             /* wide gameplay frames of $1100 with the fire (state 1 or $FF) */
    uint64_t fire_continued;          /* of them, past the game's cut: the view drew the sway on */
    uint64_t fire_lines;              /* fire lines compared with the model (the game's words, or the view's) */
    uint64_t fire_errors;             /* fire lines whose plane A word is not the model's */
    /* R20, the gameplay mode's wide view for the game's own decisions (widescreen.cpp view_extent;
       ws JSON "play"): the screen-placed spawns, the players against the view, the placed objects
       followed (drawn gameplay frames) */
    uint64_t placed_moved;            /* placed outside the 4:3 view, moved out for the wide view's edge */
    uint64_t placed_kept;             /* placed inside the 4:3 view: unchanged */
    uint64_t placed_unmoved;          /* placed outside the 4:3 view, not moved (BR_WS_PLAY_OFF, a 0 extent) */
    uint64_t player_margin_frames;    /* player-frames with the player's x in a margin of the wide view */
    uint64_t player_margin_locked;    /* of them, with the camera locked (an arena: past the lock's 4:3 edge) */
    uint64_t player_offview_frames;   /* player-frames with the player's x outside the wide view */
    uint64_t play_ended, play_never_in, play_offview_long, play_untracked;   /* the followed objects: gone,
                                         gone without entering the view, 10 s alive outside it, not followed */
    uint64_t locked_frames, lock_long;   /* gameplay frames with the camera locked; locks held a minute */
    /* the pinned HUD over the bars (widescreen.cpp "the HUD over the bars", ws-hudbars 2026-10-30;
       ws JSON `hud_bars`): on wide gameplay frames with a bar, every opaque pixel of a pinned piece
       (the panels, the boss meter, the GO! sign) inside a bar, before and after the bars are painted */
    uint64_t hud_bar_frames;          /* frames with a pinned piece's pixel inside a bar */
    uint64_t hud_bar_pixels;          /* those pixels (kept: the HUD drawn over the bar) */
    uint64_t hud_hidden_frames;       /* frames where a bar painted over one (each also hud_misplaced) */
    uint64_t hud_hidden_pixels;
    uint64_t hud_bar_unmatched;       /* of hud_bar_pixels, the frame showing another colour than the
                                         piece's own there before the bars (a diagnostic: a sprite in
                                         front, the per-line limit) */
    EnhHudBar hud_bars[ENH_HUD_BARS]; /* by screen and item */
    int      nhud_bars;
    EnhScreen screens[ENH_SCREENS];  /* enh_ws_screen */
    int      nscreens;
} EnhWs;

typedef struct EnhSprites {
    int      active;          /* the counters are filled (the VDP model's and the list's) */
    uint64_t line_drops;      /* sprite pieces dropped by the 20-a-line / 320-px-a-line limits */
    uint64_t line_drop_frames;
    uint64_t list_drops;      /* display-list nodes dropped by the game's 80-entry cap */
    uint64_t list_drop_frames;
    uint64_t x0_masked_lines; /* lines an x=0 sprite masked (the hardware rule, kept) */
    uint64_t cover_frames;    /* frames with the x 0-15 cover drawn (Spr_MaskLeftColumn, $F808) */
} EnhSprites;

typedef struct EnhAudio {
    int      active;          /* the sound side's counters are filled */
    uint64_t keyons_by_channel[ENH_FM];        /* machine 1 (the cartridge's driver), FM1-FM6 */
    int      music_machine;   /* 1 = machine 2 (the music's own) ran this run */
    uint64_t music_keyons_by_channel[ENH_FM];  /* machine 2 */
    uint64_t notes_lost;      /* music notes an effect took (the sound side's own count, if it has one) */
    uint64_t sfx_sent;        /* effects the 68000 sent (the window $17EA-$180D: six 6-byte slots) */
    uint64_t sfx_started;     /* effects the effects machine started */
    uint64_t sfx_overwritten; /* of the sent, effects the game wrote over in their slot before the driver
                                 read it (the cartridge's driver drops them too: it reads the slots
                                 every other tick, about once a frame) */
    uint64_t sfx_start_latency_max; /* the most frames any started effect took from its send to its start */
    /* (the report adds sfx_in_flight / sfx_in_flight_age: the effects sent and not started yet at
       the run's or the branch's end, the driver's intake and start still ahead, and the frames since
       the oldest one's send: sound2.cpp snd_fx_in_flight) */
    uint64_t machine2_reads;  /* 68000 reads of machine 2 (must stay 0) */
    uint64_t fade_pause_mismatch;   /* frames where the fade or pause bytes differ between the machines */
    uint64_t audible_outside_effect;/* effects-machine output on a channel not in effect mode */
    int      pcm_hashed;      /* --audio-hash: the PCM output was made and hashed */
    uint64_t pcm_fnv1a64;
    uint64_t pcm_samples;
} EnhAudio;

typedef struct EnhCounters {
    uint64_t   gameplay_frames;   /* sync points on gameplay frames (the runner: enh_gameplay) */
    EnhPresent present;
    EnhWs      ws;
    EnhSprites sprites;
    EnhAudio   audio;
} EnhCounters;

extern EnhCounters g_enh;

/* Diagnostics the runner was asked for (the command line), read by the pieces that act on them. */
typedef struct EnhFlags {
    int present_log;          /* --present-log */
    int game_rows;            /* --game-rows */
    int hash_centre;          /* --hash-centre: fb_centre beside each sync row */
    int audio_hash;           /* --audio-hash: make the PCM output headless and hash it */
    int suppress_sfx;         /* --suppress-sfx: the effect window's writes dropped (E6's reference) */
    long flip;                /* --flip N: the C button inverted in runner frames N..N+7 (-1: none) */
    long render_from;         /* --render-from F: no picture before runner frame F (0: always) */
} EnhFlags;

extern EnhFlags g_enh_flags;

/* A gameplay frame (this game's definition, NOTES "Phase 6: the contract"): in a stage section
   (g_SectionId $1100-$4FFF) with no fade running (g_FadeLevel and g_FadeTarget 0). From work RAM
   in the 68000's byte order (the exporter's image). */
int enh_gameplay(const uint8_t *ram);
uint16_t enh_section(const uint8_t *ram);

/* Helpers for the pieces that fill the groups */
void enh_ws_screen(const char *id, const char *mode);              /* one frame on screen `id` ("wide"/"4:3") */
void enh_scroll_frame(uint16_t section, int interpolated);        /* an interpolated frame with per-line scroll */
void enh_scroll_static(uint16_t section);                         /* one whose per-line scroll does not move */

/* The interpolator (high_fps): renders runner frame k's picture moved back toward k-1 by `alpha`
   (0 < alpha < 1) into fb (the machine's framebuffer layout, stride 512) and returns 1, or 0 to show
   frame k-1 again. Set once, before the first frame. */
typedef int (*EnhInterpolator)(uint64_t k, double alpha, uint32_t *fb, int stride, int w, int h);
void enh_set_interpolator(EnhInterpolator f);

/* The centre of a wide frame (E3's fb_centre: compared with the 4:3 run's frame): its columns
   [x0 + skip, x0 + 320). Default x0 = (width - the VDP's screen width) / 2, skip 0; the widescreen
   code sets them where they differ (the x 0-15 cover: skip 16, listed in NOTES). */
extern int g_enh_centre_x0;   /* -1: the default */
extern int g_enh_centre_skip;
/* Rectangles of the centre (its coordinates: 0 = the 4:3 view's x 0) hashed as zero pixels in every run
   alike, for what the wide view moves on purpose (the HUD pinned to the screen edges: the widescreen
   code lists where it is in either view, in the 4:3 run too). Set per drawn frame. */
typedef struct EnhRect { int x0, y0, x1, y1; } EnhRect;
#define ENH_CENTRE_RECTS 48   /* the panels' 14 pieces x 2 views, the boss meter's, the GO! sign's */
extern EnhRect g_enh_centre_rects[ENH_CENTRE_RECTS];
extern int g_enh_centre_nrects;

/* The render timer: one sample per displayed frame (microseconds); p99 in the present counters */
void enh_render_sample(double us);

/* The sound side's hook in headless runs (--benchmark): called after each runner frame, before the
   runner empties the audio event queue (nothing is heard headless), with the runner frame's number.
   Null: none. The sound side sets it at start (the --audio-hash PCM, its counters). */
extern void (*g_enh_audio_frame)(uint64_t frame);

#ifdef __cplusplus
}

/* The runner's side (runner.cpp) */
#include <string>
struct DkImage;
std::string enh_json(int with_render);                  /* the `[dkit] enhance` object */
void enh_present_init(const char *present_csv);         /* null: no log (the clock runs with high_fps) */
void enh_present_frame(uint64_t k, int gameplay, int rendered, double frame_us);
void enh_present_close(void);
uint32_t enh_game_crc(const DkImage *im, const uint8_t *ram_mask);   /* game.csv's game_crc32 */
uint64_t enh_centre_hash(void);                         /* fb_centre of the frame just drawn */
void enh_centre_shot(uint64_t sync);   /* BR_CENTRE_SHOT: that sync row's frame as a PPM (diagnosing) */
void enh_branch_begin(void);                            /* a branch counts from zero */
std::string enh_branch_end(void);                       /* its counts; the carrier's put back */
#endif
