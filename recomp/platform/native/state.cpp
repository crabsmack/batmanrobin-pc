/* state.cpp - the native runner's savestates as named sections (state.h): quick saves and rewind
 * (rewind.cpp, docs/phases/P6-enhance.md R33).
 *
 * A snapshot is the game's whole state while its fiber is parked (at a sync point, or at a played
 * run's extra V-int): named sections, each a list of memory ranges or a custom save/load (the fiber's
 * stack and context, the machine with its host pointers scrubbed, the sound machines and chips). The
 * fiber's stack holds host addresses (frame pointers, pointers into the arena and the ROM image), so
 * a snapshot is good in this process only: nothing here is written to disk.
 *
 * machine.cpp registers the sections after power-on (nm_state_init); after a load the after-load
 * hooks rebuild what is derived (the palette cache, the high_fps interpolator's previous frame).
 * Adapted from the X-Men 2 port's platform/native/state.cpp (the same kit).
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "state.h"

[[noreturn]] static void state_fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[native] state: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    fflush(stderr);
    exit(5);
}

namespace {

struct Range { void *p; size_t n; };
struct Section {
    std::string name;
    std::vector<Range> ranges;
    StateSaveFn save = nullptr;   /* custom (instead of the ranges) */
    StateLoadFn load = nullptr;
};

/* never destroyed: the counters' report runs at exit (atexit) */
std::vector<Section> &sections()
{
    static auto *s = new std::vector<Section>;
    return *s;
}

std::vector<void (*)(void)> &after_load()
{
    static auto *a = new std::vector<void (*)(void)>;
    return *a;
}

Section &section(const char *name)
{
    for (Section &s : sections())
        if (s.name == name) return s;
    sections().push_back(Section{ name, {}, nullptr, nullptr });
    return sections().back();
}

std::vector<size_t> s_last_spans;   /* each section's size in the last snapshot saved (reported) */

size_t section_size(const Section &s)
{
    if (s.save) return s.save(nullptr, 0);
    size_t n = 0;
    for (const Range &r : s.ranges) n += r.n;
    return n;
}

}  // namespace

extern "C" void state_section_range(const char *name, void *p, size_t n) { section(name).ranges.push_back(Range{ p, n }); }

extern "C" void state_section_custom(const char *name, StateSaveFn save, StateLoadFn load)
{
    Section &s = section(name);
    s.save = save;
    s.load = load;
}

extern "C" void state_after_load(void (*fn)(void)) { after_load().push_back(fn); }

void state_save(Snapshot &snap)
{
    snap.spans.clear();
    s_last_spans.clear();
    size_t total = 0;
    for (const Section &s : sections()) total += section_size(s);
    snap.bytes.resize(total);
    size_t at = 0;
    for (const Section &s : sections()) {
        size_t n;
        if (s.save) {
            n = s.save(snap.bytes.data() + at, snap.bytes.size() - at);
            if (!n && section_size(s)) state_fatal("section %s: its save failed", s.name.c_str());
        } else {
            n = 0;
            for (const Range &r : s.ranges) { memcpy(snap.bytes.data() + at + n, r.p, r.n); n += r.n; }
        }
        snap.spans.push_back({ at, n });
        s_last_spans.push_back(n);
        at += n;
    }
    snap.bytes.resize(at);
}

void state_load(const Snapshot &snap, const char *skip)
{
    std::vector<Section> &all = sections();
    if (snap.spans.size() != all.size()) state_fatal("a snapshot of %zu sections for %zu", snap.spans.size(), all.size());
    for (size_t i = 0; i < all.size(); i++) {
        const Section &s = all[i];
        if (skip && s.name == skip) continue;
        const uint8_t *src = snap.bytes.data() + snap.spans[i].first;
        size_t n = snap.spans[i].second;
        if (s.load) {
            if (!s.load(src, n)) state_fatal("section %s: its load failed", s.name.c_str());
            continue;
        }
        size_t k = 0;
        for (const Range &r : s.ranges) { memcpy(r.p, src + k, r.n); k += r.n; }
    }
    for (auto fn : after_load()) fn();
}

int state_has_section(const char *name)
{
    for (const Section &s : sections())
        if (s.name == name) return 1;
    return 0;
}

std::vector<std::string> state_section_names(void)
{
    std::vector<std::string> n;
    for (const Section &s : sections()) n.push_back(s.name);
    return n;
}

std::string state_sections_json(void)
{
    std::string o = "{";
    for (size_t i = 0; i < sections().size(); i++)
        o += (i ? ", \"" : "\"") + sections()[i].name + "\": " + std::to_string(s_last_spans.size() > i ? s_last_spans[i] : 0);
    return o + "}";
}

/* FNV-1a's step on 8-byte words (the tail byte by byte): a few hundred KB a frame */
uint64_t state_digest(const Snapshot &snap)
{
    uint64_t h = 0xCBF29CE484222325ULL;
    for (size_t i = 0; i < snap.spans.size() && i < sections().size(); i++) {
        if (sections()[i].name == "fiber") continue;
        const uint8_t *p = snap.bytes.data() + snap.spans[i].first;
        size_t n = snap.spans[i].second, k = 0;
        h = (h ^ n) * 0x100000001B3ULL;
        for (; k + 8 <= n; k += 8) {
            uint64_t w;
            memcpy(&w, p + k, 8);
            h = (h ^ w) * 0x100000001B3ULL;
        }
        for (; k < n; k++) h = (h ^ p[k]) * 0x100000001B3ULL;
    }
    return h;
}
