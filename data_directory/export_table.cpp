#include <Windows.h>

// RVA: IMAGE_DIRECTORY_ENTRY_EXPORT
// 描述 DLL 导出的函数表信息
struct IMAGE_EXPORT_DIRECTORY {
    DWORD Characteristics;          // 0x00 保留，通常为0
    DWORD TimeDateStamp;            // 0x04 创建时间戳（自1970年起的秒数）
    WORD  MajorVersion;             // 0x08 主版本号
    WORD  MinorVersion;             // 0x0A 次版本号
    DWORD Name;                     // 0x0C RVA 指向 DLL 名称字符串
    DWORD Base;                     // 0x10 导出函数起始序号（通常为1）
    DWORD NumberOfFunctions;        // 0x14 导出函数总数（包含无名字的）
    DWORD NumberOfNames;            // 0x18 有名字的导出函数数量
    DWORD AddressOfFunctions;       // 0x1C RVA 指向导出函数地址表（EAT / Export Address Table）
    DWORD AddressOfNames;           // 0x20 RVA 指向导出函数名称表（Name Pointer Table）
    DWORD AddressOfNameOrdinals;    // 0x24 RVA 指向序号表（Name Ordinal Table）
};

// 导出表中各个数组的嵌套结构

// Export Address Table (EAT) 数组，每个 DWORD 是 RVA
typedef DWORD EXPORT_ADDRESS_RVA;   // 4字节，RVA 或序号/跳转

// Name Pointer Table (NPT) 数组，每个 DWORD 指向函数名字符串的 RVA
typedef DWORD EXPORT_NAME_RVA;      // 4字节，RVA -> ASCII null结尾字符串

// Name Ordinal Table (NOT) 数组，每个 WORD 对应函数在 EAT 中的索引
typedef WORD  EXPORT_ORDINAL;       // 2字节，EAT 索引

/*
结构说明：
1. EAT（AddressOfFunctions）：存放导出函数地址或跳转RVA。
2. Name Pointer Table（AddressOfNames）：存放按名字导出的函数名称 RVA。
3. Name Ordinal Table（AddressOfNameOrdinals）：每个名称对应 EAT 的索引。
4. NumberOfFunctions 表示 EAT 数量，NumberOfNames 表示有名字的导出函数数量。
5. Base + Name Ordinal Table 可以得到实际序号（Ordinal）。
6. 导出表在 PE 文件中位于 DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress。
*/
