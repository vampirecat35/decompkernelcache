/*	CFStringEncodingConverterExt.h
	Copyright (c) 1998-2012, Apple Inc. All rights reserved.
*/

#if !defined(__COREFOUNDATION_CFSTRINGENCODINGCONVERETEREXT__)
#define __COREFOUNDATION_CFSTRINGENCODINGCONVERETEREXT__ 1

#include <CoreFoundation/CFStringEncodingConverter.h>

CF_EXTERN_C_BEGIN

#define MAX_DECOMPOSED_LENGTH (10)

enum {
    kCFStringEncodingConverterStandard = 0,
    kCFStringEncodingConverterCheapEightBit = 1,
    kCFStringEncodingConverterStandardEightBit = 2,
    kCFStringEncodingConverterCheapMultiByte = 3,
    kCFStringEncodingConverterPlatformSpecific = 4, // Other fields are ignored
    kCFStringEncodingConverterICU = 5 // Other fields are ignored
};

/* kCFStringEncodingConverterStandard */
typedef CFIndex (*CFStringEncodingToBytesProc)(uint32_t flags, const UniChar *characters, CFIndex numChars, uint8_t *bytes, CFIndex maxByteLen, CFIndex *usedByteLen);
typedef CFIndex (*CFStringEncodingToUnicodeProc)(uint32_t flags, const uint8_t *bytes, CFIndex numBytes, UniChar *characters, CFIndex maxCharLen, CFIndex *usedCharLen);
/* kCFStringEncodingConverterCheapEightBit */
typedef bool (*CFStringEncodingCheapEightBitToBytesProc)(uint32_t flags, UniChar character, uint8_t *byte);
typedef bool (*CFStringEncodingCheapEightBitToUnicodeProc)(uint32_t flags, uint8_t byte, UniChar *character);
/* kCFStringEncodingConverterStandardEightBit */
typedef uint16_t (*CFStringEncodingStandardEightBitToBytesProc)(uint32_t flags, const UniChar *characters, CFIndex numChars, uint8_t *byte);
typedef uint16_t (*CFStringEncodingStandardEightBitToUnicodeProc)(uint32_t flags, uint8_t byte, UniChar *characters);
/* kCFStringEncodingConverterCheapMultiByte */
typedef uint16_t (*CFStringEncodingCheapMultiByteToBytesProc)(uint32_t flags, UniChar character, uint8_t *bytes);
typedef uint16_t (*CFStringEncodingCheapMultiByteToUnicodeProc)(uint32_t flags, const uint8_t *bytes, CFIndex numBytes, UniChar *character);

typedef CFIndex (*CFStringEncodingToBytesLenProc)(uint32_t flags, const UniChar *characters, CFIndex numChars);
typedef CFIndex (*CFStringEncodingToUnicodeLenProc)(uint32_t flags, const uint8_t *bytes, CFIndex numBytes);

typedef CFIndex (*CFStringEncodingToBytesPrecomposeProc)(uint32_t flags, const UniChar *character, CFIndex numChars, uint8_t *bytes, CFIndex maxByteLen, CFIndex *usedByteLen);
typedef bool (*CFStringEncodingIsValidCombiningCharacterProc)(UniChar character);

typedef struct {
    void *toBytes;
    void *toUnicode;
    uint16_t maxBytesPerChar;
    uint16_t maxDecomposedCharLen;
    uint8_t encodingClass;
    uint32_t :24;
    CFStringEncodingToBytesLenProc toBytesLen;
    CFStringEncodingToUnicodeLenProc toUnicodeLen;
    CFStringEncodingToBytesFallbackProc toBytesFallback;
    CFStringEncodingToUnicodeFallbackProc toUnicodeFallback;
    CFStringEncodingToBytesPrecomposeProc toBytesPrecompose;
    CFStringEncodingIsValidCombiningCharacterProc isValidCombiningChar;
} CFStringEncodingConverter;

extern const CFStringEncodingConverter *CFStringEncodingGetConverter(uint32_t encoding);

enum {
    kCFStringEncodingGetConverterSelector = 0,
    kCFStringEncodingIsDecomposableCharacterSelector = 1,
    kCFStringEncodingDecomposeCharacterSelector = 2,
    kCFStringEncodingIsValidLatin1CombiningCharacterSelector = 3,
    kCFStringEncodingPrecomposeLatin1CharacterSelector = 4
};

extern const void *CFStringEncodingGetAddressForSelector(uint32_t selector);

#define BOOTSTRAPFUNC_NAME	CFStringEncodingBootstrap
typedef const CFStringEncodingConverter* (*CFStringEncodingBootstrapProc)(uint32_t encoding, const void *getSelector);

/* Latin precomposition */
/* This function does not precompose recursively nor to U+01E0 and U+01E1.
*/
extern bool CFStringEncodingIsValidCombiningCharacterForLatin1(UniChar character);
extern UniChar CFStringEncodingPrecomposeLatinCharacter(const UniChar *character, CFIndex numChars, CFIndex *usedChars);

/* Convenience functions for converter development */
typedef struct _CFStringEncodingUnicodeTo8BitCharMap {
    UniChar _u;
    uint8_t _c;
    uint8_t :8;
} CFStringEncodingUnicodeTo8BitCharMap;

