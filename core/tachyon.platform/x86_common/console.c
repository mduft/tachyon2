/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#include <tachyon.platform/architecture.h>

static uint16_t __kcon_x = 0;
static uint16_t __kcon_y = 0;

/* accessed from assembler */
uint16_t __kcon_attr = 0x0700;

void __kcon_reset() {
    asm("cld; rep stosl;"
        :   /* no output */
        :   "D"(VRAM_LOCATION),
            "c"(VRAM_SIZE / 4),
            "a"(0x0F200F20)
        );

    /* hide the cursor */
    asm("outb %%al, %%dx;"
        :   /* no output */
        :   "a"(0xA),
            "d"(0x3D4)
        );
    asm("outb %%al, %%dx;"
        :   /* no output */
        :   "a"(0x20),
            "d"(0x3D5)
        );

    __kcon_x = 0;
    __kcon_y = 0;
}

static void __kcon_scroll1() {
    register int16_t* vram_source = (int16_t*)VRAM_LOCATION + KCON_WIDTH;
    register int16_t* vram_dest = (int16_t*)VRAM_LOCATION;
    register intmax_t move_len = (KCON_WIDTH * KCON_HEIGHT) - KCON_WIDTH;

    while(move_len--) {
        *vram_dest++ = *vram_source++;
    }

    /* clear the last line. vram_dest points there */
    move_len = KCON_WIDTH;
    while(move_len--) {
        *vram_dest++ = ' ' | __kcon_attr;
    }
}

#define CALC_LOCATION (int16_t*)VRAM_LOCATION + (((KCON_WIDTH) * __kcon_y) + __kcon_x)

void __kcon_write(char* str) {
    int16_t* vram_current = CALC_LOCATION;

    while(str && *str) {
        if(*str != '\n' && *str != '\r') {
            *vram_current = *str | __kcon_attr;
            ++__kcon_x;
        }

        if(__kcon_x >= KCON_WIDTH || *str == '\n' || *str == '\r') {
            __kcon_x = 0;
            if(*str != '\r') {
                ++__kcon_y;
                vram_current = CALC_LOCATION - 1;
            }
        }

        if(__kcon_y >= KCON_HEIGHT) {
            __kcon_scroll1();
            __kcon_y = KCON_HEIGHT - 1;
            vram_current = CALC_LOCATION - 1;
        }

        ++str;
        ++vram_current;
    }
}
