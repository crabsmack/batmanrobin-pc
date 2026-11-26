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
| `batmanrobin.asm`, `src/` | A byte-matching disassembly: it rebuilds the cartridge byte for byte. One file per function (`src/f/`, named by start address), 1,343 functions, every one named and documented. |
| `docs/NOTES.md` | What is known about the game's code and data: the engine, the object system, the script VMs, the sound driver, each stage's quirks. |
| `recomp/game/c/` | Hand-written C for every one of those 1,343 functions. |
| `recomp/platform/native/` | The native port: the C on a native runtime with no emulated 68000, plus the enhancement options. |
| `recomp/` | Also a static recompilation of the original with [segagenesisrecomp](https://github.com/crabsmack/segagenesisrecomp) (a submodule), used as the reference the C was checked against. |

Each layer was checked against the one before it:
- The disassembly assembles to the original byte for byte.
- The recompilation matches a reference emulator on every frame of a full-game TAS.
- The hand-written C and the native port match the recompiled game's state (RAM and video
  memory) at every sync point of 209 recorded runs: the TAS, fuzzed play branching from it and
  runs through each of its 12 boss fights, 12.7 million sync points in all.

## Build the ROM from the disassembly

```
python build.py path/to/your/dump.bin
```

It prints `OK: byte-identical` only when the result matches the original byte for byte. It needs
Python 3.11+ and the Macro Assembler AS: Windows binaries are in `tools/as/` (GPL v2); elsewhere,
put `asl` and `p2bin` from https://github.com/Clownacy/asl-releases on PATH. The game's data
(graphics, levels, sound and tables: about 2 MB in some 3,900 spans, all but 289 of them named) is
not reproduced here: each span is an `incrom` line, copied from your dump when you build.

## Build the PC port

CMake 3.20+, a C11/C++17 compiler (Visual Studio 2022 on Windows; GCC or Clang on Linux) and
SDL2 2.24 or newer (bundled with the engine on Windows; `libsdl2-dev` on Linux, as in Ubuntu
24.04 or Debian 12 and later):

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

**Starting without a command line:** run the program on its own and it finds your dump (the one
chosen last, or a dump beside the program; it asks with a file dialog otherwise) and opens a
launcher: the ROM, every option and the controls, then PLAY. Any common dump format works (.md,
.bin, .gen, .smd interleaved): the game is recognised by its bytes, not its file name. The
launcher's checkbox turns it off; `--no-launcher` skips it once. Everything is kept in
`settings.ini` beside the program.

**Controls:** arrow keys, Z / X / C for A / B / C, Enter for Start, or a game controller (face
buttons by position); the launcher's Controls tab remaps both players (two keys and two controller
inputs per button, a controller per player, the dead zone). F11 toggles fullscreen, F9 the high
frame rate, F10 the widescreen mode, F4 the scaling and F8 the shader preset (each shows its new
setting on screen for 2 seconds), Esc quits.

**Quick saves and rewind:** F5 saves to a slot, F7 loads it, F6 picks the next of 4 slots. Holding
Backspace (or a controller's left trigger; the Rewind row of the Controls tab) takes the game back a
frame at a time, as it was, up to a minute, with its sound played backwards. The slots are kept
while the game runs and are gone when it quits (nothing is written to disk). `settings.ini`
`[rewind]`: `seconds = 60` (0-3600; 0 turns rewind off, the quick saves still work), `sound =
reversed` or `silent`.

**If the game stops:** the port runs only code that has been translated to C, and stops with a
message if the game ever reaches code that has not been (or on any other crash). It then saves a
folder in `crash-reports/` beside the executable: `report.txt` says what happened, and
`inputs.input` holds every button press of the session from power-on, so the exact moment can be
replayed (`report.txt` has the command). Please attach that folder to an issue.

## Options

Every enhancement is off by default, so the port plays as the cartridge does, with two
exceptions: `music_voices` (`music_voices=0` is the cartridge's sound) and `scaling`, which
fills the window by default (`scaling=integer` shows whole multiples of the cartridge's pixels).
Set them on the command line, or in `settings.ini` beside the
executable under `[options]` (the command line wins):

```
batmanrobin_native.exe dump.bin --options widescreen=16:9,high_fps=144,unlimited_sprites=1
batmanrobin_native.exe --options --json      # every option, its values and default
```

| Option | Values | What it does |
|---|---|---|
| `widescreen` | `0`, `16:9`, `16:9-gameplay` | A 16:9 view. `16:9`: the game itself plays exactly as in 4:3; the extra margins are drawn from the level, and enemies appear about 40 px inside them, where the original creates them. `16:9-gameplay`: enemies and objects become active across the whole wide view and the players can walk into the margins; fights play slightly differently from the original. **F10** steps through `0`, `16:9` and `16:9-gameplay` in game. |
| `high_fps` | `0`, `120`, `144`, `165`, `240`, or any 61-1000 | Frames in between the game's 60 Hz frames, with sprites and scrolling interpolated. Each 1/60 s is split evenly into the rate's share rounded (120 and 144: 2 pictures, 165: 3, 240: 4), so a display at a multiple of 60 shows every picture for the same time; set your display's rate. The automated checks run it at 120 and 144 Hz. **F9** toggles it in game. |
| `unlimited_sprites` | `0`, `1` | No sprite flicker: no per-line sprite limit and no 80-sprite cap. |
| `scaling` | `sharp_bilinear`, `integer`, `nearest` | How the picture is enlarged to the window. `sharp_bilinear` (the default): fills the window with every game pixel the same size, blending one screen pixel where two meet. `integer`: the largest whole multiple that fits, sharp, with a black border. `nearest`: fills the window, sharp, but at most sizes some pixels come out a screen pixel wider or taller than others. At a whole multiple (the 3x window) all three look the same. **F4** switches it in game. |
| `shader_preset` | a path to a `.slangp` file, or empty (the default: none) | The picture through a RetroArch shader preset, by [librashader](https://github.com/SnowflakePowered/librashader). Off by default, and no presets come with the game: get them from libretro's [slang-shaders](https://github.com/libretro/slang-shaders) and give the `.slangp` file's path (a relative path is from the program's folder; the launcher has a Browse button). The library comes with the game (`librashader.dll` on Windows, the upstream release; `librashader.so` on Linux, built from the same release's source, which needs OpenGL 3.3). Without it, or with a preset that doesn't load, the game says so in its log and shows the plain picture. **F8** turns it off and on in game. |
| `music_voices` | `1`, `0` | The music gets sound chips of its own, so sound effects no longer cut its notes. `0` = shared chips, as on the cartridge. |
| `fm_ladder` | `1`, `0` | `0` turns off the YM2612's "ladder effect" distortion (the cartridge has it). |
| `fm_filter` | `0`, `1` | `1` = a cleaner output low-pass than the console's. |
| `weapon_level` | `0`-`6` | Every player starts with this weapon level (the original starts at 0 and raises it with pickups). Changes the game. |
| `start_stage` | `0`, `2`-`4`, or a section: `1-street`, `1-gem-expo`, `1-warehouse`, `1-parade`, `2-lift`, `2-flight`, `2-clouds`, `2-airship`, `2-beam-ship`, `3-backlot`, `3-cat-head`, `3-mushrooms`, `3-spotted-room`, `3-card-hall`, `3-hatter`, `4-hall`, `4-lab`, `4-tube`, `4-freeze` | A level select: a new game starts at that stage's opening or that section, as the game itself moves on when a section ends. `0` = the beginning. |

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