/* Binary searches CFStringEncodingUnicodeTo8BitCharMap */
CF_INLINE bool CFStringEncodingUnicodeTo8BitEncoding(const CFStringEncodingUnicodeTo8BitCharMap *theTable, CFIndex numElem, UniChar character, uint8_t *ch) {
    const CFStringEncodingUnicodeTo8BitCharMap *p, *q, *divider;

    if ((character < theTable[0]._u) || (character > theTable[numElem-1]._u)) {
        return 0;
    }
    p = theTable;
    q = p + (numElem-1);
    while (p <= q) {
        divider = p + ((q - p) >> 1);	/* divide by 2 */
        if (character < divider->_u) { q = divider - 1; }
        else if (character > divider->_u) { p = divider + 1; }
        else { *ch = divider->_c; return 1; }
    }
    return 0;
}

#ifndef CF_OPEN_SOURCE
#if KERNEL
#include <TargetConditionals.h>
#endif // KERNEL
#if TARGET_OS_MAC || TARGET_OS_WIN32 || TARGET_OS_EMBEDDED || TARGET_OS_IPHONE
// The following defines are used by 16bit converters in CFStringEncodingConverters project
typedef struct _CFStringEncodingUnicodeTo16BitCharMap {
    UniChar _u;
    uint16_t _c;
} CFStringEncodingUnicodeTo16BitCharMap;

/* Binary searches CFStringEncodingUnicodeTo8BitCharMap */
CF_INLINE bool CFStringEncodingUnicodeTo16BitEncoding(const CFStringEncodingUnicodeTo16BitCharMap *theTable, CFIndex numElem, UniChar character, uint16_t *ch) {
    const CFStringEncodingUnicodeTo16BitCharMap *p, *q, *divider;

    if ((character < theTable[0]._u) || (character > theTable[numElem-1]._u)) {
        return 0;
    }
    p = theTable;
    q = p + (numElem-1);
    while (p <= q) {
        divider = p + ((q - p) >> 1);	/* divide by 2 */
        if (character < divider->_u) { q = divider - 1; }
        else if (character > divider->_u) { p = divider + 1; }
        else { *ch = divider->_c; return 1; }
    }
    return 0;
}

typedef struct _CFStringEncodingTwoCharUnicodeTo16BitCharMap {
    uint32_t _u;
    uint16_t _c;
    uint16_t :16;
} CFStringEncodingTwoCharUnicodeTo16BitCharMap;

/* Binary searches CFStringEncodingUnicodeTo8BitCharMap */
CF_INLINE bool CFStringEncodingTwoCharUnicodeTo16BitEncoding(const CFStringEncodingTwoCharUnicodeTo16BitCharMap *theTable, CFIndex numElem, uint32_t character, uint16_t *ch) {
    const CFStringEncodingTwoCharUnicodeTo16BitCharMap *p, *q, *divider;

    if ((character < theTable[0]._u) || (character > theTable[numElem-1]._u)) {
        return 0;
    }
    p = theTable;
    q = p + (numElem-1);
    while (p <= q) {
        divider = p + ((q - p) >> 1);	/* divide by 2 */
        if (character < divider->_u) { q = divider - 1; }
        else if (character > divider->_u) { p = divider + 1; }
        else { *ch = divider->_c; return 1; }
    }
    return 0;
}

#define CFCharMappingDefinitionWithSize(theSize)		\
typedef struct _CFStringEncodingUnicodeTo16BitCharMapWithArray##theSize {\
    UniChar startChar;\
    uint16_t bytes[theSize];\
} CFStringEncodingUnicodeTo16BitCharMapWithArray##theSize;\
CF_INLINE uint16_t CFStringEncodingUnicodeTo16BitEncodingWithArray##theSize(const CFStringEncodingUnicodeTo16BitCharMapWithArray##theSize *theTable, CFIndex numElem, UniChar character) {\
    const CFStringEncodingUnicodeTo16BitCharMapWithArray##theSize *p, *q, *divider;\
    if ((character < theTable[0].startChar) || (character > theTable[numElem-1].startChar + theSize)) {\
        return 0;\
    }\
    p = theTable;\
    q = p + (numElem-1);\
    while (p <= q) {\
        divider = p + ((q - p) >> 1);	/* divide by 2 */\
        if (character < divider->startChar) { q = divider - 1; }\
        else if (character < (divider->startChar + theSize)) { return divider->bytes[character - divider->startChar]; }\
        else { p = divider + 1; }\
    }\
    return 0;\
}

/* Emoji encodings
 The values are temporarily and subject to change (there is already a plan to do so).
 It's intended to be used only from desktop Mail.
 */
#define __COREFOUNDATION_USE_EMOJI_CONVERTERS__ 1
enum {
    kCFStringEncodingShiftJIS_NTT_Docomo = 0x0BF0,
    kCFStringEncodingShiftJIS_KDDI = 0x0BF1,
    kCFStringEncodingShiftJIS_SoftBank = 0x0BF2,

    kCFStringEncodingISO_2022_JP_NTT_Docomo = 0x0BF3,
    kCFStringEncodingISO_2022_JP_KDDI = 0x0BF4
};
#endif
#endif // CF_OPEN_SOURCE

CF_EXTERN_C_END

#endif /* ! __COREFOUNDATION_CFSTRINGENCODINGCONVERETEREXT__ */

