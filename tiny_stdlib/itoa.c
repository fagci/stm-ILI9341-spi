#include "itoa.h"

unsigned long itoa(int value, char *sp, int radix) {
    char tmp[16];
    char *tp = tmp;
    int  i;
    int  v;

    int sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned) value;

    while (v || tp == tmp) {
        i = v % radix;
        v /= radix;
        if (i < 10)
            *tp++ = (char) (i + '0');
        else
            *tp++ = (char) (i + 'a' - 10);
    }

    unsigned long len = tp - tmp;

    if (sign) {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    return len;
}