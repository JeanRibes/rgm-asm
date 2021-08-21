//
// Created by jean on 21/08/2021.
//

#ifndef M5HEX_STORE_H
#define M5HEX_STORE_H
#include "FS.h"
#include "SPIFFS.h"
#include "rgb_asm.h"

void fs_tests();
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void store_prgm(String path="/default.prgm");
void load_prgm(String path="/default.prgm");
#endif //M5HEX_STORE_H
