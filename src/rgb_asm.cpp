//
// Created by jean on 20/08/2021.
//

#include "rgb_asm.h"
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <stdio.h>
#include <FastLED.h>


void parse_mem_action(struct instruction *ins, char *rest) {
    int addr, value;
    char suffix = '0';
    sscanf(rest, "%d,%d%c", &addr, &value, &suffix);
    ins->mem.address = addr;
    ins->mem.value = value;
    ins->immediate = suffix == 'i';
}

bool parse_instr(struct instruction *ins, const char *str) {
    char type[16];
    char rest[64];
    sscanf(str, "%s %s", type, rest);
    //printf("rest: %s\n", rest);
    if (strcmp(type, "LED") == 0) {
        ins->type = LED;
        int n, r, g, b;
        char suffix = '0';
        sscanf(rest, "%d,%d,%d,%d%c", &n, &r, &g, &b, &suffix);
        ins->led.n = n;
        ins->led.r = r;
        ins->led.g = g;
        ins->led.b = b;
        ins->immediate = suffix == 'i';
        return true;
    }
    if (strcmp(type, "JMP") == 0) {
        int len;
        char direction;
        char suffix1 = '0';
        char suffix2 = '0';
        sscanf(rest, "%c%d%c%c", &direction, &len, &suffix1, &suffix2);
        ins->type = JMP;
        ins->jmp.forward = direction != '-';
        ins->jmp.len = len;
        ins->jmp.conditionnal = suffix2 == 'c' || suffix1 == 'c';
        ins->immediate = suffix1 == 'i' || suffix2 == 'i';
        return true;
    }
    if (strcmp(type, "CMP") == 0) {
        ins->type = CMP;
        parse_mem_action(ins, rest);
        return true;
    }

    if (strcmp(type, "STORE") == 0) {
        ins->type = STORE;
        parse_mem_action(ins, rest);
        return true;
    }

    if (strcmp(type, "ADD") == 0) {
        ins->type = ADD;
        parse_mem_action(ins, rest);
        return true;
    }

    if (strcmp(type, "SUB") == 0) {
        ins->type = SUB;
        parse_mem_action(ins, rest);
        return true;
    }
    if (strcmp(type, "MUL") == 0) {
        ins->type = MUL;
        parse_mem_action(ins, rest);
        return true;
    }
    if (strcmp(type, "DBG") == 0) {
        ins->type = DBG;
        int address;
        sscanf(rest, "%d", &address);
        ins->mem.address = address;
        return true;
    }
    if (strcmp(type, "SHOW") == 0) {
        ins->type = SHOW;
        return true;
    }
    if (strcmp(type, "RAND") == 0) {
        ins->type = RAND;
        int addr;
        sscanf(rest, "%d", &addr);
        ins->mem.address = addr;
        return true;
    }
    if (strcmp(type, "SLEEP") == 0) {
        ins->type = SLEEP;
        int time;
        char suffix = '0';
        sscanf(rest, "%d%c", &time, &suffix);
        ins->mem.value = time;
        ins->immediate = suffix == 'i';
        return true;
    }
    if (strcmp(type, "LNJMP") == 0) {
        ins->type = LNJMP;
        int addr;
        sscanf(rest, "%d", &addr);
        ins->longjump = addr;
        return true;
    }
    if (strcmp(type, "ABSJMP") == 0) {
        ins->type = ABSJUMP;
        int addr;
        sscanf(rest, "%d", &addr);
        ins->longjump = addr;
        return true;
    }
    return false;
}

