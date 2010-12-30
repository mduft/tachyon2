/* Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
 * This file is part of the 'tachyon' operating system. */

#pragma once

#include <tachyon.platform/architecture.h>

#define MB_MAGIC            0x2badb002

#define MB_INFO_MEMORY      0x001
#define MB_INFO_DEVICE      0x002
#define MB_INFO_CMDLINE     0x004
#define MB_INFO_MODS        0x008
#define MB_INFO_AOUT_SYMS   0x010
#define MB_INFO_ELF_SYMS    0x020
#define MB_INFO_MEMMAP      0x040
#define MB_INFO_DRIVES      0x080
#define MB_INFO_CONFIG_TBL  0x100
#define MB_INFO_LOADER_NAM  0x200
#define MB_INFO_APM_TABLE   0x400
#define MB_INFO_VIDEO       0x800

class MultiBootInformation {

    struct mb_info {
        uint32_t    flags;
        uint32_t    mem_lower;
        uint32_t    mem_upper;
        uint32_t    boot_device;
        uint32_t    cmdline;
        uint32_t    mods_count;
        uint32_t    mods_addr;
        uint32_t    esym_shdr_num;
        uint32_t    esym_shdr_size;
        uint32_t    esym_shdr_addr;
        uint32_t    esym_shdr_shndx;
        uint32_t    mmap_length;
        uint32_t    mmap_addr;
        uint32_t    drives_length;
        uint32_t    drives_addr;
        uint32_t    config_table;
        uint32_t    boot_loader_name;
        uint32_t    apm_table;
        uint32_t    vbe_control_info;
        uint32_t    vbe_mode_info;
        uint16_t    vbe_mode;
        uint16_t    vbe_interface_seg;
        uint16_t    vbe_interface_off;
        uint16_t    vbe_interface_len;
    } PACKED;

    mb_info* info;

    uintmax_t mmap_count;

public:

    class Module {
        struct mb_module {
            uint32_t    mod_start;
            uint32_t    mod_end;
            uint32_t    string;
            uint32_t    reserved;
        } PACKED;

        mb_module* pmod;

        friend class MultiBootInformation;
        Module(uint32_t mod_base, uint32_t mod_num)
            :   pmod(reinterpret_cast<mb_module*>(mod_base) + mod_num) {}
    public:
        uintptr_t getStart()    { return pmod->mod_start; }
        uintptr_t getEnd()      { return pmod->mod_end; }
        char const* getString() { return reinterpret_cast<char const*>(pmod->string); }
    };

    class MemoryRange {
        struct mb_mmap {
            uint32_t    size;
            uint64_t    addr;
            uint64_t    len;
            uint32_t    type;
        } PACKED;

        mb_mmap* pinfo;

        friend class MultiBootInformation;
        MemoryRange(uint32_t mmap_base, uint32_t mmap_num);
        static uintmax_t countEntries(uint32_t mmap_base, uint32_t mmap_length);
    public:
        uint64_t getStart()    { return pinfo->addr; }
        uint64_t getEnd()      { return pinfo->addr + pinfo->len; }
        uintmax_t getLength()   { return pinfo->len; }
        bool isAvailable() { return (pinfo->type == 1); }
    };

    MultiBootInformation(void* mbi);

    uint32_t getFlags()         { return info->flags; }
    uint32_t getLowMemoryKB()   { if(!(info->flags & MB_INFO_MEMORY)) return 0; return info->mem_lower; }
    uint32_t getHighMemoryKB()  { if(!(info->flags & MB_INFO_MEMORY)) return 0; return info->mem_upper; }

    char const* getCommandLine()    { return reinterpret_cast<char const*>(info->cmdline); }
    uintmax_t getMemRangeCount()    { return mmap_count; }
    MemoryRange getMemRange(uintmax_t index);
};
