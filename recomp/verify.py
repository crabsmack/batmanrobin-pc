"""Check the recompilation against the original game, frame by frame, without an emulator.

    python recomp/verify.py path/to/ezgames69-adventuresofbatmanandrobin-1p.bk2 [--frames N]

The reference, reference/tas.frames.csv.gz, holds the CRC-32s of 68000 RAM, VRAM, CRAM, VSRAM, z80_crc32 after
every frame, as the reference emulator (BizHawk 2.9.1's Genesis core) produced them playing
the TAS https://tasvideos.org/6248M (the movie file's SHA-1: 8056abdec1896ff185614de50ee3ae723ba49f40). This plays the same input on
the Release runner (build it first: README) and compares every frame. Exit 0 only when every
frame is equal. Needs Python 3.11+ and your dump at recomp/game/batmanrobin.bin.
"""
from __future__ import annotations

import argparse
import csv
import gzip
import hashlib
import io
import os
import re
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
RUNNER = "batmanrobin_recomp"
ROM_FILE = "batmanrobin.bin"
ROM_SHA1 = "1da4b3fc92a6572e70d748ebad346aeb291f89f3"
MOVIE_SHA1 = "8056abdec1896ff185614de50ee3ae723ba49f40"
REFERENCE = HERE / "reference" / "tas.frames.csv.gz"
COMPARED = ['ram_crc32', 'vram_crc32', 'cram_crc32', 'vsram_crc32', 'z80_crc32']
SETTINGS = '[input.p1]\ndevice = 0\npad_type = 0\n\n[input.p2]\ndevice = 0\npad_type = 0\n\n'
BK2_ORDER = "UDLRABCSXYZM"   # BizHawk's GEN input log
RUNNER_BITS = {"U": 0x001, "D": 0x002, "L": 0x004, "R": 0x008, "B": 0x010, "C": 0x020, "A": 0x040,
               "S": 0x080, "X": 0x100, "Y": 0x200, "Z": 0x400, "M": 0x800}


def movie_frames(path: Path) -> list[tuple[int, int]]:
    """[(p1, p2)] in runner bits, one per movie frame (frame k of the reference is entry k-1)."""
    with zipfile.ZipFile(path) as z:
        log = z.read("Input Log.txt").decode()
    out = []
    for line in log.splitlines():
        if not line.startswith("|"):
            continue
        parts = line.strip("|").split("|")
        if parts[0].strip("."):
            raise SystemExit(f"movie frame {len(out) + 1} has a console event {parts[0]!r} (a reset): not playable")
        pads = [sum(RUNNER_BITS[ch] for ch, c in zip(BK2_ORDER, p) if c != ".") for p in parts[1:3]]
        out.append(tuple((pads + [0, 0])[:2]))
    return out


def script(frames: list[tuple[int, int]]) -> str:
    """The runner's input script: PADS p1 p2 (hex), then WAIT n frames."""
    lines, i = [], 0
    while i < len(frames):
        j = i
        while j < len(frames) and frames[j] == frames[i]:
            j += 1
        lines += [f"PADS {frames[i][0]:X} {frames[i][1]:X}", f"WAIT {j - i}"]
        i = j
    return "\n".join(lines) + "\n"


def rows(text: str) -> dict[int, dict[str, str]]:
    return {int(r["frame"]): r for r in csv.DictReader(io.StringIO(text))}


def sha1(path: Path) -> str:
    return hashlib.sha1(path.read_bytes()).hexdigest()


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="verify.py", description=__doc__.split("\n\n")[0])
    ap.add_argument("movie", help="the TAS movie (.bk2)")
    ap.add_argument("--frames", type=int, help="stop after N frames (default: the whole reference)")
    a = ap.parse_args(argv)
    exe = next((p for p in (HERE / "build" / "Release" / f"{RUNNER}.exe", HERE / "build" / RUNNER) if p.is_file()), None)
    if not exe:
        print(f"FAIL: no runner under {HERE / 'build'}: build it first (README)")
        return 1
    rom = HERE / "game" / ROM_FILE
    if not rom.is_file() or sha1(rom) != ROM_SHA1:
        print(f"FAIL: recomp/game/{ROM_FILE} missing or not the expected dump (SHA-1 {ROM_SHA1})")
        return 1
    movie = Path(a.movie)
    if not movie.is_file() or sha1(movie) != MOVIE_SHA1:
        print(f"FAIL: {movie} missing or not the reference movie (SHA-1 {MOVIE_SHA1})")
        return 1
    ref = rows(gzip.decompress(REFERENCE.read_bytes()).decode())
    n = min(a.frames or len(ref), len(ref))
    frames = movie_frames(movie)[:n]
    work = HERE / "build" / "verify"
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    for f in exe.parent.iterdir():   # a private copy: the runner writes settings and evidence beside itself
        if f.is_file() and (f.name == exe.name or f.suffix.lower() in (".dll", ".so", ".dylib")):
            shutil.copy2(f, work / f.name)
    (work / "settings.ini").write_text(SETTINGS)
    (work / "input.input").write_text(script(frames), newline="\n")
    out = work / "frames.csv"
    env = {**os.environ, "GENESIS_NO_LAUNCHER": "1", "SDL_AUDIODRIVER": "dummy", "DKIT_TRACE_CSV": str(out),
           "SDL_RENDER_DRIVER": "software"}   # --benchmark never presents: no GPU driver to fail
    print(f"playing {n} frames on {exe.name} ...", flush=True)
    r = subprocess.run([str(work / exe.name), str(rom), "--benchmark", str(n), "--input-script", "input.input",
                        "--fresh-evidence"], cwd=work, env=env, capture_output=True, text=True, errors="replace")
    (work / "runner.log").write_text(r.stdout + r.stderr)
    got = rows(out.read_text()) if out.is_file() else {}
    for k in range(1, n + 1):
        g = got.get(k)
        bad = ["row missing"] if g is None else [c for c in COMPARED if g.get(c, "").lower() != ref[k][c].lower()]
        if bad:
            print(f"FAIL: first divergence at frame {k}: {', '.join(bad)} (runner exit {r.returncode}; "
                  f"log: {work / 'runner.log'})")
            return 1
    counters = re.search(r"counters (\{.*\})", r.stderr)
    print(f"OK: {n} frames equal" + (f"; runner counters {counters.group(1)}" if counters else ""))
    return 0 if r.returncode == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
