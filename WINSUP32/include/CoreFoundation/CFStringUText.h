/* CFStringUText.h
    Copyright (c) 2006-2012, Apple Inc. All rights reserved.
*/

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFAttributedString.h>
#include <unicode/utext.h>

CF_EXTERN_C_BEGIN

CF_EXPORT
UText *CFStringOpenUText(UText *ut, CFStringRef string);

CF_EXPORT
UText *CFMutableStringOpenUText(UText *ut, CFMutableStringRef string);

CF_EXPORT
UText *CFAttributedStringOpenUText(UText *ut, CFAttributedStringRef string);

CF_EXPORT
UText *CFMutableAttributedStringOpenUText(UText *ut, CFMutableAttributedStringRef string);

CF_EXTERN_C_END
