/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>
#include <tachyon.platform/external.h>

#define MAX_LOG_WRITERS 0x8
#define MAX_WRITE_BUFFER 0x400
#define MAX_NUMBER_DIGITS 0x20

#define KLOG(lvl, fmt, ...) \
    if(Log::instance()->getLevel() <= lvl) { Log::instance()->write("%10s:%4d: " fmt, __FUNCTION__,  __LINE__, __VA_ARGS__); }

#define KTRACE(...) KLOG(Log::Trace,     __VA_ARGS__, 0); 
#define KINFO(...)  KLOG(Log::Info,      __VA_ARGS__, 0);
#define KWARN(...)  KLOG(Log::Warning,   __VA_ARGS__, 0);
#define KERROR(...) KLOG(Log::Error,     __VA_ARGS__, 0);
#define KFATAL(...) KLOG(Log::Fatal,     __VA_ARGS__, 0); abort();
#define KWRITE(...) Log::instance()->write(__VA_ARGS__);

/* TODO: maybe find a better place for this...? */
#define KASSERT(x)      if(!(x)) { KFATAL("assertion fail: %s\n", #x); }
#define KASSERTM(x,m)    if(!(x)) { KFATAL("assertion fail: %s (%s)\n", #x, m); }

typedef void(*log_writer_t)(char const* psz);

class Log {
public:

    enum LogLevel {
        Trace,
        Info,
        Warning,
        Error,
        Fatal
    };

private:
    static Log staticLog;
    log_writer_t writers[MAX_LOG_WRITERS];
    LogLevel level;

    Log() : level(Info) { }
    ~Log() { }

    void multicast(char const* str);
    bool parse_s(intmax_t num, uint8_t base, char* buf, char fill, uint32_t width);
    bool parse_u(uintmax_t num, uint8_t base, char* buf, char fill, uint32_t width);
    uintmax_t parse_num(char const* str);
    void format(char* buf, size_t len, char const* fmt, va_list args);
public:
    static Log* instance();

    void addWriter(log_writer_t);
    void removeWriter(log_writer_t);

    void write(char const* fmt, ...);

    LogLevel getLevel() { return level; }
    void setLevel(LogLevel lvl) { level = lvl; }
};
