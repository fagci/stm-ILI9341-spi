#include "stm32f10x.h"
#include "font.h" 
#include "correct_ANSI.h"

unsigned char corr_ANSI(unsigned char *s, unsigned char z) {
    switch (s[z]) {
        case 134:
            return 95;
        case 150:
            return 96;
        case 151:
            return 97;
        case 168:
            return 98;
        case 169:
            return 99;
        case 174:
            return 100;
        case 176:
            return 101;
        case 184:
            return 102;
        case 185:
            return 103;
        default:
            return 32;
    }
}

	