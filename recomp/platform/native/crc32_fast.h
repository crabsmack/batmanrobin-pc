/* crc32_fast.h - CRC-32 (IEEE 802.3, reflected 0xEDB88320), the same values as the byte-at-a-time
 * table loop, eight bytes a step (slicing-by-8). The sync rows' columns and game_crc32 hash
 * ~200 KB a runner frame; byte at a time that was ~0.3 ms of every frame of every corpus pass. */
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error "crc32_fast.h reads words little-endian"
#endif

static inline const uint32_t (*crc32_fast_tables(void))[256]
{
    static uint32_t t[8][256];
    static int ready;
    if (!ready) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int k = 0; k < 8; k++) c = (c & 1u) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            t[0][i] = c;
        }
        for (uint32_t i = 0; i < 256; i++)
            for (int s = 1; s < 8; s++) t[s][i] = (t[s - 1][i] >> 8) ^ t[0][t[s - 1][i] & 0xFFu];
        ready = 1;
    }
    return t;
}

/* c: the running register (start 0xFFFFFFFF, final ^ 0xFFFFFFFF), as the byte loop's */
static inline uint32_t crc32_fast_update(uint32_t c, const uint8_t *p, size_t n)
{
    const uint32_t (*t)[256] = crc32_fast_tables();
    while (n && ((uintptr_t)p & 7u)) { c = t[0][(c ^ *p++) & 0xFFu] ^ (c >> 8); n--; }
    while (n >= 8) {
        uint32_t lo, hi;
        memcpy(&lo, p, 4);
        memcpy(&hi, p + 4, 4);   /* little-endian host (x86-64, arm64) */
        lo ^= c;
        c = t[7][lo & 0xFFu] ^ t[6][(lo >> 8) & 0xFFu] ^ t[5][(lo >> 16) & 0xFFu] ^ t[4][lo >> 24]
          ^ t[3][hi & 0xFFu] ^ t[2][(hi >> 8) & 0xFFu] ^ t[1][(hi >> 16) & 0xFFu] ^ t[0][hi >> 24];
        p += 8;
        n -= 8;
    }
    while (n--) c = t[0][(c ^ *p++) & 0xFFu] ^ (c >> 8);
    return c;
}
