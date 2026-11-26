/* state.h - the native runner's savestates as named sections (state.cpp), for quick saves and rewind
 * (rewind.cpp; docs/phases/P6-enhance.md R33). The branch plans keep their own one-at-a-time copy
 * (machine.cpp nm_snapshot_save): these are the same machine into caller-owned Snapshots, many held
 * at once. Adapted from the X-Men 2 port's platform/native/state.h (the same kit).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#include <string>
#include <utility>
#include <vector>

struct Snapshot {
    std::vector<uint8_t> bytes;                       /* the sections, in table order */
    std::vector<std::pair<size_t, size_t>> spans;     /* each section's offset and length in bytes */
};

void        state_save(Snapshot &snap);
/* skip: a section left out of the load (the negative control, BR_REWIND_SKIP) */
void        state_load(const Snapshot &snap, const char *skip = nullptr);
int         state_has_section(const char *name);
std::vector<std::string> state_section_names(void);   /* in table order (Snapshot::spans') */
std::string state_sections_json(void);   /* {"name": bytes, ...} of the last save */
/* the canonical digest of a snapshot: FNV-1a 64 of every section but the game fiber's stack (host
   addresses and the dead bytes below its frames) */
uint64_t    state_digest(const Snapshot &snap);

extern "C" {
#endif

/* custom sections: save(nullptr, 0) returns the bytes needed; save(dst, cap) writes them and returns
   the count (0 on failure); load(src, len) returns 1 on success */
typedef size_t (*StateSaveFn)(uint8_t *dst, size_t cap);
typedef int (*StateLoadFn)(const uint8_t *src, size_t len);

void state_section_range(const char *name, void *p, size_t n);    /* a range of memory of a section */
void state_section_custom(const char *name, StateSaveFn save, StateLoadFn load);
void state_after_load(void (*fn)(void));         /* rebuild derived state after every load */

#define STATE_VAR(section, v) state_section_range((section), (void *)&(v), sizeof(v))

#ifdef __cplusplus
}
#endif
