//
// Created by tjp56 on 2023/4/21.
//

#ifndef RVLD_DEFINE_H
#define RVLD_DEFINE_H

#include "elf.h"


enum class FileType : uint32_t
{
    Unknown,
    Empty,
    ELF,
    Archive,
};

enum class MachineType : uint8_t
{
    None,
    RISCV64,
};

enum class ChunkKind
{
    HEADER,
    OUTPUT_SECTION,
    SYNTHETIC
};

constexpr uint64_t IMAGE_BASE  = 0x200000;

namespace ELF
{
    using Elf64_Word = uint32_t;
    using Elf64_Half = uint16_t;
    using Elf64_Addr = uint64_t;
    using Elf64_Xword = uint64_t;

    using Elf32_Word = uint32_t;
    using Elf32_Half = uint16_t;
    using Elf32_Addr = uint32_t;

    using Elf32_Off = uint32_t;
    using Elf64_Off = uint64_t ;

    /**
     * Special section indices in the ELF format
     */
    enum class ElfSpecialSectionIndex : uint16_t {
        UNDEFINED = 0,          // Undefined section
        LORESERVE = 0xff00,     // Start of reserved indices
        LOPROC    = 0xff00,     // Start of processor-specific section range
        BEFORE    = 0xff00,     // Order section before all others (used by Solaris)
        AFTER     = 0xff01,     // Order section after all others (used by Solaris)
        HIPROC    = 0xff1f,     // End of processor-specific section range
        LOOS      = 0xff20,     // Start of OS-specific section range
        HIOS      = 0xff3f,     // End of OS-specific section range
        ABSOLUTE  = 0xfff1,     // Associated symbol is absolute
        COMMON    = 0xfff2,     // Associated symbol is common
        XINDEX    = 0xffff,     // Index is in extra table
        HIRESERVE = 0xffff      // End of reserved indices
    };


    enum class STB : uint8_t
    {
        LOCAL = 0,          // Local symbol
        GLOBAL = 1,         // Global symbol
        WEAK = 2,           // Weak symbol
        NUM = 3,            // Number of defined types
        LOOS = 10,          // Start of OS-specific
        GNU_UNIQUE = 10,    // Unique symbol (GNU extension)
        HIOS = 12,          // End of OS-specific
        LOPROC = 13,        // Start of processor-specific
        HIPROC = 15         // End of processor-specific
    };

    struct Elf64_Ehdr
    {
        uint8_t e_ident[16];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint64_t e_entry;
        uint64_t e_phoff;
        uint64_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    };

    struct Elf32_Ehdr
    {
        uint8_t e_ident[16];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint32_t e_entry;
        uint32_t e_phoff;
        uint32_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    };

    struct Elf64_Shdr
    {
        uint32_t sh_name;
        uint32_t sh_type;
        uint64_t sh_flags;
        uint64_t sh_addr;
        uint64_t sh_offset;
        uint64_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint64_t sh_addralign;
        uint64_t sh_entsize;
    };

    struct Elf32_Shdr
    {
        uint32_t sh_name;
        uint32_t sh_type;
        uint32_t sh_flags;
        uint32_t sh_addr;
        uint32_t sh_offset;
        uint32_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint32_t sh_addralign;
        uint32_t sh_entsize;
    };

    struct Elf64_Sym
    {
        Elf64_Word st_name;
        unsigned char st_info;
        unsigned char st_other;
        Elf64_Half st_shndx;
        Elf64_Addr st_value;
        Elf64_Xword st_size;

        [[nodiscard]] bool IsLocalSymbol() const
        {
            return (static_cast<STB>(st_info >> 4) == STB::LOCAL);
        }

        [[nodiscard]] bool IsGlobalSymbol() const
        {
            return (static_cast<STB>(st_info >> 4) == STB::GLOBAL);
        }

        [[nodiscard]] bool IsAbsSymbol() const
        {
            return static_cast<ElfSpecialSectionIndex>(st_shndx) == ElfSpecialSectionIndex::ABSOLUTE;
        }

