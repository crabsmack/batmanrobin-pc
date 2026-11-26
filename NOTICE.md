# Notice

## The game

*The Adventures of Batman and Robin* and its characters are the property of their respective
rights holders. This project is not affiliated with or endorsed by Sega, Warner Bros., DC, Clockwork
Tortoise or anyone else connected with the original game.

This repository contains **no game data**. The disassembly's data spans are `incrom` lines that
copy the bytes from a ROM file you supply yourself when you build, and the port reads the game's
graphics, sound and levels from that file each time it starts. Both check it by SHA-1. The
export that writes this repository refuses ROM-like files.

The MIT license in [LICENSE](LICENSE) covers the original work here. It does not, and cannot,
grant any rights in the original game.

## Third-party components

| Component | Role | License |
|---|---|---|
| [segagenesisrecomp](https://github.com/mstan/segagenesisrecomp) (Matthew Stan), submodule `recomp/engine/` (fork [crabsmack/segagenesisrecomp](https://github.com/crabsmack/segagenesisrecomp)) | The recompiler and engine the port grew from; the port keeps its video model, Z80 and sound chips | PolyForm Noncommercial 1.0.0, so **builds of this port may not be used commercially** |
| [m68k-recomp-core](https://github.com/mstan/m68k-recomp-core), [z80-recomp-core](https://github.com/mstan/z80-recomp-core) | Recompiler cores (the recompilation; the native port has no 68000 left) | their own license files, in the submodules |
| [ymfm](https://github.com/aaronsgiles/ymfm) (Aaron Giles) | YM2612 FM synthesis | BSD-3-Clause |
| [superzazu/z80](https://github.com/superzazu/z80) | Z80 sound CPU | MIT |
| [SDL2](https://libsdl.org) | Window, input, audio | zlib |
| [minicoro](https://github.com/edubart/minicoro) | The game fiber's context switch | Unlicense or MIT-0 (LuaCoco parts: MIT) |
| [clowncommon](https://github.com/Clownacy/clowncommon), [tomlc99](https://github.com/cktan/tomlc99) | Engine helpers, TOML parsing | ISC, MIT |
| [Dear ImGui](https://github.com/ocornut/imgui) (Omar Cornut), `recomp/platform/native/external/imgui/` | The launcher's window | MIT |
| [librashader](https://github.com/SnowflakePowered/librashader) (chyyran and contributors) | RetroArch shader presets on the picture (`shader_preset`): its C headers vendored in `recomp/platform/native/external/librashader/`, the library loaded at run time from `librashader.dll` (Windows) or `librashader.so` (Linux) | headers MIT; the library MPL-2.0, unmodified v0.12.0 (source at the link): on Windows the upstream release, fetched by `recomp/platform/native/native.cmake`; on Linux built from the release's source (tag `librashader-v0.12.0`, commit `87e8a97b`) by `tools/librashader_linux.py` |
| [Karla](https://github.com/googlefonts/karla), [Roboto](https://github.com/googlefonts/roboto), `recomp/platform/native/external/fonts/` | The launcher's fonts | SIL OFL 1.1, Apache 2.0 |
| [The Macro Assembler AS](http://john.ccac.rwth-aachen.de:8000/as/) | `tools/as/`: assembles the disassembly | GPL v2 |
| EZGames69's TAS, [TASVideos 6248M](https://tasvideos.org/6248M) | The input `recomp/verify.py` plays (not included: download it) | TASVideos' terms |

The engine's own list is `recomp/engine/THIRD-PARTY-LICENSES.md`.

Parts of the port's platform layer (the options, the widescreen and high-frame-rate presentation, the
launcher, the controls and the ROM finder)
were adapted from the [X-Men 2: Clone Wars PC port](https://github.com/crabsmack/xmen2-pc) by the
same author, MIT.
