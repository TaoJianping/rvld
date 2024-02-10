//
// Created by tjp56 on 2023/6/6.
//

#ifndef RVLD_UTILS_H
#define RVLD_UTILS_H

#include <cmath>
#include <cstdint>

#include "string/strutil.h"
#include "../Define.h"


namespace Utils
{
    static inline auto AlignTo = [](uint64_t size, uint64_t align) -> uint64_t {
        if (align == 0)
            return size;

        return static_cast<uint64_t>(std::ceil(size / align)) * align;
    };

    constexpr std::array<std::string_view, 13> PREFIXES = {
        ".text.",
        ".data.rel.ro.",
        ".data.",
        ".rodata.",
        ".bss.rel.ro.",
        ".bss.",
        ".init_array.",
        ".fini_array.",
        ".tbss.",
        ".tdata.",
        ".gcc_except_table.",
        ".ctors.",
        ".dtors."
    };

    static inline std::string GetOutputName(std::string name, uint64_t flags)
    {
        if (((name == ".rodata") || strutil::starts_with(name, ".rodata.")) && ((flags & SHF_MERGE) != 0))
        {
            if ((flags & SHF_STRINGS) != 0)
            {
                return ".rodata.str";
            }
            else
            {
                return ".rodata.cst";
            }
        }

        for (auto prefix : PREFIXES)
        {
            auto stem = prefix.substr(0, prefix.size() - 1);
            if (name == stem || strutil::starts_with(name, prefix.data()))
            {
                return std::string{ stem };
            }
        }

        return name;
    }

}



#endif //RVLD_UTILS_H