        [[nodiscard]] bool IsUndefinedSymbol() const
        {
            return static_cast<ElfSpecialSectionIndex>(st_shndx) == ElfSpecialSectionIndex::UNDEFINED;
        }

        [[nodiscard]] bool IsCommonSymbol() const
        {
            return static_cast<ElfSpecialSectionIndex>(st_shndx) == ElfSpecialSectionIndex::COMMON;
        }

        [[nodiscard]] bool NeedExtendSection() const
        {
            return static_cast<ElfSpecialSectionIndex>(st_shndx) == ElfSpecialSectionIndex::XINDEX;
        }
    };

    struct Elf32_Sym
    {
        Elf32_Word st_name;
        Elf32_Addr st_value;
        Elf32_Word st_size;
        unsigned char st_info;
        unsigned char st_other;
        Elf32_Half st_shndx;
    };

    struct Elf32_Phdr
    {
        Elf32_Word	p_type;			/* Segment type */
        Elf32_Off	p_offset;		/* Segment file offset */
        Elf32_Addr	p_vaddr;		/* Segment virtual address */
        Elf32_Addr	p_paddr;		/* Segment physical address */
        Elf32_Word	p_filesz;		/* Segment size in file */
        Elf32_Word	p_memsz;		/* Segment size in memory */
        Elf32_Word	p_flags;		/* Segment flags */
        Elf32_Word	p_align;		/* Segment alignment */
    } ;

    struct Elf64_Phdr
    {
        Elf64_Word	p_type;			/* Segment type */
        Elf64_Word	p_flags;		/* Segment flags */
        Elf64_Off	p_offset;		/* Segment file offset */
        Elf64_Addr	p_vaddr;		/* Segment virtual address */
        Elf64_Addr	p_paddr;		/* Segment physical address */
        Elf64_Xword	p_filesz;		/* Segment size in file */
        Elf64_Xword	p_memsz;		/* Segment size in memory */
        Elf64_Xword	p_align;		/* Segment alignment */
    };


    enum class ElfSectionType : uint32_t
    {
        NULL_ = 0, // 未使用的节
        PROGBITS = 1, // 包含程序定义的信息（代码/数据）
        SYMTAB = 2, // 符号表节
        STRTAB = 3, // 字符串表节
        RELA = 4, // 带有重定位入口的重定位节
        HASH = 5, // 符号哈希表节
        DYNAMIC = 6, // 动态信息节
        NOTE = 7, // 说明信息
        NOBITS = 8, // 不占空间，仅用于对齐的节
        REL = 9, // 带有重定位入口的重定位节
        SHLIB = 10, // 保留
        DYNSYM = 11, // 动态链接符号表节
        INIT_ARRAY = 14, // 初始化函数指针数组节
        FINI_ARRAY = 15, // 结束函数指针数组节
        PREINIT_ARRAY = 16, // 先于 INIT_ARRAY 的初始化函数指针数组节
        GROUP = 17, // 组相关节
        SYMTAB_SHNDX = 18, // 符号表的节索引节
        NUM = 19, // 节数量
        LOOS = 0x60000000, // 专用于该操作系统节类型的开始值
        HIOS = 0x6fffffff, // 专用于该操作系统节类型的结束值
        LOPROC = 0x70000000, // 专用于该处理器体系结构节类型的开始值
        HIPROC = 0x7fffffff, // 专用于该处理器体系结构节类型的结束值
        LOUSER = 0x80000000, // 专用于应用程序节类型的开始值
        HIUSER = 0xffffffff // 专用于应用程序节类型的结束值
    };

    enum class ElfType : uint16_t
    {
        NONE = 0, // No file type
        REL = 1, // Relocatable file
        EXEC = 2, // Executable file
        DYN = 3, // Shared object file
        CORE = 4, // Core file
        LOOS = 0xfe00, // Operating system-specific
        HIOS = 0xfeff, // Operating system-specific
        LOPROC = 0xff00, // Processor-specific
        HIPROC = 0xffff // Processor-specific
    };

