//
// Created by tjp56 on 2023/4/21.
//

#ifndef RVLD_DEFINE_H
#define RVLD_DEFINE_H


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

namespace ELF
{
    using Elf64_Word = uint32_t;
    using Elf64_Half = uint16_t;
    using Elf64_Addr = uint64_t;
    using Elf64_Xword = uint64_t;

    using Elf32_Word = uint32_t;
    using Elf32_Half = uint16_t;
    using Elf32_Addr = uint32_t;

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

    enum class ElfSectionType : uint32_t
    {
        SHT_NULL = 0, // 未使用的节
        SHT_PROGBITS = 1, // 包含程序定义的信息（代码/数据）
        SHT_SYMTAB = 2, // 符号表节
        SHT_STRTAB = 3, // 字符串表节
        SHT_RELA = 4, // 带有重定位入口的重定位节
        SHT_HASH = 5, // 符号哈希表节
        SHT_DYNAMIC = 6, // 动态信息节
        SHT_NOTE = 7, // 说明信息
        SHT_NOBITS = 8, // 不占空间，仅用于对齐的节
        SHT_REL = 9, // 带有重定位入口的重定位节
        SHT_SHLIB = 10, // 保留
        SHT_DYNSYM = 11, // 动态链接符号表节
        SHT_INIT_ARRAY = 14, // 初始化函数指针数组节
        SHT_FINI_ARRAY = 15, // 结束函数指针数组节
        SHT_PREINIT_ARRAY = 16, // 先于 INIT_ARRAY 的初始化函数指针数组节
        SHT_GROUP = 17, // 组相关节
        SHT_SYMTAB_SHNDX = 18, // 符号表的节索引节
        SHT_NUM = 19, // 节数量
        SHT_LOOS = 0x60000000, // 专用于该操作系统节类型的开始值
        SHT_HIOS = 0x6fffffff, // 专用于该操作系统节类型的结束值
        SHT_LOPROC = 0x70000000, // 专用于该处理器体系结构节类型的开始值
        SHT_HIPROC = 0x7fffffff, // 专用于该处理器体系结构节类型的结束值
        SHT_LOUSER = 0x80000000, // 专用于应用程序节类型的开始值
        SHT_HIUSER = 0xffffffff // 专用于应用程序节类型的结束值
    };

    enum class ElfType : uint16_t
    {
        ET_NONE = 0, // No file type
        ET_REL = 1, // Relocatable file
        ET_EXEC = 2, // Executable file
        ET_DYN = 3, // Shared object file
        ET_CORE = 4, // Core file
        ET_LOOS = 0xfe00, // Operating system-specific
        ET_HIOS = 0xfeff, // Operating system-specific
        ET_LOPROC = 0xff00, // Processor-specific
        ET_HIPROC = 0xffff // Processor-specific
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

    constexpr auto EHDR64_SIZE = sizeof(Elf64_Ehdr{});
    constexpr auto SHDR64_SIZE = sizeof(Elf64_Shdr{});
    constexpr auto Sym64_SIZE = sizeof(Elf64_Sym{});
}

#endif //RVLD_DEFINE_H
