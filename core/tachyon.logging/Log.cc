#include <tachyon.logging/Log.h>

Log& Log::instance() {
    static Log staticLog;
    return staticLog;
}

void Log::addWriter(log_writer_t writer) {
    for(register int i = 0; i < MAX_LOG_WRITERS; ++i) {
        if(!writers[i]) {
            writers[i] = writer;
            return;
        }
    }

    KWARN("no more room to add writer %p\n", writer);
}

void Log::removeWriter(log_writer_t writer) {
    for(register int i = 0; i < MAX_LOG_WRITERS; ++i) {
        if(writers[i] == writer) {
            writers[i] = 0;
            return;
        }
    }

    KWARN("writer %p did not exist\n", writer);
}

void Log::multicast(char const* str) {
    for(register int i = 0; i < MAX_LOG_WRITERS; ++i) {
        if(writers[i] != 0) {
            writers[i](str);
        }
    }
}

bool Log::parse_s(intmax_t num, uint8_t base, char* buf, char fill, uint32_t width) {
    if(!buf)
        return false;

    if(num < 0) {
        buf[0] = '-';
        return parse_u(static_cast<uintmax_t>(-num), base, &buf[1], fill, (width == 0 ? 0 : width - 1));
    } else {
        return parse_u(static_cast<uintmax_t>(num), base, buf, fill, width);
    }
}

bool Log::parse_u(uintmax_t num, uint8_t base, char* buf, char fill, uint32_t width) {
    if(!buf)
        return false;

    register char * p = buf;

    do {
        register uintmax_t rem = num % base;
        *p++ = (rem < 10) ? rem + '0' : rem + 'a' - 10;
    } while(num /= base);

    /* fill with zeros. */
    if(fill) {
        while((p - buf) < width) {
            *p++ = fill;
        }
    }

    *p = 0;

    /* reverse */
    register char * p1 = buf;
    register char * p2 = p - 1;

    while(p1 < p2) {
        register char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return true;
}

uintmax_t Log::parse_num(char const* str) {
    uintmax_t val = 0;

    while(*str >= '0' && *str <= '9') {
        val*= 10;
        val += *str - '0';
        ++str;
    }

    return val;
}

void Log::format(char* buffer, size_t len, char const* fmt, va_list args) {
    char c;
    char* p = buffer;

    #define CHECKED_APPEND(c) { if((p - buffer) < len) *p++ = (c); }

    while((c = *fmt++) != 0) {
        if(c != '%') {
            CHECKED_APPEND(c);
        } else {
            char  temp[MAX_NUMBER_DIGITS];
            char * ptemp = temp;
            char const* append = 0;
            bool lng = false;
            uint32_t width = 0;
            c = *fmt++;

            /* field width specification */
            if(c >= '0' && c <= '9') {
                width = parse_num((fmt - 1));

                while(c >= '0' && c <= '9') {
                    c = *fmt++;
                }
            }

            /* "long" switch */
            if(c == 'l') {
                lng = true;
                c = *fmt++;
            }

            #define DO_NUMBER(s, t, b, f, w)                \
                parse_##s(va_arg(args, t), b, ptemp, f, w); \
                append = temp;                              \
                goto string;

            switch(c) {
            case 'd':   if(lng) { DO_NUMBER(s, int64_t, 10, ' ', width);   }
                        else    { DO_NUMBER(s, int32_t, 10, ' ', width);  }
            case 'u':   if(lng) { DO_NUMBER(s, uint64_t, 10, ' ', width);  }
                        else    { DO_NUMBER(s, uint32_t, 10, ' ', width); }
            case 'x':   if(lng) { DO_NUMBER(u, uint64_t, 16, '0', width);  }
                        else    { DO_NUMBER(u, uint32_t, 16, '0', width); }
            case 'p':   *ptemp++ = '0'; *ptemp++ = 'x'; DO_NUMBER(u, uintmax_t, 16, '0', sizeof(uintmax_t) * 2);
            case 'c':
                temp[0] = static_cast<char>(va_arg(args, int));
                temp[1] = 0;
                append = temp;
                goto string;
            case 's':
                append = va_arg(args, char const*);
                /* fall through to label */
            string:
                if(!append)
                    append = "(null)";

                {
                    size_t len = 0;
                    while(*append) {
                        CHECKED_APPEND(*append++);
                        len++;
                    }

                    while(len++ < width) {
                        CHECKED_APPEND(' ');
                    }
                }

                break;

            default:
                append = "(unknown format)";
                goto string;
            }
        }

        if((p - buffer) >= len) break;
    }

    /* terminate */
    *p = 0;

    #undef DO_NUMBER
    #undef CHECKED_APPEND
}

void Log::write(char const* fmt, ...) {
    /* TODO: dynamic memory? or stay static? */
    char buffer[MAX_WRITE_BUFFER];
    va_list args;

    va_start(args, fmt);
    format(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    multicast(buffer);
}
