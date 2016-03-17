#include "itoa.h"

u32 itoa(s32 value, unsigned char *sp, u8 radix) {
    u32 len;
    unsigned char          tmp[16], *tp = tmp;
    s32           i, v, sign   = radix == 10 && value < 0;

    v = sign ? -value : value;

    while (v || tp == tmp) {
        i = v % radix;
        v /= radix;
        *tp++ = i < 10 ? (unsigned char) (i + '0') : (unsigned char) (i + 'a' - 10);
    }


    len = tp - tmp;

    if (sign) {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    *sp++ = '\0';

    return len;
}
