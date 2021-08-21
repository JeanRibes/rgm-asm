/*
    Please install FastLED library first.
    In arduino library manage search FastLED
*/
#include <M5StickC.h>
#include "FastLED.h"
#include "rgb_asm.h"
#include "FS.h"
#include "SPIFFS.h"
#include "store.h"

#define Neopixel_PIN    32
#define NUM_LEDS    38
#define FORMAT_SPIFFS_IF_FAILED true

CRGB leds[NUM_LEDS];


CRGB mix(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0) {
    return (green << 16) | (red << 8) | blue;
}

static TaskHandle_t task_prgm;
std::vector<instruction> prgm;

void TaskRunProgram(void *pv){
    execute(prgm);
    vTaskDelete(nullptr);
}

void setup() {
    Serial.begin(115200);
    M5.begin(false);
    M5.Axp.SetLDO2(false);

    // Neopixel initialization
    FastLED.addLeds<WS2811, Neopixel_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(3);
    leds[0] = mix(255, 255, 255);
    FastLED.show();
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    fs_tests();
    load_prgm();
}

void led(uint8_t n, uint8_t r, uint8_t g, uint8_t b) {
    leds[n] = mix(r, g, b);
}



void loop() {
    String input = Serial.readStringUntil('\n');
    if (input.length() >= 3) {
        Serial.printf("read: %s\n", input.c_str());
        auto new_ins = instruction();
        if (parse_instr(&new_ins, input.c_str())) {
            prgm.push_back(new_ins);
        } else {
            if (input.equals("RUN")) {
                //vTaskDelete(task_prgm);
                /*if(xTaskCreate(TaskRunProgram, "prgm",4096, nullptr,1,&task_prgm)){
                    puts("running");
                }*/
                execute(prgm);
            }
            if (input.equals("RESET")) {
                prgm.clear();
                for(auto & led : leds){
                    led=0;
                }
                FastLED.show();
            }
            if(input.equals("STOP")){
                vTaskDelete(task_prgm);
            }
            if(input.equals("SIZE")){
                printf("prgm current size: %d\n", prgm.size());
            }
            if(input.startsWith("SAVE")){
                Serial.printf("path:%s\n",input.substring(5).c_str());
                store_prgm();
            }
            if(input.startsWith("LOAD")){
                Serial.printf("path:%s\n",input.substring(5).c_str());
                load_prgm();
            }
            if(input.equals("LIST")){
                int i=0;
                Serial.printf("%d: ", i);
                for (auto ins : prgm){
                    print_instruction(&ins);
                    i+=1;
                }
            }
        }
    }
    if(M5.BtnA.isPressed() || M5.BtnB.isPressed()){
        execute(prgm);
    }
    M5.update();
}