    enum class ELFMachine : uint16_t {
        NoSpecific = 0x00,
        ATTWe32100 = 0x01,
        SPARC = 0x02,
        x86 = 0x03,
        Motorola68000 = 0x04,
        Motorola88000 = 0x05,
        IntelMCU = 0x06,
        Intel80860 = 0x07,
        MIPS = 0x08,
        IBMSystem370 = 0x09,
        MIPSRS3000LittleEndian = 0x0A,
        HewlettPackardPARISC = 0x0E,
        Intel80960 = 0x13,
        PowerPC = 0x14,
        PowerPC64Bit = 0x15,
        S390IncludingS390x = 0x16,
        IBMSPUSPC = 0x17,
        NECV800 = 0x24,
        FujitsuFR20 = 0x25,
        TRWRH32 = 0x26,
        MotorolaRCE = 0x27,
        ArmUpToArmv7AArch32 = 0x28,
        DigitalAlpha = 0x29,
        SuperH = 0x2A,
        SPARCVersion9 = 0x2B,
        SiemensTriCoreEmbeddedProcessor = 0x2C,
        ArgonautRISCCore = 0x2D,
        HitachiH8300 = 0x2E,
        HitachiH8300H = 0x2F,
        HitachiH8S = 0x30,
        HitachiH8500 = 0x31,
        IA64 = 0x32,
        StanfordMIPSX = 0x33,
        MotorolaColdFire = 0x34,
        MotorolaM68HC12 = 0x35,
        FujitsuMMAMultimediaAccelerator = 0x36,
        SiemensPCP = 0x37,
        SonynCPUEmbeddedRISCProcessor = 0x38,
        DensoNDR1Microprocessor = 0x39,
        MotorolaStarCoreProcessor = 0x3A,
        ToyotaME16Processor = 0x3B,
        STMicroelectronicsST100Processor = 0x3C,
        AdvancedLogicCorpTinyJEmbeddedProcessorFamily = 0x3D,
        AMDx8664 = 0x3E,
        SonyDSPProcessor = 0x3F,
        DigitalEquipmentCorpPDP10 = 0x40,
        DigitalEquipmentCorpPDP11 = 0x41,
        SiemensFX66Microcontroller = 0x42,
        STMicroelectronicsST9Plus816BitMicrocontroller = 0x43,
        STMicroelectronicsST78BitMicrocontroller = 0x44,
        MotorolaMC68HC16Microcontroller = 0x45,
        MotorolaMC68HC11Microcontroller = 0x46,
        MotorolaMC68HC08Microcontroller = 0x47,
        MotorolaMC68HC05Microcontroller = 0x48,
        SiliconGraphicsSVx = 0x49,
        STMicroelectronicsST198BitMicrocontroller = 0x4A,
        DigitalVAX = 0x4B,
        AxisCommunications32BitEmbeddedProcessor = 0x4C,
        InfineonTechnologies32BitEmbeddedProcessor = 0x4D,
        Element14_64BitDSPProcessor = 0x4E,
        LSILogic16BitDSPProcessor = 0x4F,
        TMS320C6000Family = 0x8C,
        MCSTElbruse2k = 0xAF,
        Arm64BitsArmv8AArch64 = 0xB7,
        ZilogZ80 = 0xDC,
        RISC_V = 0xF3,
        BerkeleyPacketFilter = 0xF7,
    };

    using ELFHeader = Elf64_Ehdr;
    using ELFSectionHeader = Elf64_Shdr;
    using ELFProgramHeader = Elf64_Phdr;

    constexpr auto EHDR64_SIZE = sizeof(Elf64_Ehdr{});
    constexpr auto SHDR64_SIZE = sizeof(Elf64_Shdr{});
    constexpr auto Sym64_SIZE = sizeof(Elf64_Sym{});
    constexpr auto PHDR64_SIZE = sizeof(Elf64_Phdr{});
}

#endif //RVLD_DEFINE_H
