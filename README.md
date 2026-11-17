# The Adventures of Batman and Robin (Genesis), on PC

A complete decompilation of *The Adventures of Batman and Robin* for the Sega Genesis / Mega
Drive, and a native PC port built from it, with optional widescreen, high frame rate, no sprite
flicker and cleaner music. It was built mostly by AI agents (Claude Code), each step checked
against the original game.

**No game data is in this repository.** You need your own dump of the cartridge (USA), SHA-1
`1da4b3fc92a6572e70d748ebad346aeb291f89f3`. The build and the port check it and refuse anything else.

## What is here

| | |
|---|---|
| `batmanrobin.asm`, `src/` | A byte-matching disassembly: it rebuilds the cartridge byte for byte. One file per function (`src/f/`, named by start address), 1,317 functions, every one named and documented. |
| `docs/NOTES.md` | What is known about the game's code and data: the engine, the object system, the script VMs, the sound driver, each stage's quirks. |
| `recomp/game/c/` | Hand-written C for every one of those 1,317 functions. |
| `recomp/platform/native/` | The native port: the C on a native runtime with no emulated 68000, plus the enhancement options. |
| `recomp/` | Also a static recompilation of the original with [segagenesisrecomp](https://github.com/crabsmack/segagenesisrecomp) (a submodule), used as the reference the C was checked against. |

Each layer was checked against the one before it:
- The disassembly assembles to the original byte for byte.
- The recompilation matches a reference emulator on every frame of a full-game TAS.
- The hand-written C and the native port match the recompiled game's state (RAM and video
  memory) at every sync point of 172 recorded runs: the TAS plus fuzzed play branching from it,
  10.3 million sync points in all.

## Build the ROM from the disassembly

```
python build.py path/to/your/dump.bin
```

It prints `OK: byte-identical` only when the result matches the original byte for byte. It needs
Python 3.11+ and the Macro Assembler AS: Windows binaries are in `tools/as/` (GPL v2); elsewhere,
put `asl` and `p2bin` from https://github.com/Clownacy/asl-releases on PATH. The game's data
(graphics, levels, sound and tables: about 2 MB in some 3,900 spans, all but 306 of them named) is
not reproduced here: each span is an `incrom` line, copied from your dump when you build.

## Build the PC port

CMake 3.20+, a C11/C++17 compiler (Visual Studio 2022 on Windows; GCC or Clang on Linux) and
SDL2 (bundled with the engine on Windows; `libsdl2-dev` on Linux):

```
git submodule update --init recomp/engine
git -C recomp/engine submodule update --init -- external
cmake -S recomp -B recomp/build -G "Visual Studio 17 2022" -A x64
cmake --build recomp/build --config Release --target batmanrobin_native
recomp/build/Release/batmanrobin_native.exe path/to/your/dump.bin
```

On Linux, leave out `-G ... -A x64` and `--config Release`, add `-DCMAKE_BUILD_TYPE=Release`,
and run `recomp/build/batmanrobin_native`. The port reads the game's graphics, sound and levels
from your dump each time it starts.

**Controls:** arrow keys, Z / X / C for A / B / C, Enter for Start, or a game controller. F11
toggles fullscreen, Esc quits.

## Options

Every enhancement is off by default except `music_voices`, so the port plays as the cartridge
does until you turn them on. Set them on the command line, or in `settings.ini` beside the
executable under `[options]` (the command line wins):

```
batmanrobin_native.exe dump.bin --options widescreen=16:9,high_fps=144,unlimited_sprites=1
batmanrobin_native.exe --options --json      # every option, its values and default
```

| Option | Values | What it does |
|---|---|---|
| `widescreen` | `0`, `16:9`, `16:9-gameplay` | A 16:9 view. `16:9`: the game itself plays exactly as in 4:3; the extra margins are drawn from the level, and enemies appear about 40 px inside them, where the original creates them. `16:9-gameplay`: enemies and objects become active across the whole wide view and the players can walk into the margins; fights play slightly differently from the original. **F10** toggles it in game. |
| `high_fps` | `0`, `120`, `144`, `165`, `240`, or any 61-1000 | The display rate in Hz, with sprites and scrolling interpolated between the game's 60 Hz frames. **F9** toggles it in game. |
| `unlimited_sprites` | `0`, `1` | No sprite flicker: no per-line sprite limit and no 80-sprite cap. |
| `music_voices` | `1`, `0` | The music gets sound chips of its own, so sound effects no longer cut its notes. `0` = shared chips, as on the cartridge. |
| `fm_ladder` | `1`, `0` | `0` turns off the YM2612's "ladder effect" distortion (the cartridge has it). |
| `fm_filter` | `0`, `1` | `1` = a cleaner output low-pass than the console's. |
| `weapon_level` | `0`-`6` | Every player starts with this weapon level (the original starts at 0 and raises it with pickups). Changes the game. |

## Check the recompilation against the original

The recompiler runs as part of this build and writes the generated C into the build tree, so it
needs your dump in `recomp/game/` first (the file is gitignored):

```
cp path/to/your/dump.bin recomp/game/batmanrobin.bin
cmake --build recomp/build --config Release --target batmanrobin_recomp
python recomp/verify.py path/to/ezgames69-adventuresofbatmanandrobin-1p.bk2
```

It plays EZGames69's TAS ([TASVideos publication 6248M](https://tasvideos.org/6248M); unzip the
download) on the recompiled runner, and compares RAM, VRAM, CRAM and VSRAM after every one of its
130,772 frames with a reference emulator's checksums, stored in `recomp/reference/`.

## License

The original work here (the disassembly's names, comments and structure, the hand-written C, the
platform layer, the tools and the documentation) is MIT: see [LICENSE](LICENSE). It grants no
rights in the game itself. The engine the port is built on is PolyForm Noncommercial, so **builds
of the port may not be used commercially**. Details and third-party components:
[NOTICE.md](NOTICE.md).
