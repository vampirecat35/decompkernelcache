#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOCFUnserialize.h>

#if defined(__linux__)
#define kCFBundleExecutableKey CFSTR("CFBundleExecutable")
#define kCFBundleIdentifierKey CFSTR("CFBundleIdentifier")
#endif /* Linux Support */

#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS) || defined(_WINDOWS)
#include <Windows.h>
#include <io.h>

#ifdef _MSC_VER
#include <direct.h>

#define mkdir(a,b) _mkdir(a)
#else
#define mkdir(a,b) mkdir(a)
#endif

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#ifndef mode_t
#define mode_t int
#endif
#endif /* Windows Support */

#include "prelink.h"
#include "lzvn.h"

#if defined(__APPLE__) && defined(__MACH__)
#include <libkern/OSByteOrder.h>
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
inline uint32_t OSSwapInt32(uint32_t data)
{
    __asm__ ("bswap   %0" : "+r" (data));

    return data;
}

#ifdef __x86_64__
inline uint64_t OSSwapInt64(uint64_t data)
{
    __asm__ ("bswap   %0" : "+r" (data));

    return data;
}
#else /* Generic */
#define OSSwapInt32(x) \
((((x) & 0xff) << 24) |	\
 (((x) & 0xff00) << 8) |	\
 (((x) & 0xff0000) >> 8) |	\
 (((x) & 0xff000000) >> 24))

