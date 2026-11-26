/* launcher.h - the start window (launcher.cpp). */
#pragma once
#include <string>

/* The launcher: the ROM (rom, "" if none found), every option, the controls. true: PLAY, *rom_out is the
   ROM to play and the options are set (and written to settings.ini with the controls); false: Quit. */
bool launcher_run(const std::string &rom, std::string *rom_out);

std::string rom_check_path(const char *path);   /* runner.cpp: "" if it is the cartridge, else why not */
