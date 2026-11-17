/* dk_opts.h - the native runner's game-changing options the game's native branches read (phase 6,
 * docs/phases/P6-enhance.md R24; recomp/platform/native/options.cpp). Native only (ruling R2): in
 * the emulated-PC modes each is the cartridge's value, so the branches compile to the original and
 * verify keeps proving the C. */
#pragma once
#include <stdint.h>

#if DK_MODE_NATIVE
extern "C" int opt_weapon_level(void);   /* weapon_level: 0 = the cartridge's, else 1-6 */
/* Main_Start's start weapon level (g_StartWeaponLvl, copied to each player's record +$2A when it
   spawns): the option's level when set, else the cartridge's */
#define OPT_START_WEAPON_LVL(v)  (opt_weapon_level() ? (uint16_t)opt_weapon_level() : (uint16_t)(v))
#else
#define OPT_START_WEAPON_LVL(v)  ((uint16_t)(v))
#endif
