/*
 *  CFUnicodePrecomposition.h
 *  CoreFoundation
 *
 *  Created by aki on Wed Oct 03 2001.
 *  Copyright (c) 2001-2012, Apple Inc. All rights reserved.
 *
 */

#if !defined(__COREFOUNDATION_CFUNICODEPRECOMPOSITION__)
#define __COREFOUNDATION_CFUNICODEPRECOMPOSITION__ 1

#include <CoreFoundation/CFUniChar.h>

CF_EXTERN_C_BEGIN

// As you can see, this function cannot precompose Hangul Jamo
CF_EXPORT UTF32Char CFUniCharPrecomposeCharacter(UTF32Char base, UTF32Char combining);

CF_EXPORT bool CFUniCharPrecompose(const UTF16Char *characters, CFIndex length, CFIndex *consumedLength, UTF16Char *precomposed, CFIndex maxLength, CFIndex *filledLength);

CF_EXTERN_C_END

#endif /* ! __COREFOUNDATION_CFUNICODEPRECOMPOSITION__ */