void execute(std::vector<instruction> prgm) {
    int memory[255];
    int pc = 0;
    bool cmp;
    memset(&memory, 0, sizeof(memory));
    printf("size: %d\n", prgm.size());

    while (pc < prgm.size()) {
        auto ins = prgm[pc];
        printf("[%d]> ", pc);
        if (ins.type == LED) {
            if (ins.immediate)
                led(memory[ins.led.n], memory[ins.led.r], memory[ins.led.g], memory[ins.led.b]);
            else
                led(ins.led.n, ins.led.r, ins.led.g, ins.led.b);
        }

        if (ins.type == JMP) {
            if ((ins.jmp.conditionnal && cmp) || !ins.jmp.conditionnal) {
                int diff;
                if (ins.immediate)
                    diff = memory[ins.jmp.len];
                else
                    diff = ins.jmp.len;
                if (ins.jmp.forward)
                    pc += diff;
                else
                    pc -= diff + 1;
                printf("jmp: now at %d\n", pc);
            } else
                puts("no jump");
        }

        if (ins.type == LNJMP) {
            if ((ins.jmp.conditionnal && cmp) || !ins.jmp.conditionnal) {
                pc += ins.longjump;
                printf("long jump : %d\n", ins.longjump);
            }
        }
        if (ins.type == ABSJUMP) {
            if ((ins.jmp.conditionnal && cmp) || !ins.jmp.conditionnal) {
                pc = ins.longjump;
                printf("long jump : %d\n", ins.longjump);
            }
        }

        if (ins.type == CMP) {
            if (ins.immediate) // inversé
                cmp = memory[ins.mem.address] > ins.mem.value;
            else
                cmp = memory[ins.mem.address] < ins.mem.value;
            printf("result cmp %d %c %d: %d\n", memory[ins.mem.address], ins.immediate ? '>' : '<', ins.mem.value, cmp);
        }
        if (ins.type == STORE) {
            if (ins.immediate)
                memory[ins.mem.address] = memory[ins.mem.value];
            else
                memory[ins.mem.address] = ins.mem.value;
            printf("stored %d at %d\n", memory[ins.mem.address], ins.mem.address);
        }

        if (ins.type == ADD) {
            if (ins.immediate)
                memory[ins.mem.address] += memory[ins.mem.value];
            else
                memory[ins.mem.address] += ins.mem.value;
            printf("added: %d at %d\n", memory[ins.mem.address], ins.mem.address);
        }

        if (ins.type == SUB) {
            if (ins.immediate)
                memory[ins.mem.address] -= memory[ins.mem.value];
            else
                memory[ins.mem.address] -= ins.mem.value;
            printf("substract: %d at %d\n", memory[ins.mem.address], ins.mem.address);
        }

        if (ins.type == MUL) {
            if (ins.immediate)
                memory[ins.mem.address] *= memory[ins.mem.value];
            else
                memory[ins.mem.address] *= ins.mem.value;
            printf("multiply: %d at %d\n", memory[ins.mem.address], ins.mem.address);
        }

        if (ins.type == DBG) {
            printf("DBG: %d at %d\n", memory[ins.mem.address], ins.mem.address);
        }

        if (ins.type == SHOW) {
            //puts("showing leds");
            FastLED.show();
        }
        if (ins.type == SLEEP) {
            if (!ins.immediate)
                delay(10 * ins.mem.value);
            else
                delay(10 * memory[ins.mem.value]);
            /*if(ins.immediate)
                vTaskDelay(10 * ins.mem.value / portTICK_PERIOD_MS);
            else
                vTaskDelay(10 * memory[ins.mem.value] / portTICK_PERIOD_MS);
                */
        }
        if (ins.type == RAND) {
            memory[ins.mem.address] = random(0, 255);
            printf("random value: %d\n", memory[ins.mem.address]);
        }

        pc += 1;
    }
    puts("");

}

void print_mem_action(struct instruction *ins) {
    Serial.printf("%d,%d",ins->mem.address, ins->mem.value);
}

void print_instruction(struct instruction *ins) {
    if (ins->type == LED) {
        Serial.print("LED ");
        Serial.printf("%d,%d,%d,%d", ins->led.n, ins->led.r, ins->led.g, ins->led.b);
    }
    if (ins->type == JMP) {
        Serial.print("JMP ");
        Serial.printf("%c%d", ins->jmp.forward ? '+' : '-', ins->jmp.len);
        if (ins->jmp.conditionnal)
            Serial.write('c');
    }
    if (ins->type == CMP) {
        Serial.print("CMP ");
        print_mem_action(ins);
    }
    if (ins->type == STORE) {
        Serial.print("STORE ");
        print_mem_action(ins);
    }
    if (ins->type == ADD) {
        Serial.print("ADD ");
        print_mem_action(ins);
    }
    if (ins->type == SUB) {
        Serial.print("SUB ");
        print_mem_action(ins);
    }
    if (ins->type == MUL) {
        Serial.print("MUL ");
        print_mem_action(ins);
    }
    if (ins->type == DBG) {
        Serial.print("DBG ");
        Serial.printf("%d", ins->mem.address);
    }
    if (ins->type == SHOW) { Serial.print("SHOW "); }
    if (ins->type == SLEEP) { Serial.print("SLEEP "); }
    if (ins->type == RAND) {
        Serial.print("RAND ");
        Serial.printf("%d", ins->mem.address);
    }
    if (ins->type == LNJMP) {
        Serial.print("LNJMP ");
        Serial.printf("%d", ins->mem.address);
    }
    if (ins->type == ABSJUMP) {
        Serial.print("ABSJUMP ");
        Serial.printf("%d", ins->mem.address);
    }

    if (ins->immediate)
        Serial.write('i');
    Serial.println();
}