#define OSSwapInt64(x) \
    ((uint64_t)((((uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
                (((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
                (((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
                (((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
                (((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
                (((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
                (((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
                (((uint64_t)(x) & 0x00000000000000ffULL) << 56)))
#endif /* Swap 64 */
#elif defined(_MSC_VER) && (defined(__i386__) || defined(_M_IX86) || defined(_X86_)) /* MSVC x86 (32-Bit) */
static __inline uint32_t OSSwapInt32(uint32_t data)
{
    __asm
	{
		push eax
		mov eax, data
		bswap eax
		mov data, eax
		pop eax
	};

    return data;
}

#define OSSwapInt64(x) \
    ((uint64_t)((((uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
                (((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
                (((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
                (((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
                (((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
                (((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
                (((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
                (((uint64_t)(x) & 0x00000000000000ffULL) << 56)))
#else /* Generic */
#define OSSwapInt32(x) \
((((x) & 0xff) << 24) |	\
 (((x) & 0xff00) << 8) |	\
 (((x) & 0xff0000) >> 8) |	\
 (((x) & 0xff000000) >> 24))

#define OSSwapInt64(x) \
    ((uint64_t)((((uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
                (((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
                (((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
                (((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
                (((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
                (((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
                (((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
                (((uint64_t)(x) & 0x00000000000000ffULL) << 56)))
#endif /* Type of swap */

#ifndef FAT_MAGIC
#define FAT_MAGIC	            0xcafebabe
#endif

#ifndef FAT_CIGAM
#define FAT_CIGAM	            0xbebafeca  /* OSSwapInt32(FAT_MAGIC) */
#endif

#ifndef FAT_MAGIC_64
#define FAT_MAGIC_64            0xcafebabf
#endif

#ifndef FAT_CIGAM_64
#define FAT_CIGAM_64            0xbfbafeca  /* OSSwapInt32(FAT_MAGIC_64) */
#endif

/* Constant for the magic field of the mach_header (32-bit architectures) */
#ifndef MH_MAGIC
#define MH_MAGIC                0xfeedface    /* the mach magic number */
#endif

#ifndef MH_CIGAM
#define MH_CIGAM                0xcefaedfe    /* NXSwapInt(MH_MAGIC) */
#endif

/* Constant for the magic field of the mach_header_64 (64-bit architectures) */
#ifndef MH_MAGIC_64
#define MH_MAGIC_64             0xfeedfacf /* the 64-bit mach magic number */
#endif

#ifndef MH_CIGAM_64
#define MH_CIGAM_64             0xcffaedfe /* NXSwapInt(MH_MAGIC_64) */
#endif

#ifndef CPU_ARCH_MASK
#define CPU_ARCH_MASK           0xff000000      /* mask for architecture bits */
#endif

#ifndef CPU_ARCH_ABI64
#define CPU_ARCH_ABI64          0x01000000      /* 64 bit ABI */
#endif

#ifndef CPU_ARCH_ABI64_32
#define CPU_ARCH_ABI64_32       0x02000000      /* ABI for 64-bit hardware with 32-bit types; LP32 */
#endif

#ifndef KMOD_MAX_NAME
#define KMOD_MAX_NAME   (64)
#endif

#ifndef CPU_IS_64BIT
#define CPU_IS_64BIT(CPU) ((CPU) & ((CPU_ARCH_ABI64) | (CPU_ARCH_ABI64_32)))
#endif

#ifndef CPU_IS_32BIT
#define CPU_IS_32BIT(CPU) (CPU_IS_64BIT(CPU) == 0)
#endif

typedef int32_t cpu_type_t;
typedef int32_t cpu_subtype_t;
typedef int vm_prot_t;

#ifndef CPU_TYPE_ANY
#define CPU_TYPE_ANY            ((cpu_type_t) -1)
#endif

#ifndef CPU_TYPE_NONE
#define CPU_TYPE_NONE           ((cpu_type_t) 0)
#endif

#ifndef CPU_TYPE_VAX
#define CPU_TYPE_VAX            ((cpu_type_t) 1)
#endif

#ifndef CPU_TYPE_MC680x0
#define CPU_TYPE_MC680x0        ((cpu_type_t) 6)
#endif

#ifndef CPU_TYPE_I386
#define CPU_TYPE_I386           ((cpu_type_t) 7)
#endif

#ifndef CPU_TYPE_X86_64
#define CPU_TYPE_X86_64         (CPU_TYPE_I386 | CPU_ARCH_ABI64)
#endif

#ifndef CPU_TYPE_MC98000
#define CPU_TYPE_MC98000        ((cpu_type_t) 10)
#endif

#ifndef CPU_TYPE_HPPA
#define CPU_TYPE_HPPA           ((cpu_type_t) 11)
#endif

#ifndef CPU_TYPE_ARM
#define CPU_TYPE_ARM            ((cpu_type_t) 12)
#endif

#ifndef CPU_TYPE_ARM64
#define CPU_TYPE_ARM64          (CPU_TYPE_ARM | CPU_ARCH_ABI64)
#endif

#ifndef CPU_TYPE_ARM64_32
#define CPU_TYPE_ARM64_32       (CPU_TYPE_ARM | CPU_ARCH_ABI64_32)
#endif

#ifndef CPU_TYPE_MC88000
#define CPU_TYPE_MC88000        ((cpu_type_t) 13)
#endif

#ifndef CPU_TYPE_SPARC
#define CPU_TYPE_SPARC          ((cpu_type_t) 14)
#endif

#ifndef CPU_TYPE_I860
#define CPU_TYPE_I860           ((cpu_type_t) 15)
#endif

#ifndef CPU_TYPE_ALPHA
#define CPU_TYPE_ALPHA          ((cpu_type_t) 16)
#endif

#ifndef CPU_TYPE_POWERPC
#define CPU_TYPE_POWERPC        ((cpu_type_t) 18)
#endif

#ifndef CPU_TYPE_POWERPC64
#define CPU_TYPE_POWERPC64      (CPU_TYPE_POWERPC | CPU_ARCH_ABI64)
#endif

#ifndef _MSC_VER
#define PACK_GNU __attribute__((aligned(1)))
#else /* GNUC/CLANG */
#define PACK_GNU
#endif /* PACK_DEF */

#ifndef LC_SEGMENT
#define LC_SEGMENT          0x1             /* segment of this file to be mapped */
#endif

#ifndef LC_SEGMENT_64
#define LC_SEGMENT_64       0x19            /* 64-bit segment of this file to be mapped */
#endif

#ifndef LC_CODE_SIGNATURE
#define LC_CODE_SIGNATURE   0x1d            /* local of code signature */
#endif

#ifndef SEG_LINKEDIT
#define SEG_LINKEDIT        "__LINKEDIT"    /* the segment containing all structs */
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct fat_header {
	uint32_t	magic;		/* FAT_MAGIC */
	uint32_t	nfat_arch;	/* number of structs that follow */
} fat_header_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct fat_arch {
	cpu_type_t	cputype;	/* cpu specifier (int) */
	cpu_subtype_t	cpusubtype;	/* machine specifier (int) */
	uint32_t	offset;		/* file offset to this object file */
	uint32_t	size;		/* size of this object file */
	uint32_t	align;		/* alignment as a power of 2 */
} fat_arch_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct fat_arch_64 {
    cpu_type_t    cputype;      /* cpu specifier (int) */
    cpu_subtype_t cpusubtype;   /* machine specifier (int) */
    uint64_t      offset;       /* file offset to this object file */
    uint64_t      size;         /* size of this object file */
    uint32_t      align;        /* alignment as a power of 2 */
    uint32_t      reserved;     /* reserved */
} fat_arch_64_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifndef PLATFORM_NAME_LEN
#define PLATFORM_NAME_LEN  (64)
#endif

#ifndef ROOT_PATH_LEN
#define ROOT_PATH_LEN     (256)
#endif

#ifndef RESERVED_DWORDS
#define RESERVED_DWORDS    (10)
#endif

/*
 * prelinkVersion ((value) >= 1) => KASLR supported.
 * prelinked kernel header size is 384 bytes.
 */
#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct prelinked_kernel_header {
    /* Compression signatures */
    uint32_t  signature;                        /*   0 -   3 */
    uint32_t  compressType;                     /*   4 -   7 */

    /* Checksum */
    uint32_t  adler32;                          /*   8 -  11 */

    /* Sizes */
    uint32_t  uncompressedSize;                 /*  12 -  15 */
    uint32_t  compressedSize;                   /*  16 -  19 */

    /* Version */
    uint32_t  prelinkVersion;                   /*  20 -  23 */
    
    /* Reserved */
    uint32_t  reserved[RESERVED_DWORDS];        /*  24 -  63 */

    /* Platform name */
    char      platformName[PLATFORM_NAME_LEN];  /*  64 - 127 => unused */
    
    /* Root path */
    char      rootPath[ROOT_PATH_LEN];          /* 128 - 383 => unused */
} PrelinkedKernelHeader PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct platform_info
{
    /* Platform name */
    char platformName[PLATFORM_NAME_LEN];
    
    /* Root path */
    char rootPath[ROOT_PATH_LEN];
} PlatformInfo PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct mach_header {
    uint32_t        magic;          /* mach magic number identifier */
    cpu_type_t      cputype;        /* cpu specifier */
    cpu_subtype_t   cpusubtype;     /* machine specifier */
    uint32_t        filetype;       /* type of file */
    uint32_t        ncmds;          /* number of load commands */
    uint32_t        sizeofcmds;     /* the size of all the load commands */
    uint32_t        flags;          /* flags */
} mach_header_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct mach_header_64 {
    uint32_t        magic;          /* mach magic number identifier */
    cpu_type_t      cputype;        /* cpu specifier */
    cpu_subtype_t   cpusubtype;     /* machine specifier */
    uint32_t        filetype;       /* type of file */
    uint32_t        ncmds;          /* number of load commands */
    uint32_t        sizeofcmds;     /* the size of all the load commands */
    uint32_t        flags;          /* flags */
    uint32_t        reserved;       /* reserved */
} mach_header_64_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct load_command {
    uint32_t cmd;        /* type of load command */
    uint32_t cmdsize;    /* total size of command in bytes */
} load_command_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct segment_command { /* for 32-bit architectures */
    uint32_t    cmd;            /* LC_SEGMENT */
    uint32_t    cmdsize;        /* includes sizeof section structs */
    char        segname[16];    /* segment name */
    uint32_t    vmaddr;         /* memory address of this segment */
    uint32_t    vmsize;         /* memory size of this segment */
    uint32_t    fileoff;        /* file offset of this segment */
    uint32_t    filesize;       /* amount to map from the file */
    vm_prot_t   maxprot;        /* maximum VM protection */
    vm_prot_t   initprot;       /* initial VM protection */
    uint32_t    nsects;         /* number of sections in segment */
    uint32_t    flags;          /* flags */
} segment_command_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct segment_command_64 { /* for 64-bit architectures */
    uint32_t    cmd;            /* LC_SEGMENT_64 */
    uint32_t    cmdsize;        /* includes sizeof section_64 structs */
    char        segname[16];    /* segment name */
    uint64_t    vmaddr;         /* memory address of this segment */
    uint64_t    vmsize;         /* memory size of this segment */
    uint64_t    fileoff;        /* file offset of this segment */
    uint64_t    filesize;       /* amount to map from the file */
    vm_prot_t   maxprot;        /* maximum VM protection */
    vm_prot_t   initprot;       /* initial VM protection */
    uint32_t    nsects;         /* number of sections in segment */
    uint32_t    flags;          /* flags */
} segment_command_64_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct section { /* for 32-bit architectures */
    char        sectname[16];   /* name of this section */
    char        segname[16];    /* segment this section goes in */
    uint32_t    addr;           /* memory address of this section */
    uint32_t    size;           /* size in bytes of this section */
    uint32_t    offset;         /* file offset of this section */
    uint32_t    align;          /* section alignment (power of 2) */
    uint32_t    reloff;         /* file offset of relocation entries */
    uint32_t    nreloc;         /* number of relocation entries */
    uint32_t    flags;          /* flags (section type and attributes)*/
    uint32_t    reserved1;      /* reserved (for offset or index) */
    uint32_t    reserved2;      /* reserved (for count or sizeof) */
} section_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct section_64 { /* for 64-bit architectures */
    char        sectname[16];   /* name of this section */
    char        segname[16];    /* segment this section goes in */
    uint64_t    addr;           /* memory address of this section */
    uint64_t    size;           /* size in bytes of this section */
    uint32_t    offset;         /* file offset of this section */
    uint32_t    align;          /* section alignment (power of 2) */
    uint32_t    reloff;         /* file offset of relocation entries */
    uint32_t    nreloc;         /* number of relocation entries */
    uint32_t    flags;          /* flags (section type and attributes)*/
    uint32_t    reserved1;      /* reserved (for offset or index) */
    uint32_t    reserved2;      /* reserved (for count or sizeof) */
    uint32_t    reserved3;      /* reserved */
} section_64_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct linkedit_data_command {
    uint32_t    cmd;        /* LC_CODE_SIGNATURE, LC_SEGMENT_SPLIT_INFO,
                               LC_FUNCTION_STARTS, LC_DATA_IN_CODE,
                               LC_DYLIB_CODE_SIGN_DRS,
                               LC_LINKER_OPTIMIZATION_HINT,
                               LC_DYLD_EXPORTS_TRIE, or
                               LC_DYLD_CHAINED_FIXUPS. */
    uint32_t    cmdsize;    /* sizeof(struct linkedit_data_command) */
    uint32_t    dataoff;    /* file offset of data in __LINKEDIT segment */
    uint32_t    datasize;   /* file size of data in __LINKEDIT segment  */
} linkedit_data_command_t PACK_GNU;
#ifdef _MSC_VER
#pragma pack()
#endif

/* Convert CPU name string to CPU type value */
cpu_type_t cpu_type_for_name(const char *archstr)
{
    if (strncmp("i860", archstr, 4) == 0)
    {
        return CPU_TYPE_I860;
    }

    if (strncmp("i386", archstr, 4) == 0)
    {
        return CPU_TYPE_I386;
    }

    if (strncmp("x86_64", archstr, 5) == 0)
    {
        return CPU_TYPE_X86_64;
    }

    if (strncmp("ppc64", archstr, 5) == 0)
    {
        return CPU_TYPE_POWERPC64;
    }

    if (strncmp("ppc", archstr, 5) == 0)
    {
        return CPU_TYPE_POWERPC;
    }
    
    if (strncmp("arm64_32", archstr, 8) == 0)
    {
        return CPU_TYPE_ARM64_32;
    }

    if (strncmp("arm64", archstr, 5) == 0)
    {
        return CPU_TYPE_ARM64;
    }

    if (strncmp("arm", archstr, 3) == 0)
    {
        return CPU_TYPE_ARM;
    }

    if (strncmp("vax", archstr, 3) == 0)
    {
        return CPU_TYPE_VAX;
    }

    if (strncmp("mc68k", archstr, 5) == 0)
    {
        return CPU_TYPE_MC680x0;
    }

    if (strncmp("mc88k", archstr, 5) == 0)
    {
        return CPU_TYPE_MC88000;
    }

    if (strncmp("mc98k", archstr, 5) == 0)
    {
        return CPU_TYPE_MC98000;
    }

    if (strncmp("sparc", archstr, 5) == 0)
    {
        return CPU_TYPE_SPARC;
    }

    if (strncmp("alpha", archstr, 5) == 0)
    {
        return CPU_TYPE_ALPHA;
    }

    printf("WARNING: Invalid architecture name %s found, using any architecture\n", archstr);
    return CPU_TYPE_ANY;
}

/* Convert CPU name string to CPU type value */
const char *cpu_name_for_type(cpu_type_t cputype)
{
    switch (cputype)
    {
        case CPU_TYPE_I860:
            return "i860";

        case CPU_TYPE_I386:
            return "i386";

        case CPU_TYPE_X86_64:
            return "x86_64";

        case CPU_TYPE_POWERPC64:
            return "ppc64";

        case CPU_TYPE_POWERPC:
            return "ppc";

        case CPU_TYPE_ARM64_32:
            return "arm64_32";

        case CPU_TYPE_ARM64:
            return "arm64";

        case CPU_TYPE_ARM:
            return "arm";

        case CPU_TYPE_VAX:
            return "vax";

        case CPU_TYPE_MC680x0:
            return "m68k";

        case CPU_TYPE_MC88000:
            return "m88k";

        case CPU_TYPE_MC98000:
            return "m98k";

        case CPU_TYPE_SPARC:
            return "sparc";

        case CPU_TYPE_ALPHA:
            return "alpha";

        case CPU_TYPE_ANY:
        case CPU_TYPE_NONE:
        default:
            break;
    }

    printf("WARNING: Invalid cpu type 0x%x found, using any architecture\n", cputype);
    return "any";
}

struct load_command *find_load_command(struct mach_header *machHeader, uint32_t targetCmd)
{
    struct load_command *loadCommand;
    size_t machHeaderSize = 0;
    int is32bit = (int)((machHeader->magic == MH_MAGIC) || (machHeader->magic == MH_CIGAM));
    int swapped = ((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));

    // First LOAD_COMMAND begins after the mach header.
    if (is32bit == true)
    {
        machHeaderSize = sizeof(struct mach_header);
    } else {
        machHeaderSize = sizeof(struct mach_header_64);
    }

    loadCommand = (struct load_command *)((uint64_t)machHeader + machHeaderSize);

    while ((uintptr_t)loadCommand < (((uintptr_t)machHeader + ((swapped == 0) ? ((uintptr_t)machHeader->sizeofcmds) :
        ((uintptr_t)OSSwapInt32((uint32_t)machHeader->sizeofcmds)))) + machHeaderSize))
    {
        if (((swapped == 0) ? (loadCommand->cmd) : (OSSwapInt32(loadCommand->cmd))) == targetCmd)
        {
            return ((struct load_command *)loadCommand);
        }

        // Next load command.
        loadCommand = (struct load_command *)((uintptr_t)loadCommand + ((swapped == 0) ?
                                              (uintptr_t)loadCommand->cmdsize :
                        (uintptr_t)OSSwapInt32((uint32_t)loadCommand->cmdsize)));
    }

    // Return NULL on failure (not found).
    return NULL;
}

void *find_segment(struct mach_header *aMachHeader, const char *aSegmentName, int *isSeg32)
{
    struct load_command *loadCommand;
    struct segment_command *segment32;
    struct segment_command_64 *segment64;
    size_t machHeaderSize = 0;
    int is32bit = ((aMachHeader->magic == MH_MAGIC) || (aMachHeader->magic == MH_CIGAM));
    int swapped = ((aMachHeader->magic == MH_CIGAM) || (aMachHeader->magic == MH_CIGAM_64));

    // First LOAD_COMMAND begins straight after the mach header.
    if (is32bit == 0)
    {
        machHeaderSize = sizeof(struct mach_header_64);
    } else {
        machHeaderSize = sizeof(struct mach_header);
    }
    
    loadCommand = (struct load_command *)((uintptr_t)aMachHeader + machHeaderSize);
    
    while ((uintptr_t)loadCommand < (uintptr_t)aMachHeader + ((swapped == 0) ? (uintptr_t)aMachHeader->sizeofcmds : (uintptr_t)OSSwapInt32((uint32_t)aMachHeader->sizeofcmds)) + machHeaderSize)
    {
        if (((swapped == 0) ? loadCommand->cmd : OSSwapInt32(loadCommand->cmd)) == LC_SEGMENT)
        {
            // Check load command's segment name in case of a 32-bit segment.
            segment32 = (struct segment_command *)loadCommand;

            if (strncmp(segment32->segname, aSegmentName, sizeof(segment32->segname)) == 0)
            {
                if (isSeg32 != NULL)
                {
                    *isSeg32 = 1;
                }

                return ((void *)segment32);
            }
        }  else  if (((swapped == 0) ? loadCommand->cmd : OSSwapInt32(loadCommand->cmd)) == LC_SEGMENT_64) {
            // Check load command's segment name in case of a 64_bit segment.
            segment64 = (struct segment_command_64 *)loadCommand;

            if (strncmp(segment64->segname, aSegmentName, sizeof(segment64->segname)) == 0)
            {
                if (isSeg32 != NULL)
                {
                    *isSeg32 = 0;
                }

                return ((void *)segment64);
            }
        }
        
        // Next load command.
        loadCommand = (struct load_command *)((uintptr_t)loadCommand + ((swapped == 0) ? (uintptr_t)loadCommand->cmdsize : (uintptr_t)OSSwapInt32((uint32_t)loadCommand->cmdsize)));
    }
    
    // Return NULL on failure (32/64 bit segment not found).
    return NULL;
}

uint8_t is_prelinkedkernel(unsigned char *aFileBuffer)
{
    struct segment_command_64 *prelinkTextSegment64 = NULL;
    struct segment_command_64 *prelinkInfoSegment64 = NULL;
    struct segment_command *prelinkTextSegment32 = NULL;
    struct segment_command *prelinkInfoSegment32 = NULL;
    int is32bit = 0;

    struct mach_header *machHeader = (struct mach_header *)((unsigned char *)aFileBuffer);

    if ((machHeader->magic != MH_MAGIC) && (machHeader->magic != MH_MAGIC_64) && (machHeader->magic != MH_CIGAM) && (machHeader->magic != MH_CIGAM_64))
    {
        return 0;
    }

    prelinkTextSegment64 = find_segment(machHeader, "__PRELINK_TEXT", &is32bit);
    prelinkInfoSegment64 = find_segment(machHeader, "__PRELINK_INFO", &is32bit);
    prelinkTextSegment32 = (struct segment_command *)prelinkTextSegment64;
    prelinkInfoSegment32 = (struct segment_command *)prelinkInfoSegment64;

    if (((prelinkTextSegment32) && (prelinkInfoSegment32->filesize)) &&
        ((prelinkTextSegment32) && (prelinkTextSegment32->filesize)) &&
        (is32bit != 0))
    {
        return 1;
    } else if (((prelinkTextSegment64) && (prelinkInfoSegment64->filesize)) &&
               ((prelinkTextSegment64) && (prelinkTextSegment64->filesize)) &&
               (is32bit == 0)) {
        return 1;
    }

    return 0;
}

/* List prelinked kexts */
uint8_t listKexts(unsigned char *aFileBuffer, const char *outfile)
{
    struct segment_command_64 *prelinkTextSegment64 = NULL;
    struct segment_command_64 *prelinkInfoSegment64 = NULL;
    struct segment_command *prelinkTextSegment32 = NULL;
    struct segment_command *prelinkInfoSegment32 = NULL;
    struct mach_header *machHeader = (struct mach_header *)((unsigned char *)aFileBuffer);
    //struct linkedit_data_command *codeSignature = NULL;
    int is32bit = 0;
    int swapped = ((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));
    FILE *out = NULL;

    prelinkTextSegment32 = find_segment(machHeader, "__PRELINK_TEXT", &is32bit);
    prelinkInfoSegment32 = find_segment(machHeader, "__PRELINK_INFO", &is32bit);
    prelinkTextSegment64 = (struct segment_command_64 *)prelinkTextSegment32;
    prelinkInfoSegment64 = (struct segment_command_64 *)prelinkInfoSegment32;

    if (((prelinkTextSegment32) && (prelinkInfoSegment32) && (is32bit != 0)) ||
        ((prelinkTextSegment64) && (prelinkInfoSegment64) && (is32bit == 0)))
    {
        const char *prelinkInfoBytes = (const char *)aFileBuffer + ((is32bit == 0) ? ((swapped == 0) ? prelinkInfoSegment64->fileoff : OSSwapInt64(prelinkInfoSegment64->fileoff)) : ((swapped == 0) ? prelinkInfoSegment32->fileoff : OSSwapInt32(prelinkInfoSegment32->fileoff)));

        CFPropertyListRef prelinkInfoPlist = IOCFUnserialize(prelinkInfoBytes, kCFAllocatorDefault, 0, NULL);

        if (outfile != NULL)
        {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            fopen_s(&out, outfile, "wb");
#else
            out = fopen(outfile, "wb");
#endif
        }

        if (prelinkInfoPlist)
        {
            printf("NOTICE: Unserialized prelink info\n");
            if (out != NULL)
            {
                fprintf(out, "NOTICE: Unserialized prelink info\n");
            }
    
            CFArrayRef kextPlistArray = NULL;
            kextPlistArray = (CFArrayRef)CFDictionaryGetValue(prelinkInfoPlist, CFSTR("_PrelinkInfoDictionary"));
            CFIndex i = 0;
            CFIndex kextCount = CFArrayGetCount(kextPlistArray);
            printf("kextCount: %ld\n", (long)kextCount);
            if (out != NULL)
            {
                fprintf(out, "kextCount: %ld\n", (long)kextCount);
            }

            //char kextIdentifierBuffer[64];    // KMOD_MAX_NAME = 64
            char kextBundlePathBuffer[PATH_MAX];
            //char kextPath[PATH_MAX];
            //char kextPlistPath[PATH_MAX];
            //char kextExecutablePath[PATH_MAX];
            
            //struct stat st = {0};

            for (i = 0; i < kextCount; i++)
            {
                CFDictionaryRef kextPlist = (CFDictionaryRef)CFArrayGetValueAtIndex(kextPlistArray, i);
                CFStringRef bundlePath = (CFStringRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkBundlePathKey));
                
                if (bundlePath)
                {
                    CFStringGetCString(bundlePath, kextBundlePathBuffer, sizeof(kextBundlePathBuffer), kCFStringEncodingUTF8);
                    printf("%s\n", kextBundlePathBuffer);
                    if (out != NULL)
                    {
                        fprintf(out, "%s\n", kextBundlePathBuffer);
                    }
                }
            }

            if (out != NULL)
            {
                fclose(out);
            }
        } else {
            printf("ERROR: Can't unserialize _PrelinkInfoDictionary!\n");
            
            return -1;
        }
    } else {
        printf("ERROR: find_segment(\"__PRELINK_TEXT/__PRELINK_INFO\") failed!\n");

        return -1;
    }

    return 0;
}

uint8_t saveKernel(unsigned char *aFileBuffer, const char *outfile)
{
    struct segment_command_64 *lastSegment64        = NULL;
    struct segment_command_64 *prelinkTextSegment64 = NULL;
    struct segment_command_64 *prelinkInfoSegment64 = NULL;
    struct segment_command_64 *linkeditSegment64    = NULL;
    struct segment_command *lastSegment32           = NULL;
    struct segment_command *prelinkTextSegment32    = NULL;
    struct segment_command *prelinkInfoSegment32    = NULL;
    struct segment_command *linkeditSegment32       = NULL;
    
    struct section_64 *prelinkTextSection64         = NULL;
    struct section_64 *prelinkInfoSection64         = NULL;
    struct section *prelinkTextSection32            = NULL;
    struct section *prelinkInfoSection32            = NULL;
    
    struct mach_header *machHeader = (struct mach_header *)aFileBuffer;

    int is32bitMachO = ((machHeader->magic == MH_MAGIC) || (machHeader->magic == MH_CIGAM));
    int is32bit = 0;

    int swapped = (int)((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));

    if ((lastSegment64 = find_segment(machHeader, "__LAST", &is32bit)) == NULL)
    {
        printf("ERROR: find_segment(\"__LAST\") failed!\n");
        return -1;
    }

    if ((prelinkTextSegment64 = find_segment(machHeader, "__PRELINK_TEXT", &is32bit)) == NULL)
    {
        printf("ERROR: find_segment(\"__PRELINK_TEXT\") failed!\n");
        return -1;
    }

    if ((prelinkInfoSegment64 = find_segment(machHeader, "__PRELINK_INFO", &is32bit)) == NULL)
    {
        printf("ERROR: find_segment(\"__PRELINK_INFO\") failed!\n");
        return -1;
    }
    
    if ((linkeditSegment64 = find_segment(machHeader, SEG_LINKEDIT, &is32bit)) == NULL)
    {
        printf("ERROR: find_segment(\"__LINKEDIT\") failed!\n");
        return -1;
    }

    // 32 Bits
    lastSegment32 = (struct segment_command *)lastSegment64;
    prelinkTextSegment32 = (struct segment_command *)prelinkTextSegment64;
    prelinkInfoSegment32 = (struct segment_command *)prelinkInfoSegment64;
    linkeditSegment32 = (struct segment_command *)linkeditSegment64; 

    if ((is32bit == 0) && (is32bitMachO == 0))
    {
        prelinkTextSegment64->vmaddr = linkeditSegment64->vmaddr;
        prelinkTextSegment64->vmsize = 0;
        prelinkTextSegment64->fileoff = (swapped == 0) ? (lastSegment64->fileoff + lastSegment64->filesize) : (OSSwapInt64(OSSwapInt64(lastSegment64->fileoff) + OSSwapInt64(lastSegment64->filesize)));
        prelinkTextSegment64->filesize = 0;
        
        prelinkTextSection64 = (struct section_64 *)((uintptr_t)prelinkTextSegment64 + sizeof(struct segment_command_64));
        
        prelinkTextSection64->addr = prelinkTextSegment64->vmaddr;
        prelinkTextSection64->size = 0;
        prelinkTextSection64->offset = (uint32_t)prelinkTextSegment64->fileoff;

        prelinkInfoSegment64->vmaddr = linkeditSegment64->vmaddr;
        prelinkInfoSegment64->vmsize = 0;
        prelinkInfoSegment64->fileoff = (swapped == 0) ? (lastSegment64->fileoff + lastSegment64->filesize) : (OSSwapInt64(OSSwapInt64((lastSegment64->fileoff)) + OSSwapInt64(lastSegment64->filesize)));
        prelinkInfoSegment64->filesize = 0;

        prelinkInfoSection64 = (struct section_64 *)((uintptr_t)prelinkInfoSegment64 + sizeof(struct segment_command_64));

        prelinkInfoSection64->addr = prelinkTextSegment64->vmaddr;
        prelinkInfoSection64->size = 0;
        prelinkInfoSection64->offset = (uint32_t)prelinkInfoSegment64->fileoff;
        FILE *fp = NULL;

        if (outfile == NULL)
        {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            fopen_s(&fp, outfile, "wb");
#else
            fp = fopen("kernel", "wb");
#endif
        } else {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            fopen_s(&fp, outfile, "wb");
#else
            fp = fopen(outfile, "wb");
#endif
        }

        fwrite(aFileBuffer, 1, (long)(((swapped == 0) ? linkeditSegment64->fileoff : OSSwapInt64(linkeditSegment64->fileoff)) + ((swapped == 0) ? linkeditSegment64->filesize : OSSwapInt64(linkeditSegment64->filesize))), fp);
        printf("%ld bytes written\n", ftell(fp));
        fclose(fp);
    } else if ((is32bit == 1) || (is32bitMachO == 1)) {
        prelinkTextSegment32->vmaddr = linkeditSegment32->vmaddr;
        prelinkTextSegment32->vmsize = 0;
        prelinkTextSegment32->fileoff = (swapped == 0) ? (lastSegment32->fileoff + lastSegment32->filesize) : (OSSwapInt32(OSSwapInt32(lastSegment32->fileoff) + OSSwapInt32(lastSegment32->filesize)));
        prelinkTextSegment32->filesize = 0;
        
        prelinkTextSection32 = (struct section *)((uintptr_t)prelinkTextSegment32 + sizeof(struct segment_command));

        prelinkTextSection32->addr = prelinkTextSegment32->vmaddr;
        prelinkTextSection32->size = 0;
        prelinkTextSection32->offset = prelinkTextSegment32->fileoff;
        
        prelinkInfoSegment32->vmaddr = linkeditSegment32->vmaddr;
        prelinkInfoSegment32->vmsize = 0;
        prelinkInfoSegment32->fileoff = (swapped == 0) ? (lastSegment32->fileoff + lastSegment32->filesize) : (OSSwapInt32(OSSwapInt32(lastSegment32->fileoff) + OSSwapInt32(lastSegment32->filesize)));
        prelinkInfoSegment32->filesize = 0;
        
        prelinkInfoSection32 = (struct section *)((uintptr_t)prelinkInfoSegment32 + sizeof(struct segment_command));
        
        prelinkInfoSection32->addr = prelinkTextSegment32->vmaddr;
        prelinkInfoSection32->size = 0;
        prelinkInfoSection32->offset = prelinkInfoSegment32->fileoff;
        
        FILE *fp = NULL;
        
        if (outfile == NULL)
        {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            fopen_s(&fp, "kernel", "wb");
#else
            fp = fopen("kernel", "wb");
#endif
        } else {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            fopen_s(&fp, outfile, "wb");
#else
            fp = fopen(outfile, "wb");
#endif
        }

        fwrite(aFileBuffer, 1, (size_t)(((swapped == 0) ? linkeditSegment32->fileoff : (size_t)OSSwapInt32(linkeditSegment32->fileoff)) + ((size_t)(swapped == 0) ? linkeditSegment32->filesize : OSSwapInt32(linkeditSegment32->filesize))), fp);
        printf("%ld bytes written\n", ftell(fp));
        fclose(fp);
    } else {
	printf("ERROR: Get kernel segments failed\n");

	return -1;
    }

    return 0;
}

int d_mkdir(char *aDirectory, mode_t aMode)
{
    char * p = aDirectory;
    struct stat sb;

    // Skip leading slashes.
    while (*p == '/')
    {
        p++;
    }

    while ((p = strchr(p, '/')))
    {
        *p = '\0';
        
        if (stat(aDirectory, &sb) != 0)
        {
            if (mkdir(aDirectory, aMode))
            {
                printf("Error: cannot create directory: %s\n", aDirectory);

                return 1;
            }
        }
        
        // Restore slash.
        *p++ = '/';
        
        while (*p == '/')
        {
            p++;
        }
    }

    // Create the final directory component.
    if (stat(aDirectory, &sb) && mkdir(aDirectory, aMode))
    {
        printf("Error: cannot create directory: %s", aDirectory);

        return 1;
    }
    
    return 0;
}

uint8_t saveDictionary(unsigned char * aFileBuffer, const char *outfile)
{
    struct segment_command_64 *prelinkInfoSegment64 = NULL;
    struct segment_command *prelinkInfoSegment32 = NULL;
    struct mach_header *machHeader = (struct mach_header *)((unsigned char *)aFileBuffer);
    CFPropertyListRef prelinkInfoPlist;
    int is32bit = 0;
    int swapped = (int)((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));
    const char * prelinkInfoBytes = NULL;

    if ((prelinkInfoSegment64 = find_segment(machHeader, "__PRELINK_INFO", &is32bit)) == NULL)
    {
        printf("ERROR: find_segment(\"__PRELINK_INFO\") failed!\n");
        return -1;
    }

    prelinkInfoSegment32 = (struct segment_command *)prelinkInfoSegment64;

    if (is32bit == 0)
    {
        prelinkInfoBytes = (const char *)aFileBuffer + ((swapped == 0) ? prelinkInfoSegment64->fileoff : OSSwapInt64(prelinkInfoSegment64->fileoff));

        prelinkInfoPlist = (CFPropertyListRef)IOCFUnserialize(prelinkInfoBytes, kCFAllocatorDefault, /* options */ 0, /* errorString */ NULL);
    } else {
        prelinkInfoBytes = (const char *)aFileBuffer + ((swapped == 0) ? prelinkInfoSegment32->fileoff : OSSwapInt32(prelinkInfoSegment32->fileoff));

        prelinkInfoPlist = (CFPropertyListRef)IOCFUnserialize(prelinkInfoBytes, kCFAllocatorDefault, /* options */ 0, /* errorString */ NULL);
    }

    if (prelinkInfoPlist)
    {
        printf("NOTICE: Unserialized _PrelinkInfoDictionary\n");

        CFErrorRef xmlError = NULL;
        CFDataRef xmlData = CFPropertyListCreateData(kCFAllocatorDefault, prelinkInfoPlist, kCFPropertyListXMLFormat_v1_0, 0, &xmlError);

        if (xmlError == NULL)
        {
            const unsigned char *buffer = CFDataGetBytePtr(xmlData);
            long xmlLength = (long)CFDataGetLength(xmlData);
            FILE *fp = NULL;

            if (outfile == NULL)
            {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                fopen_s(&fp, "Dictionary.plist", "wb");
#else
                fp = fopen("Dictionary.plist", "wb");
#endif
            } else {
#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                fopen_s(&fp, outfile, "wb");
#else
                fp = fopen(outfile, "wb");
#endif
            }

            fwrite(buffer, 1, xmlLength, fp);
            printf("%ld bytes written\n", ftell(fp));

            fclose(fp);
        }
    }
    
    return 0;
}

uint8_t saveKexts(unsigned char *aFileBuffer, const char *dir)
{
    int signedKexts = 0;

    struct segment_command_64 *prelinkTextSegment64    = NULL;
    struct segment_command_64 *prelinkInfoSegment64    = NULL;
    struct segment_command *prelinkTextSegment32    = NULL;
    struct segment_command *prelinkInfoSegment32    = NULL;
    struct linkedit_data_command *codeSignature = NULL;
    struct mach_header *machHeader = ((struct mach_header *)aFileBuffer);
    int is32bit = 0;
    int swapped = ((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));
    const char *outdir = dir;

    prelinkTextSegment64 = find_segment(machHeader, "__PRELINK_TEXT", &is32bit);
    prelinkInfoSegment64 = find_segment(machHeader, "__PRELINK_INFO", &is32bit);
    prelinkTextSegment32 = (struct segment_command *)prelinkTextSegment64;
    prelinkInfoSegment32 = (struct segment_command *)prelinkInfoSegment64;

    if ((prelinkTextSegment64) && (prelinkInfoSegment64) && (is32bit == 0))
    {
        printf("prelinkInfoSegment64->vmaddr..: 0x%llx\n", ((swapped == 0) ? (unsigned long long)prelinkInfoSegment64->vmaddr : (unsigned long long)OSSwapInt64(prelinkInfoSegment64->vmaddr))) ;
        printf("prelinkInfoSegment64->fileoff.: 0x%llx\n", ((swapped == 0) ? (unsigned long long)prelinkInfoSegment64->fileoff : (unsigned long long)OSSwapInt64(prelinkInfoSegment64->fileoff)));
        printf("prelinkInfoSegment64->filesize: 0x%llx\n", ((swapped == 0) ? (unsigned long long)prelinkInfoSegment64->filesize : (unsigned long long)OSSwapInt64(prelinkInfoSegment64->filesize)));

        const char *prelinkInfoBytes = (const char *)aFileBuffer + ((swapped == 0) ? prelinkInfoSegment64->fileoff : OSSwapInt64(prelinkInfoSegment64->fileoff));
            
            CFPropertyListRef prelinkInfoPlist = (CFPropertyListRef)IOCFUnserialize(prelinkInfoBytes,
                                                   kCFAllocatorDefault, 0, NULL);
            
            if (prelinkInfoPlist)
            {
                printf("NOTICE: Unserialized prelink info\n");
                
                CFArrayRef kextPlistArray = NULL;
                kextPlistArray = (CFArrayRef)CFDictionaryGetValue(prelinkInfoPlist, CFSTR("_PrelinkInfoDictionary"));
                CFIndex i = 0;
                CFIndex kextCount = CFArrayGetCount(kextPlistArray);
                printf("kextCount: %ld\n", (long)kextCount);
                
                char kextIdentifierBuffer[KMOD_MAX_NAME];    // KMOD_MAX_NAME = 64
                char kextBundlePathBuffer[PATH_MAX];
                char kextPath[PATH_MAX];
                char kextPlistPath[PATH_MAX];
                char kextExecutablePath[PATH_MAX];
                
                struct stat st = {0};

		if (outdir == NULL)
		{
		    outdir = "kexts";
		}

                if ((kextCount) && (stat(outdir, &st) == -1))
                {
                    mkdir(outdir, 0755);
                }

                for (i = 0; i < kextCount; i++)
                {
                    // printf("kextPlist: 0x%llx\n", (int64_t)kextPlist - (int64_t)fileBuffer - prelinkInfoSegment->fileoff);
                    CFDictionaryRef kextPlist = (CFDictionaryRef)CFArrayGetValueAtIndex(kextPlistArray, i);
                    CFStringRef kextIdentifier = (CFStringRef)CFDictionaryGetValue(kextPlist, kCFBundleIdentifierKey);

                    if (kextIdentifier)
                    {
                        CFStringGetCString(kextIdentifier, kextIdentifierBuffer, sizeof(kextIdentifierBuffer), kCFStringEncodingUTF8);
                        printf("\nCFBundleIdentifier[%3ld].......: %s\n", (long)i, kextIdentifierBuffer);
                    }

                    CFStringRef bundlePath = (CFStringRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkBundlePathKey));
                    
                    if (bundlePath)
                    {
                        CFStringGetCString(bundlePath, kextBundlePathBuffer, sizeof(kextBundlePathBuffer), kCFStringEncodingUTF8);
                        printf("_PrelinkBundlePath............: %s\n", kextBundlePathBuffer);
 
                        snprintf(kextPath, sizeof(kextPath), "%s%s", outdir, kextBundlePathBuffer);
                        printf("kextPath......................: %s\n", kextPath);

                        if (stat(kextPath, &st) == -1)
                        {
                            printf("d_mkdir(%s, 755)\n", kextPath);
                            d_mkdir(kextPath, 0755);
                        }
                    }

                    CFStringRef executableRelativePath = (CFStringRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkExecutableRelativePathKey));

                    if (executableRelativePath)
                    {
                        CFStringGetCString(executableRelativePath, kextBundlePathBuffer, sizeof(kextBundlePathBuffer), kCFStringEncodingUTF8);
                        printf("_PrelinkExecutableRelativePath: %s\n", kextBundlePathBuffer);

                        if (strncmp(kextBundlePathBuffer, "Contents/MacOS/", 15) == 0)
                        {
                            snprintf(kextExecutablePath, sizeof(kextExecutablePath), "%s/Contents/MacOS", kextPath);
                            printf("kextExecutablePath............: %s\n", kextExecutablePath);

                            if (stat(kextExecutablePath, &st) == -1)
                            {
                                d_mkdir(kextExecutablePath, 0755);
                                printf("d_mkdir(%s, 755)\n", kextExecutablePath);
                            }
                        } else {
                            snprintf(kextExecutablePath, sizeof(kextExecutablePath), "%s", kextPath);
                            printf("kextExecutablePath............: %s\n", kextExecutablePath);
                        }
                    }
                    
                    CFStringRef executableName = (CFStringRef)CFDictionaryGetValue(kextPlist, kCFBundleExecutableKey);
                    
                    if (executableName)
                    {
                        uint64_t offset            = 0;
                        uint64_t sourceAddress     = 0;
                        uint64_t sourceSize        = 0;
                        
                        CFStringGetCString(executableName, kextIdentifierBuffer, sizeof(kextIdentifierBuffer), kCFStringEncodingUTF8);
                        printf("CFBundleIdentifier............: %s\n", kextIdentifierBuffer);

                        CFNumberRef kextSourceAddress = (CFNumberRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkExecutableSourceKey));

                        if (kextSourceAddress != NULL)
                        {
                            CFNumberGetValue(kextSourceAddress, kCFNumberSInt64Type, &sourceAddress);
                            offset = ((sourceAddress - ((swapped == 0) ? prelinkTextSegment64->vmaddr : OSSwapInt64(prelinkTextSegment64->vmaddr))) + ((swapped == 0) ? prelinkTextSegment64->fileoff : OSSwapInt64(prelinkTextSegment64->fileoff)));

                            printf("_PrelinkExecutableSourceAddr64..: 0x%llx -> 0x%llx/%lld (offset)\n", (unsigned long long)sourceAddress, (unsigned long long)offset, (unsigned long long)offset);
                        }
                        
                        CFNumberRef kextSourceSize = (CFNumberRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkExecutableSizeKey));
                        
                        if (kextSourceSize != NULL)
                        {
                            CFNumberGetValue(kextSourceSize, kCFNumberSInt64Type, &sourceSize);
                            printf("_PrelinkExecutableSize........: 0x%llx/%lld\n", (unsigned long long)sourceSize, (unsigned long long)sourceSize);
                        }

                        machHeader = (struct mach_header *)((unsigned char *)(uintptr_t)aFileBuffer + offset);
                        
                        codeSignature = (struct linkedit_data_command *)find_load_command(machHeader, LC_CODE_SIGNATURE);
                        
                        if (codeSignature)
                        {
                            printf("Signed kext...................: Yes\n");
                            signedKexts++;
                        }

                        if ((offset) && (sourceSize))
                        {
                            char executablePath[PATH_MAX];
                            snprintf(executablePath, sizeof(executablePath), "%s/%s", kextExecutablePath, kextIdentifierBuffer);
                            printf("executablePath................: %s\n", executablePath);

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                            FILE *executable = NULL;
                            fopen_s(&executable, executablePath, "wb");
#else
                            FILE *executable = fopen(executablePath, "wb");
#endif

                            fwrite((aFileBuffer + offset), 1, (size_t)sourceSize, executable);
                            printf("Executable....................: %s (%ld bytes written)\n", kextIdentifierBuffer, ftell(executable));
                            fclose(executable);
                        }
                    }
                    
                    CFErrorRef xmlError = NULL;
                    CFDataRef xmlData = CFPropertyListCreateData(kCFAllocatorDefault, kextPlist, kCFPropertyListXMLFormat_v1_0, 0, &xmlError);
                    
                    if (xmlError == NULL)
                    {
                        const unsigned char * buffer = CFDataGetBytePtr(xmlData);
                        long xmlLength = (long)CFDataGetLength(xmlData);

                        snprintf(kextPlistPath, sizeof(kextPlistPath), "%s/Info.plist", kextPath);
                        printf("kextPlistPath.................: %s\n", kextPlistPath);

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                        FILE *infoPlist = NULL;
                        fopen_s(&infoPlist, kextPlistPath, "wb");
#else
                        FILE *infoPlist = fopen(kextPlistPath, "wb");
#endif

                        fwrite(buffer, 1, xmlLength, infoPlist);
                        printf("Info.plist....................: %ld bytes written\n", ftell(infoPlist));
                        
                        fclose(infoPlist);
                    } else {
                        printf("ERROR: Failed to convert/write Info.plist\n");
                    }
                }
                
                printf("\n%ld kexts extracted to directory %s (%d signed and %ld unsigned)\n", (long)kextCount, outdir, signedKexts, (long)(kextCount - signedKexts));
            } else {
                printf("ERROR: Can't unserialize _PrelinkInfoDictionary!\n");
                return -1;
            }
    } else if ((prelinkTextSegment32) && (prelinkInfoSegment32) && (is32bit == 1)) {
            printf("prelinkInfoSegment32->vmaddr..: 0x%lx\n", ((unsigned long)((swapped == 0) ? prelinkInfoSegment32->vmaddr : OSSwapInt32(prelinkInfoSegment32->vmaddr))));
            printf("prelinkInfoSegment32->fileoff.: 0x%lx\n", ((unsigned long)((swapped == 0) ? prelinkInfoSegment32->fileoff : OSSwapInt32(prelinkInfoSegment32->fileoff))));
            printf("prelinkInfoSegment32->filesize: 0x%lx\n", ((unsigned long)((swapped == 0) ? prelinkInfoSegment32->filesize : OSSwapInt32(prelinkInfoSegment32->filesize))));

            const char *prelinkInfoBytes = (const char *)aFileBuffer + ((swapped == 0) ? prelinkInfoSegment32->fileoff : OSSwapInt32(prelinkInfoSegment32->fileoff));
            
            CFPropertyListRef prelinkInfoPlist = (CFPropertyListRef)IOCFUnserialize(prelinkInfoBytes, kCFAllocatorDefault, 0, NULL);

            if (prelinkInfoPlist)
            {
                printf("NOTICE: Unserialized prelink info\n");
                
                CFArrayRef kextPlistArray = NULL;
                kextPlistArray = (CFArrayRef)CFDictionaryGetValue(prelinkInfoPlist, CFSTR("_PrelinkInfoDictionary"));
                CFIndex i = 0;
                CFIndex kextCount = CFArrayGetCount(kextPlistArray);
                printf("kextCount: %ld\n", (long)kextCount);
                
                char kextIdentifierBuffer[KMOD_MAX_NAME];
                char kextBundlePathBuffer[PATH_MAX];
                char kextPath[PATH_MAX];
                char kextPlistPath[PATH_MAX];
                char kextExecutablePath[PATH_MAX];
                
                struct stat st = {0};

		if (outdir == NULL)
		{
		    outdir = "kexts";
		}
	
                if ((kextCount) && (stat(outdir, &st) == -1))
                {
                    mkdir(outdir, 0755);
                }
                
                for (i = 0; i < kextCount; i++)
                {
                    // printf("kextPlist: 0x%llx\n", (int64_t)kextPlist - (int64_t)fileBuffer - prelinkInfoSegment->fileoff);
                    CFDictionaryRef kextPlist = (CFDictionaryRef)CFArrayGetValueAtIndex(kextPlistArray, i);
                    CFStringRef kextIdentifier = (CFStringRef)CFDictionaryGetValue(kextPlist, kCFBundleIdentifierKey);
                    
                    if (kextIdentifier)
                    {
                        CFStringGetCString(kextIdentifier, kextIdentifierBuffer, sizeof(kextIdentifierBuffer), kCFStringEncodingUTF8);
                        printf("\nCFBundleIdentifier[%3ld].......: %s\n", (long)i, kextIdentifierBuffer);
                    }
                    
                    CFStringRef bundlePath = (CFStringRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkBundlePathKey));
                    
                    if (bundlePath)
                    {
                        CFStringGetCString(bundlePath, kextBundlePathBuffer, sizeof(kextBundlePathBuffer), kCFStringEncodingUTF8);
                        printf("_PrelinkBundlePath............: %s\n", kextBundlePathBuffer);
                        
                        snprintf(kextPath, sizeof(kextPath), "%s%s", outdir, kextBundlePathBuffer);
                        printf("kextPath......................: %s\n", kextPath);
                        
                        if (stat(kextPath, &st) == -1)
                        {
                            printf("d_mkdir(%s, 755)\n", kextPath);
                            d_mkdir(kextPath, 0755);
                        }
                    }
                    
                    CFStringRef executableRelativePath = (CFStringRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkExecutableRelativePathKey));
                    
                    if (executableRelativePath)
                    {
                        CFStringGetCString(executableRelativePath, kextBundlePathBuffer, sizeof(kextBundlePathBuffer), kCFStringEncodingUTF8);
                        printf("_PrelinkExecutableRelativePath: %s\n", kextBundlePathBuffer);
                        
                        if (strncmp(kextBundlePathBuffer, "Contents/MacOS/", 15) == 0)
                        {
                            snprintf(kextExecutablePath, sizeof(kextExecutablePath), "%s/Contents/MacOS", kextPath);
                            printf("kextExecutablePath............: %s\n", kextExecutablePath);
                            
                            if (stat(kextExecutablePath, &st) == -1)
                            {
                                d_mkdir(kextExecutablePath, 0755);
                                printf("d_mkdir(%s, 755)\n", kextExecutablePath);
                            }
                        } else {
                            snprintf(kextExecutablePath, sizeof(kextExecutablePath), "%s", kextPath);
                            printf("kextExecutablePath............: %s\n", kextExecutablePath);
                        }
                    }

                    CFStringRef executableName = (CFStringRef)CFDictionaryGetValue(kextPlist, kCFBundleExecutableKey);
                    
                    if (executableName)
                    {
                        uint64_t offset            = 0;
                        uint64_t sourceAddress     = 0;
                        uint64_t sourceSize        = 0;
                        
                        CFStringGetCString(executableName, kextIdentifierBuffer, sizeof(kextIdentifierBuffer), kCFStringEncodingUTF8);
                        printf("CFBundleIdentifier............: %s\n", kextIdentifierBuffer);
                        
                        CFNumberRef kextSourceAddress = (CFNumberRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkExecutableSourceKey));
                        
                        if (kextSourceAddress != NULL)
                        {
                            CFNumberGetValue(kextSourceAddress, kCFNumberSInt64Type, &sourceAddress);
                            offset = ((sourceAddress - prelinkTextSegment32->vmaddr) + prelinkTextSegment32->fileoff);
                            printf("_PrelinkExecutableSourceAddr32..: 0x%llx -> 0x%llx/%lld (offset)\n", (unsigned long long)sourceAddress, (unsigned long long)offset, (unsigned long long)offset);
                        }
                        
                        CFNumberRef kextSourceSize = (CFNumberRef)CFDictionaryGetValue(kextPlist, CFSTR(kPrelinkExecutableSizeKey));

                        if (kextSourceSize != NULL)
                        {
                            CFNumberGetValue(kextSourceSize, kCFNumberSInt64Type, &sourceSize);
                            printf("_PrelinkExecutableSize........: 0x%llx/%lld\n", (unsigned long long)sourceSize, (unsigned long long)sourceSize);
                        }

                        machHeader = (struct mach_header *)((unsigned char *)(uintptr_t)aFileBuffer + offset);
                        
                        codeSignature = (struct linkedit_data_command *)find_load_command(machHeader, LC_CODE_SIGNATURE);

                        if (codeSignature)
                        {
                            printf("Signed kext...................: Yes\n");
                            signedKexts++;
                        }

                        if ((offset) && (sourceSize))
                        {
                            char executablePath[PATH_MAX];
                            snprintf(executablePath, sizeof(executablePath), "%s/%s", kextExecutablePath, kextIdentifierBuffer);
                            printf("executablePath................: %s\n", executablePath);

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                            FILE *executable = NULL;
                            fopen_s(&executable, executablePath, "wb");
#else
                            FILE *executable = fopen(executablePath, "wb");
#endif

                            fwrite((aFileBuffer + offset), 1, (size_t)sourceSize, executable);
                            printf("Executable....................: %s (%ld bytes written)\n", kextIdentifierBuffer, ftell(executable));
                            fclose(executable);
                        }
                    }

                    CFErrorRef xmlError = NULL;
                    CFDataRef xmlData = CFPropertyListCreateData(kCFAllocatorDefault, kextPlist, kCFPropertyListXMLFormat_v1_0, 0, &xmlError);
                    
                    if (xmlError == NULL)
                    {
                        const unsigned char * buffer = CFDataGetBytePtr(xmlData);
                        long xmlLength = (long)CFDataGetLength(xmlData);

                        snprintf(kextPlistPath, sizeof(kextPlistPath), "%s/Info.plist", kextPath);
                        printf("kextPlistPath.................: %s\n", kextPlistPath);

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
                        FILE *infoPlist = NULL;
                        fopen_s(&infoPlist, kextPlistPath, "wb");
#else
                        FILE *infoPlist = fopen(kextPlistPath, "wb");
#endif

                        fwrite(buffer, 1, xmlLength, infoPlist);
                        printf("Info.plist....................: %ld bytes written\n", ftell(infoPlist));

                        fclose(infoPlist);
                    } else {
                        printf("ERROR: Failed to convert/write Info.plist\n");
                    }
                }
                
                printf("\n%ld kexts extracted to directory %s (%d signed and %ld unsigned)\n", (long)kextCount, outdir, signedKexts, (long)(kextCount - signedKexts));
            } else {
                printf("ERROR: Can't unserialize _PrelinkInfoDictionary!\n");

                return -1;
            }
    } else {
        printf("Unsupported binary, header magic 0x%8X\n", machHeader->magic);

        return -1;
    }
    
    return 0;
}

void Usage(const char *name)
{
    printf("AnV LZVN/LZSS kernel cache tool and decompressor V2.3\n");
    printf("Usage: %s <infile> <outfile> [-kernel|-kexts|-list|-dict] [arch]\n\n", name);
    printf("-kernel     - extracts only the kernel from the prelinked kernel\n");
    printf("-kexts      - extracts the kexts from the prelinked kernel\n");
    printf("-list       - lists the info in the prelinked kernel\n");
    printf("-dict       - extracts the dictionary from the prelinked kexts\n");
    printf("-recomplzss - compress prelinked kernel using LZSS\n");
    printf("-recomplzvn - compress prelinked kernel using LZVN\n");
    printf("arch        - is an architecture to use\n\n");
    printf("Supported CPU architectures:\n");
    printf(" -> i386     => Intel 32-bit\n");
    printf(" -> x86_64   => Intel 64-bit\n");
    printf(" -> i860     => I860 32-bit\n");
    printf(" -> ppc      => PowerPC 32-bit\n");
    printf(" -> ppc64    => PowerPC 64-bit\n");
    printf(" -> arm      => ARM 32-bit\n");
    printf(" -> arm64    => ARM 64-bit\n");
    printf(" -> arm64_32 => ARM 64-bit with 32-bit types\n");
    printf(" -> vax      => VAX 32-bit\n");
    printf(" -> mc68k    => MC680X0 32-bit\n");
    printf(" -> mc88k    => MC88000 32-bit\n");
    printf(" -> mc98k    => MC98000 32-bit\n");
    printf(" -> sparc    => SPARC 32-bit\n");
    printf(" -> alpha    => ALPHA 32-bit\n\n");
    printf("Copyright (C) 2014-2020 AnV Software\n");

#if defined(_WIN32) || defined(WIN32) || defined(_WINDOWS) || defined(WINDOWS)
    printf("Windows version\n");
#elif defined(__ANDROID__)
    printf("Android version\n");
#elif defined(__linux__)
    printf("Linux version\n");
#elif defined(__APPLE__) && defined(__MACH__)
    printf("macOS/iOS version/AppleTV OS/WatchOS\n");
#endif /* Version info extra */
}

uint32_t local_adler32(uint8_t *buffer, int32_t length)
{
    int32_t cnt = 0;
    uint32_t result = 0;
    uint32_t lowHalf = 1;
    uint32_t highHalf = 0;
    
    for (cnt = 0; cnt < length; cnt++)
    {
        if ((cnt % 5000) == 0)
        {
            lowHalf  %= 65521L;
            highHalf %= 65521L;
        }
        
        lowHalf += buffer[cnt];
        highHalf += lowHalf;
    }
    
    lowHalf  %= 65521L;
    highHalf %= 65521L;

    result = (highHalf << 16) | lowHalf;

    return result;
}

#ifndef N
#define N         4096  /* size of ring buffer - must be power of 2 */
#endif /* N */

#ifndef F
#define F         18    /* upper limit for match_length */
#endif /* F */

#ifndef THRESHOLD
#define THRESHOLD 2     /* encode string into position and length */
#endif /* THRESHOLD */

#ifndef NIL
#define NIL N /* index for root of binary search trees */
#endif

typedef struct encode_state
{
    /*
     * left & right children & parent. These constitute binary search trees.
     */
    long lchild[N+1];
    long rchild[N+257];
    long parent[N+1];

    /* ring buffer of size N, with extra F-1 bytes to aid string comparison */
    uint8_t text_buf[(N+F)-1];

    /*
     * match_length of longest match.
     * These are set by the insert_node() procedure.
     */
    long match_position;
    long match_length;
} encode_state_t;

static void init_state(struct encode_state *sp)
{
    size_t i = 0;

    if (sp == NULL)
    {
    return;
    }

    memset(sp, 0, sizeof(*sp));

    for (i = 0; i < (N - F); i++)
    {
        sp->text_buf[i] = ' ';
    }

    for (i = (N + 1); i <= (N + 256); i++)
    {
        sp->rchild[i] = NIL;
    }

    for (i = 0; i < N; i++)
    {
        sp->parent[i] = NIL;
    }
}

static void insert_node(struct encode_state *sp, int r)
{
    int cmp = 1;
    uint8_t *key = &sp->text_buf[r];
    int p = N + 1 + key[0];
    size_t i = 0;

    sp->rchild[r] = sp->lchild[r] = NIL;
    sp->match_length = 0;

    while (1)
    {
        if (cmp >= 0)
    {
            if (sp->rchild[p] != NIL)
        {
                p = (int)sp->rchild[p];
            } else {
                sp->rchild[p] = r;
                sp->parent[r] = p;
                return;
            }
        } else {
            if (sp->lchild[p] != NIL)
        {
                p = (int)sp->lchild[p];
            } else {
                sp->lchild[p] = r;
                sp->parent[r] = p;

                return;
            }
        }

        for (i = 1; i < F; i++)
    {
            if ((cmp = (key[i] - sp->text_buf[p+i])) != 0)
        {
                break;
        }
        }

        if (i > (size_t)sp->match_length)
    {
            sp->match_position = p;

            if ((sp->match_length = (long)i) >= F)
        {
                break;
        }
        }
    }

    sp->parent[r] = sp->parent[p];

    sp->lchild[r] = sp->lchild[p];
    sp->rchild[r] = sp->rchild[p];

    sp->parent[sp->lchild[p]] = r;
    sp->parent[sp->rchild[p]] = r;

    if (sp->rchild[sp->parent[p]] == p)
    {
        sp->rchild[sp->parent[p]] = r;
    } else {
        sp->lchild[sp->parent[p]] = r;
    }

    sp->parent[p] = NIL; /* remove p */
}

/* deletes node p from tree */
static void delete_node(struct encode_state *sp, int p)
{
    int q = 0;

    if (sp->parent[p] == NIL)
    {
        return; /* not in tree */
    }

    if (sp->rchild[p] == NIL)
    {
        q = (int)sp->lchild[p];
    } else if (sp->lchild[p] == NIL) {
        q = (int)sp->rchild[p];
    } else {
        q = (int)sp->lchild[p];

        if (sp->rchild[q] != NIL)
    {
            do
        {
                q = (int)sp->rchild[q];
            } while (sp->rchild[q] != NIL);

            sp->rchild[sp->parent[q]] = sp->lchild[q];
            sp->parent[sp->lchild[q]] = sp->parent[q];

            sp->lchild[q] = sp->lchild[p];
            sp->parent[sp->lchild[p]] = q;
        }

        sp->rchild[q] = sp->rchild[p];
        sp->parent[sp->rchild[p]] = q;
    }

    sp->parent[q] = sp->parent[p];

    if (sp->rchild[sp->parent[p]] == p)
    {
        sp->rchild[sp->parent[p]] = q;
    } else {
        sp->lchild[sp->parent[p]] = q;
    }

    sp->parent[p] = NIL;
}

void *compress_lzss(void *dst, size_t dstlen, void *src, size_t srcLen)
{
    /* Encoding state, mostly tree but some current match stuff */
    struct encode_state *sp = NULL;

    long i = 0, c = 0, len = 0, r = 0, s = 0, last_match_length = 0, code_buf_ptr = 0;

    uint8_t code_buf[17] = { 0x00 };
    uint8_t mask = 0;
    uint8_t *srcend = (uint8_t *)(src) + srcLen;
    uint8_t *dstend = (uint8_t *)(dst) + dstlen;

    /* initialize trees */
    sp = (struct encode_state *)malloc(sizeof(*sp));

    init_state(sp);

    /*
     * code_buf[1..16] saves eight units of code, and code_buf[0] works
     * as eight flags, "1" representing that the unit is an unencoded
     * letter (1 byte), "0" a position-and-length pair (2 bytes).
     * Thus, eight units require at most 16 bytes of code.
     */
    code_buf[0] = 0;
    code_buf_ptr = mask = 1;

    /* Clear the buffer with any character that will appear often. */
    s = 0;
    r = N - F;

    /* Read F bytes into the last F bytes of the buffer */
    for (len = 0; (len < F) && ((uint8_t *)(src) < srcend); len++)
    {
        uint8_t* srcptr = src;
        sp->text_buf[r + len] = *(uint8_t *)(srcptr++);
        src = srcptr;
    }

    if (len == 0)
    {
    /* text of size zero */
        return (NULL);
    }

    /*
     * Insert the F strings, each of which begins with one or more
     * 'space' characters.  Note the order in which these strings are
     * inserted.  This way, degenerate trees will be less likely to occur.
     */
    for (i = 1; i <= F; i++)
    {
        insert_node(sp, (int)(r - i));
    }

    /*
     * Finally, insert the whole string just read.
     * The global variables match_length and match_position are set.
     */
    insert_node(sp, (int)r);

    do {
        /* match_length may be spuriously long near the end of text. */
        if (sp->match_length > len)
    {
            sp->match_length = len;
    }

        if (sp->match_length <= THRESHOLD)
    {
            sp->match_length = 1;  /* Not long enough match.  Send one byte. */
            code_buf[0] |= mask;  /* 'send one byte' flag */
            code_buf[code_buf_ptr++] = sp->text_buf[r];  /* Send uncoded. */
        } else {
            /* Send position and length pair. Note match_length > THRESHOLD. */
            code_buf[code_buf_ptr++] = ((uint8_t)sp->match_position);
            code_buf[code_buf_ptr++] = ((uint8_t)(((sp->match_position >> 4) & 0xF0) | (sp->match_length - (THRESHOLD + 1))));
        }

        if ((mask <<= 1) == 0) {  /* Shift mask left one bit. */
            /* Send at most 8 units of code together */
            for (i = 0; i < code_buf_ptr; i++)
        {
                if ((uint8_t *)(dst) < dstend)
        {
                    uint8_t* dstptr = dst;
                    *(uint8_t *)(dstptr++) = code_buf[i];
                    dst = dstptr;
                } else {
                    return (NULL);
        }
        }

            code_buf[0] = 0;
            code_buf_ptr = mask = 1;
        }

        last_match_length = sp->match_length;

        for (i = 0; (i < last_match_length) && ((uint8_t *)(src) < srcend); i++)
    {
            delete_node(sp, (int)s);    /* Delete old strings and */
            uint8_t* srcptr = src;
            c = *(long *)(srcptr++);
            src = srcptr;
            sp->text_buf[s] = (uint8_t)c;    /* read new bytes */

            /*
             * If the position is near the end of buffer, extend the buffer
             * to make string comparison easier.
             */
            if (s < F - 1)
        {
                sp->text_buf[s + N] = (uint8_t)c;
        }

            /* Since this is a ring buffer, increment the position modulo N. */
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);

            /* Register the string in text_buf[r..r+F-1] */
            insert_node(sp, (int)r);
        }

        while (i++ < last_match_length)
    {
            delete_node(sp, (int)s);

            /* After the end of text, no need to read, */
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);

            /* but buffer may not be empty. */
            if (--len)
        {
                insert_node(sp, (int)r);
        }
        }
    } while (len > 0);   /* until length of string to be processed is zero */

    if (code_buf_ptr > 1)
    {    /* Send remaining code. */
        for (i = 0; i < code_buf_ptr; i++)
    {
            if ((uint8_t *)(dst) < dstend)
        {
                uint8_t* dstptr = dst;
                *(uint8_t *)(dstptr++) = code_buf[i];
                dst = dstptr;
            } else {
                return (NULL);
        }
    }
    }

    return (dst);
}

size_t decompress_lzss(void *dstbuf, size_t dstlen, void *srcbuf, size_t srclen)
{
    /* ring buffer of size N, with extra F-1 bytes to aid string comparison */
    uint8_t *src = (uint8_t *)srcbuf;
    uint8_t *dst = (uint8_t *)dstbuf;
    uint8_t text_buf[((N)+(F))-1];
    uint8_t *dststart = dst;
    const uint8_t *dstend = ((const uint8_t *)dst + dstlen);
    const uint8_t *srcend = ((const uint8_t *)src + srclen);
    uint32_t i;
    uint32_t j;
    uint32_t k;
    uint32_t r;
    uint32_t c;
    uint32_t flags;
    
    dst = dststart;

    for (i = 0; i < ((N)-(F)); i++)
    {
        text_buf[i] = ' ';
    }

    r = N - F;
    flags = 0;

    while (1)
    {
        if (((flags >>= 1) & 0x100) == 0)
        {
            if (src < srcend)
            {
                c = *src++;
            } else {
                break;
            }

            flags = c | 0xFF00;  /* uses higher byte cleverly */
        }   /* to count eight */

        if (flags & 1)
        {
            if (src < srcend)
            {
                c = *src++;
            } else {
                break;
            }

            if (dst < dstend)
            {
                *dst++ = c;
            } else {
                break;
            }

            text_buf[r++] = c;
            r &= (N - 1);
        } else {
            if (src < srcend)
            {
                i = *src++;
            } else {
                break;
            }

            if (src < srcend)
            {
                j = *src++;
            } else {
                break;
            }

            i |= ((j & 0xF0) << 4);
            j  = ((j & 0x0F) + (THRESHOLD));

            for (k = 0; k <= j; k++)
            {
                c = text_buf[(i+k) & ((N)-1)];

                if (dst < dstend)
                {
                    *dst++ = c;
                } else {
                    break;
                }

                text_buf[r++] = c;

                r &= (N - 1);
            }
        }
    }

    return (size_t)(dst - dststart);
}

int main(int argc, char **argv)
{
    cpu_type_t targetcputype = CPU_TYPE_ANY;
    FILE *f = NULL;
    unsigned char *buffer = NULL;
    unsigned long actuallen = 0;
    unsigned long buflen = 0;
    PrelinkedKernelHeader *prelinkfile;
    fat_header_t *fathdr;
    fat_arch_t *fatarch;
    fat_arch_64_t *fatarch64;
    struct mach_header *machHeader = NULL;
    unsigned char *combuffer = NULL;
    unsigned long combuflen = 0;
    unsigned char *uncombuffer = NULL;
    unsigned long uncombuflen = 0;
    uint32_t adler32_cv = 0;
	uint32_t adler32_ck = 0;
	unsigned int ac = 0;
	int rv = 0;
    int swapped = 0;
    int do_kern = 0;
    int do_kext = 0;
    int do_list = 0;
    int do_dict = 0;
    int do_recomp = 0;
    int do_lzvn = 0;

    if (argc < 3)
    {
        Usage(argv[0]);

        return 1;
    } else if (argc == 4) {
        if (strncmp("-kernel", argv[3], 7) == 0)
        {
            printf("Extracting kernel only from prelinked kernel\n");

            do_kern = 1;
        } else if (strncmp("-kexts", argv[3], 6) == 0) {
            printf("Extracting kexts from prelinked kernel\n");

            do_kext = 1;
        } else if (strncmp("-list", argv[3], 5) == 0) {
            printf("Listing kexts info from prelinked kernel\n");

            do_list = 1;
        } else if (strncmp("-dict", argv[3], 5) == 0) {
            printf("Extracting kext dictionary info from prelinked kernel\n");

            do_dict = 1;
        } else if (strncmp("-recomplzvn", argv[3], 11) == 0) {
            printf("Recompressing prelinked kernel using LZVN\n");

            do_recomp = 1;
            do_lzvn = 1;
        } else if (strncmp("-recomplzss", argv[3], 11) == 0) {
            printf("Recompressing prelinked kernel using LZSS\n");

            do_recomp = 1;
            do_lzvn = 0;
        } else if ((targetcputype = cpu_type_for_name(argv[3])) != CPU_TYPE_ANY) {
            printf("Target architecture for operations: %s\n", argv[3]);
        }
    } else if (argc >= 5) {
        if (targetcputype == CPU_TYPE_ANY)
        {
            if ((targetcputype = cpu_type_for_name(argv[4])) != CPU_TYPE_ANY)
            {
                printf("Target CPU architecture set to %s\n", argv[4]);
            }
        }
    }

#if __STDC_WANT_SECURE_LIB__
	fopen_s(&f, argv[1], "rb");
#else
    f = fopen(argv[1], "rb");
#endif

    if (f == NULL)
    {
        printf("ERROR: Could not open file %s for reading!\n", argv[1]);

        return -1;
    }

    fseek(f,0,SEEK_END);
	buflen = ftell(f);
	fseek(f,0,SEEK_SET);

    buffer = malloc(buflen);

    if (buffer == NULL)
    {
        printf("ERROR: Memory allocation error while allocating file input buffer\n");

        if (f)
        {
            fclose(f);
        }

        return -2;
    }
    
#if __STDC_WANT_SECURE_LIB__
    actuallen = (unsigned long)fread_s(buffer, buflen, 1, buflen, f);
#else /* OTHER */
    actuallen = (unsigned long)fread(buffer, 1, buflen, f);
#endif /* READ FUNCTION */

    if (f)
    {
        fclose(f);
    }

    if (actuallen != buflen)
    {
        printf("ERROR: Read too few bytes from file %s, %lu bytes wanted but %lu bytes read!\n", argv[1], buflen, actuallen);

        if (buffer)
        {
            free(buffer);
        }

        return -3;
    }

    if (is_prelinkedkernel((unsigned char *)buffer) == 1)
    {
        uncombuffer = buffer;
        uncombuflen = actuallen;

        machHeader = (struct mach_header *)buffer;
        swapped = (int)((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));

        printf("Found decompressed kernel cache (%s)\n", cpu_name_for_type(machHeader->cputype));

        if (targetcputype != CPU_TYPE_ANY)
        {
            if (((swapped == 0) ? machHeader->cputype : OSSwapInt32(machHeader->cputype)) != targetcputype)
            {
                printf("ERROR: Target architecture (%s) is not binary architecture (%s), aborting\n", cpu_name_for_type(targetcputype), cpu_name_for_type((swapped == 0) ? machHeader->cputype : OSSwapInt32(machHeader->cputype)));

                if (uncombuffer != NULL)
                {
                    free(uncombuffer);
                }

                return -4;
            }
        }

        if ((do_dict == 0) && (do_kern == 0) && (do_kext == 0) && (do_list == 0) && (do_recomp == 0))
        {
            printf("ERROR: [-kernel|-kexts|-list|-dict|-recomplzvn|-recomplzss] option is needed for a valid operation on a decompressed kernel cache\n");

            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }

            return -4;
        }

        if (do_recomp == 1)
        {
            PrelinkedKernelHeader khdr;
            char *plkdata = NULL;
            size_t plkdsize = 0;
            char *workSpace = NULL;

            if (do_lzvn == 1)
            {
                workSpace = malloc(lzvn_encode_work_size());

                if (workSpace == NULL)
                {
                    printf("ERROR: Allocate work space error (%lu bytes needed)!\n", (unsigned long)lzvn_encode_work_size());

                    return -4;
                }
            }

#if defined(__ppc__) || defined(__ppc64__)
            khdr.signature = 0x636F6D70;
            khdr.compressType = (do_lzvn == 1) ? 0x6C7A766E : 0x6C7A7373;
            khdr.prelinkVersion = 0x1;
#else
            khdr.signature = 0x706D6F63;
            khdr.compressType = (do_lzvn == 1) ? 0x6E767A6C : 0x73737A6C;
            khdr.prelinkVersion = 0x1000000;
#endif

            adler32_cv = local_adler32(uncombuffer, (int32_t)uncombuflen);
            khdr.adler32 = (swapped == 1) ? adler32_cv : OSSwapInt32(adler32_cv);

            memset(khdr.reserved, 0, sizeof(khdr.reserved));
            memset(khdr.platformName, 0, sizeof(khdr.platformName));
            memset(khdr.rootPath, 0, sizeof(khdr.rootPath));

            plkdsize = actuallen;

            printf("Prelinked kernel size: %lu\n", (unsigned long)plkdsize);

            khdr.uncompressedSize = (swapped == 1) ? (uint32_t)actuallen : OSSwapInt32((uint32_t)actuallen);

            plkdata = malloc(plkdsize);

            if (do_lzvn == 1)
            {
                printf("Compressing using LZVN\n");

                plkdsize = lzvn_encode(plkdata, plkdsize, (const void *)uncombuffer, uncombuflen, workSpace);

                printf("Got LZVN data size (%lu)\n", (unsigned long)plkdsize);
            } else {
                printf("Compressing using LZSS\n");

                workSpace = compress_lzss(plkdata, plkdsize, (void *)uncombuffer, uncombuflen);

                plkdsize = (size_t)(workSpace - plkdata);

                printf("Got LZSS data size (%lu)\n", (unsigned long)plkdsize);
            }

            if (plkdata == NULL)
            {
                printf("ERROR: Allocate compressed data error!\n");

                return -4;
            }

            khdr.compressedSize = (swapped == 1) ? (uint32_t)plkdsize : OSSwapInt32((uint32_t)plkdsize);

#if defined(_MSC_VER) && __STDC_WANT_SECURE_LIB__
            fopen_s(&f, argv[2], "wb");
#else
            f = fopen(argv[2], "wb");
#endif

            if (f == NULL)
            {
                printf("ERROR: Opening output file");

                return -4;
            } else {
                fwrite(&khdr, 1, sizeof(PrelinkedKernelHeader), f);
                fwrite(plkdata, 1, plkdsize, f);
                fclose(f);

                if (plkdata != NULL)
                {
                    free(plkdata);
                }

                if (do_lzvn == 1)
                {
                    if (workSpace != NULL)
                    {
                        free(workSpace);
                    }

                    printf("Wrote LZVN compressed prelinked kernel\n");
                } else {
                    printf("Wrote LZSS compressed prelinked kernel\n");
                }

                return 0;
            }
        }
    } else {
        fathdr = (fat_header_t *)buffer;
        
        if (fathdr->magic == FAT_CIGAM)
        {
            ac = 1;
            swapped = 1;
            printf("Found FAT 32-bit header magic (swapped)\n");
            
            fatarch = (fat_arch_t *)(buffer + sizeof(fat_header_t));
            
            if (targetcputype != CPU_TYPE_ANY)
            {
                if (OSSwapInt32(fatarch->cputype) == targetcputype)
                {
                    printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch->cputype)));
                    
                    prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + OSSwapInt32(fatarch->offset));
                } else {
                    printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch->cputype)), cpu_name_for_type(targetcputype));
                    
                    prelinkfile = (PrelinkedKernelHeader *)fatarch;
                }
            } else {
                prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + OSSwapInt32(fatarch->offset));
            }

            while ((ac < OSSwapInt32(fathdr->nfat_arch)) && ((prelinkfile->signature != 0x706D6F63) | (prelinkfile->signature != 0x636F6D70)))
            {
                fatarch = (fat_arch_t *)((unsigned char *)fatarch + sizeof(fat_arch_t));
                
                if (targetcputype != CPU_TYPE_ANY)
                {
                    if (OSSwapInt32(fatarch->cputype) == targetcputype)
                    {
                        printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch->cputype)));
                        
                        prelinkfile = (PrelinkedKernelHeader *)(buffer + OSSwapInt32(fatarch->offset));
                    } else {
                        printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch->cputype)), cpu_name_for_type(targetcputype));
                    }
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)(buffer + OSSwapInt32(fatarch->offset));
                }
                
                ++ac;
            }
        } else if (fathdr->magic == FAT_CIGAM_64) {
            ac = 1;
            swapped = 1;
            printf("Found FAT 64-bit header magic (swapped)\n");
            
            fatarch64 = (fat_arch_64_t *)(buffer + sizeof(fat_header_t));
            
            if (targetcputype != CPU_TYPE_ANY)
            {
                if (OSSwapInt32(fatarch64->cputype) == targetcputype)
                {
                    printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch64->cputype)));
                    
                    prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + OSSwapInt64(fatarch64->offset));
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)fatarch64;
                    
                    printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch64->cputype)), cpu_name_for_type(targetcputype));
                }
            } else {
                prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + OSSwapInt64(fatarch64->offset));
            }
            
            while ((ac < OSSwapInt32(fathdr->nfat_arch)) && ((prelinkfile->signature != 0x706D6F63) | (prelinkfile->signature != 0x636F6D70)))
            {
                fatarch64 = (fat_arch_64_t *)((unsigned char *)fatarch64 + sizeof(fat_arch_64_t));
                
                if (targetcputype != CPU_TYPE_ANY)
                {
                    if (OSSwapInt32(fatarch64->cputype) == targetcputype)
                    {
                        printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch64->cputype)));
                        
                        prelinkfile = (PrelinkedKernelHeader *)(buffer + OSSwapInt64(fatarch64->offset));
                    } else {
                        printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(OSSwapInt32(fatarch64->cputype)), cpu_name_for_type(targetcputype));
                    }
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)(buffer + OSSwapInt64(fatarch64->offset));
                }
                
                ++ac;
            }
        } else if (fathdr->magic == FAT_MAGIC) {
            ac = 1;
            swapped = 0;
            printf("Found FAT 32-bit header magic (not swapped)\n");

            fatarch = (fat_arch_t *)(buffer + sizeof(fat_header_t));
            
            if (targetcputype != CPU_TYPE_ANY)
            {
                if (fatarch->cputype == targetcputype)
                {
                    printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(fatarch->cputype));
                    
                    prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + fatarch->offset);
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)fatarch;
                    
                    printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(fatarch->cputype), cpu_name_for_type(targetcputype));
                }
            } else {
                prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + fatarch->offset);
            }
            
            while ((ac < fathdr->nfat_arch) && ((prelinkfile->signature != 0x636F6D70) | (prelinkfile->signature != 0x706D6F63)))
            {
                fatarch = (fat_arch_t *)((unsigned char *)fatarch + sizeof(fat_arch_t));
                
                if (targetcputype != CPU_TYPE_ANY)
                {
                    if (fatarch->cputype == targetcputype)
                    {
                        printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(fatarch->cputype));
                        
                        prelinkfile = (PrelinkedKernelHeader *)(buffer + fatarch->offset);
                    } else {
                        printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(fatarch->cputype), cpu_name_for_type(targetcputype));
                    }
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)(buffer + fatarch->offset);
                }
                
                ++ac;
            }
        } else if (fathdr->magic == FAT_MAGIC_64) {
            ac = 1;
            swapped = 0;
            printf("Found FAT 64-bit header magic (not swapped)\n");
            
            fatarch64 = (fat_arch_64_t *)(buffer + sizeof(fat_header_t));
            
            if (targetcputype != CPU_TYPE_ANY)
            {
                if (fatarch64->cputype == targetcputype)
                {
                    printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(fatarch64->cputype));
                    
                    prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + fatarch64->offset);
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)fatarch64;
                    
                    printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(fatarch64->cputype), cpu_name_for_type(targetcputype));
                }
            } else {
                prelinkfile = (PrelinkedKernelHeader *)((unsigned char *)buffer + fatarch64->offset);
            }

            while ((ac < fathdr->nfat_arch) && ((prelinkfile->signature != 0x636F6D70) | (prelinkfile->signature != 0x706D6F63)))
            {
                fatarch64 = (fat_arch_64_t *)((unsigned char *)fatarch64 + sizeof(fat_arch_64_t));
                
                if (targetcputype != CPU_TYPE_ANY)
                {
                    if (fatarch64->cputype == targetcputype)
                    {
                        printf("Found target architecture in FAT binary (%s)\n", cpu_name_for_type(fatarch64->cputype));
                        
                        prelinkfile = (PrelinkedKernelHeader *)(buffer + fatarch64->offset);
                    } else {
                        printf("Skipping architecture in FAT binary (%s) not being target architecture (%s)\n", cpu_name_for_type(fatarch64->cputype), cpu_name_for_type(targetcputype));
                    }
                } else {
                    prelinkfile = (PrelinkedKernelHeader *)(buffer + fatarch64->offset);
                }
                
                ++ac;
            }
        } else {
            fathdr = NULL;
            prelinkfile = (PrelinkedKernelHeader *)buffer;

            if (prelinkfile->signature == 0x706D6F63)
            {
                swapped = 1;

                printf("Found compressed prelinked kernel (swapped)\n");
            } else if (prelinkfile->signature == 0x636F6D70) {
                swapped = 0;

                printf("Found compressed prelinked kernel (not swapped)\n");
            }
        }
        
        if ((prelinkfile->signature != 0x706D6F63) && (prelinkfile->signature != 0x636F6D70))
        {
            printf("ERROR: No targets found to decompress, aborting\n");
            
            if (buffer)
            {
                free(buffer);
            }
            
            return -5;
        } else  if (prelinkfile->signature == 0x706D6F63) {
            swapped = 1;
            
            printf("Found prelinked compressed kernel signature (swapped)\n");
        } else if (prelinkfile->signature == 0x636F6D70) {
            swapped = 0;
            
            printf("Found prelinked compressed kernel signature (not swapped)\n");
        }
        
        if (((prelinkfile->compressType != 0x6E767A6C) && (prelinkfile->compressType != 0x73737A6C)) &&
            ((prelinkfile->compressType != 0x6C7A766E) && (prelinkfile->compressType != 0x6C7A7373)))
        {
            printf("ERROR: %s is not an LZVN/LZSS compressed kernel cache... not handling (Found 0x%x)\n", argv[1], prelinkfile->compressType);
            
            if (buffer)
            {
                free(buffer);
            }
            
            return -6;
        }
        
        combuflen = (swapped == 1) ? OSSwapInt32(prelinkfile->compressedSize) : prelinkfile->compressedSize;
        uncombuflen = (swapped == 1) ? OSSwapInt32(prelinkfile->uncompressedSize) : prelinkfile->uncompressedSize;
        
        if ((combuflen == 0) || (uncombuflen == 0))
        {
            printf("ERROR: invalid compressed/uncompressed size found: uncompressed=%lu, scompressed=%lu\n", uncombuflen, combuflen);
            
            if (buffer)
            {
                free(buffer);
            }
            
            return -7;
        }
        
        printf("%s: Initial compressed size -> %lu, Reported uncompressed size -> %lu\n", argv[1], combuflen, uncombuflen);
        
        combuffer = (unsigned char *)prelinkfile + sizeof(PrelinkedKernelHeader);
        uncombuffer = malloc(uncombuflen);
        
        if (uncombuffer == NULL)
        {
            printf("ERROR: Could not allocate memory for decompression buffer!\n");
            
            if (buffer)
            {
                free(buffer);
            }
            
            return -8;
        }
        
        if ((prelinkfile->compressType == 0x73737A6C) || (prelinkfile->compressType == 0x6C7A7373))
        {
            printf("Found LZSS compressed kernel\n");

            rv = (int)decompress_lzss(uncombuffer, uncombuflen, combuffer, combuflen);
        } else {
            printf("Found LZVN compressed kernel\n");

            rv = (int)lzvn_decode(uncombuffer, (size_t)uncombuflen, combuffer, (size_t)combuflen);
        }
        
        adler32_ck = (swapped == 0) ? OSSwapInt32(prelinkfile->adler32) : prelinkfile->adler32;
        
        if (buffer)
        {
            free(buffer);
        }
        
        printf("%s: Actual decompressed size -> %d\n", argv[2], rv);
        
        if (uncombuflen != (unsigned long)rv)
        {
            printf("ERROR: Actual decompressed size is not expected size (%lu is not %d)\n", uncombuflen, rv);
            
            if (uncombuffer)
            {
                free(uncombuffer);
            }
            
            return -9;
        }
        
        if (swapped == 1)
        {
            adler32_cv = OSSwapInt32(local_adler32(uncombuffer, (int32_t)uncombuflen));
        } else {
            adler32_cv = local_adler32(uncombuffer, (int32_t)uncombuflen);
        }

        if (adler32_cv != adler32_ck)
        {
            printf("ERROR: Checksum (adler32) mismatch (0x%.8X != 0x%.8X)\n", adler32_cv, adler32_ck);
            
            if (uncombuffer)
            {
                free(uncombuffer);
            }
            
            return -10;
        }
        
        printf("Decompressed kernel cache adler32 checksum value -> 0x%.8X\n", adler32_cv);
    }
    
    machHeader = (struct mach_header *)uncombuffer;
    swapped = (int)((machHeader->magic == MH_CIGAM) || (machHeader->magic == MH_CIGAM_64));
    
    if (swapped != 0)
    {
        printf("Swapping Mach-O header data\n");
    } else {
        printf("Not swapping Mach-O header data\n");
    }
    
    if (targetcputype != CPU_TYPE_ANY)
    {
        if (swapped != 0)
        {
            if (OSSwapInt32(machHeader->cputype) == targetcputype)
            {
                printf("Mach-O binary CPU architecture check OK (%s)\n", cpu_name_for_type(OSSwapInt32(machHeader->cputype)));
            } else {
                printf("Mach-O binary CPU architecture (%s) check FAIL, found (%s), aborting\n",
                       cpu_name_for_type(OSSwapInt32(machHeader->cputype)), cpu_name_for_type(targetcputype));
                
                if (uncombuffer)
                {
                    free(uncombuffer);
                }
                
                return -11;
            }
        } else {
            if (machHeader->cputype == targetcputype)
            {
                printf("Mach-O binary CPU architecture check OK (%s)\n", cpu_name_for_type(machHeader->cputype));
            } else {
                printf("Mach-O binary CPU architecture (%s) check FAIL, found (%s), aborting\n",
                       cpu_name_for_type(machHeader->cputype), cpu_name_for_type(targetcputype));
                
                if (uncombuffer)
                {
                    free(uncombuffer);
                }
                
                return -12;
            }
        }
    }
    
    if ((do_dict == 0) && (do_kern == 0) && (do_kext == 0) && (do_list == 0))
    {
        printf("Writing decompressed prelinked kernel Mach-O data to %s\n", argv[2]);
        
#if __STDC_WANT_SECURE_LIB__
        fopen_s(&f, argv[2], "wb");
#else
        f = fopen (argv[2], "wb");
#endif
        
        if (f == NULL)
        {
            printf("ERROR: Couldn't open file %s for writing!\n", argv[2]);
            
            if (uncombuffer)
            {
                free(uncombuffer);
            }
            
            return -13;
        }
        
        actuallen = (unsigned long)fwrite(uncombuffer, 1, uncombuflen, f);
        
        if (uncombuffer != NULL)
        {
            free(uncombuffer);
        }
        
        if (f)
        {
            fclose(f);
        }
        
        if (actuallen != uncombuflen)
        {
            printf("ERROR: Actual written decompressed file %s size incorrect, wanted %lu but wrote %lu\n", argv[2], uncombuflen, actuallen);
            
            return -14;
        }
    } else if (do_list == 1) {
        if (listKexts((unsigned char *)uncombuffer, argv[2]) == 0)
        {
            printf("Succesfully listed kext info\n");
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
        } else {
            printf("ERROR: List kext info failed\n");
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
            
            return -15;
        }
    } else if (do_dict == 1) {
        if (saveDictionary((unsigned char *)uncombuffer, argv[2]) == 0)
        {
            printf("Succesfully saved kext dictionary info to %s\n", argv[2]);
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
        } else {
            printf("ERROR: Save kext dictionary info failed\n");
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
            
            return -16;
        }
    } else if (do_kext == 1) {
        if (saveKexts((unsigned char *)uncombuffer, argv[2]) == 0)
        {
            printf("Succesfully extracted kext modules to directory %s from prelinked kernel\n", argv[2]);
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
        } else {
            printf("ERROR: Extract kext modules from prelinked kernel failed\n");
            
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
            
            return -17;
        }
    } else if (do_kern == 1) {
        if (saveKernel((unsigned char *)uncombuffer, argv[2]) == 0)
        {
            printf("Succesfully exported kernel to %s\n", argv[2]);
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
        } else {
            printf("ERROR: Export kernel failed\n");
            
            if (uncombuffer != NULL)
            {
                free(uncombuffer);
            }
            
            return -18;
        }
    }
    
    return 0;
}
