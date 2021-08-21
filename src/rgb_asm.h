//
// Created by jean on 20/08/2021.
//

#ifndef M5HEX_RGB_ASM_H
#define M5HEX_RGB_ASM_H

#include <cstdint>
#include <vector>

enum instructions {
    LED,
    JMP,
    CMP,
    STORE,
    ADD,
    SUB,
    MUL,
    DBG,
    SHOW,
    SLEEP,
    RAND,
    LNJMP,
    ABSJUMP,
};

struct led_action {
    uint8_t n;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct mem_action {
    uint8_t address;
    uint8_t value;
};

struct jump_action {
    bool forward;
    uint8_t len;
    bool conditionnal; // basé sur le précédent 'cmp'
};

struct instruction {
    instructions type;
    bool immediate;
    union {
        struct led_action led;
        struct jump_action jmp;
        struct mem_action mem;
        int longjump;
    };
};
extern std::vector<instruction> prgm;
void led(uint8_t n, uint8_t r, uint8_t g, uint8_t b);
bool parse_instr(struct instruction *ins, const char *str);
void print_instruction(struct instruction* ins);
void execute(std::vector<instruction> prgm);
#endif //M5HEX_RGB_ASM_H
