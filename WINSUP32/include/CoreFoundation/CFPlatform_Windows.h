/*	CFPlatform_Windows.h
	Copyright (c) 2013, Apple Inc. All rights reserved.
*/

/* This file contains compatibility functions that CF exports, on Windows only. */

#ifndef __CFPLATFORM_WINDOWS__
#define __CFPLATFORM_WINDOWS__

#include <stdio.h>
#include <locale.h>

#include <CoreFoundation/CFBase.h>

CF_EXPORT size_t _NS_strlcpy(char * /*restrict*/ dst, const char * /*restrict*/ src, size_t maxlen);
CF_EXPORT size_t _NS_strlcat(char * /*restrict*/ dst, const char * /*restrict*/ src, size_t maxlen);

CF_EXPORT double _NS_strtod_l(const char *ptr, char **endPtr, _locale_t loc);
CF_EXPORT unsigned long long _NS_strtoull_l(const char *ptr, char **endPtr, int base, _locale_t loc);
CF_EXPORT long long _NS_strtoll_l(const char *ptr, char **endPtr, int base, _locale_t loc);
CF_EXPORT unsigned long _NS_strtoul_l(const char *ptr, char **endPtr, int base, _locale_t loc);
CF_EXPORT long _NS_strtol_l(const char *ptr, char **endPtr, int base, _locale_t loc);

CF_EXPORT int _NS_strncasecmp_l(const char *s1, const char *s2, size_t n, _locale_t loc);
CF_EXPORT int _NS_fprintf_l(FILE *stream, _locale_t loc, const char *fmt, ...);
CF_EXPORT int _NS_snprintf_l(char *str, size_t size, _locale_t loc, const char *fmt, ...);
CF_EXPORT int _NS_asprintf(char **ret, const char *format, ...);
CF_EXPORT int _NS_sscanf_l(const char *str, _locale_t loc, const char *fmt, ...);
CF_EXPORT int _NS_vasprintf(char **ret, const char *format, va_list ap);
CF_EXPORT int _NS_vasprintf_l(char **ret, _locale_t loc, const char *format, va_list ap);
CF_EXPORT int _NS_asprintf_l(char **ret, _locale_t loc, const char *format, ...);
CF_EXPORT char * _NS_index(const char *s, int c);

// These are replacements for POSIX calls on Windows, ensuring that the UTF8 parameters are converted to UTF16 before being passed to Windows
CF_EXPORT int _NS_stat(const char *name, struct _stat64i32 *st);
CF_EXPORT int _NS_mkdir(const char *name);
CF_EXPORT int _NS_rmdir(const char *name);
CF_EXPORT int _NS_chmod(const char *name, int mode);
CF_EXPORT int _NS_unlink(const char *name);
CF_EXPORT char *_NS_getcwd(char *dstbuf, size_t size);     // Warning: this doesn't support dstbuf as null even though 'getcwd' does
CF_EXPORT char *_NS_getenv(const char *name);
CF_EXPORT int _NS_rename(const char *oldName, const char *newName);
CF_EXPORT int _NS_open(const char *name, int oflag, int pmode = 0);
CF_EXPORT int _NS_chdir(const char *name);
CF_EXPORT int _NS_mkstemp(char *name, int bufSize);
CF_EXPORT int _NS_access(const char *name, int amode);

// Atomic functions
#define OSReadLittleInt(x, y)		OSReadLittleInt32(x, y)
#define OSWriteLittleInt(x, y, z)	OSWriteLittleInt32(x, y, z)
#define OSReadLittleInt32(base, byteOffset) OSReadSwapInt32(base, byteOffset)
#define OSWriteLittleInt32(base, byteOffset, data) OSWriteSwapInt32(base, byteOffset, data)

CF_EXPORT uint32_t OSReadSwapInt32(const volatile void *base, uintptr_t byteOffset);
CF_EXPORT void OSWriteSwapInt32(volatile void *base, uintptr_t byteOffset, uint32_t data);

CF_EXPORT bool OSAtomicTestAndSetBarrier( uint32_t __n, volatile void *__theAddress );
CF_EXPORT bool OSAtomicTestAndSet( uint32_t __n, volatile void *__theAddress );
CF_EXPORT bool OSAtomicTestAndClearBarrier( uint32_t __n, volatile void *__theAddress );
CF_EXPORT bool OSAtomicTestAndClear( uint32_t __n, volatile void *__theAddress );

CF_EXPORT bool OSAtomicCompareAndSwapPtr(void *oldp, void *newp, void *volatile *dst);
CF_EXPORT bool OSAtomicCompareAndSwapLong(long oldl, long newl, long volatile *dst);
CF_EXPORT bool OSAtomicCompareAndSwapPtrBarrier(void *oldp, void *newp, void *volatile *dst);

CF_EXPORT int32_t OSAtomicDecrement32Barrier(volatile int32_t *dst);
CF_EXPORT int32_t OSAtomicIncrement32Barrier(volatile int32_t *dst);
CF_EXPORT int32_t OSAtomicIncrement32(volatile int32_t *theValue);
CF_EXPORT int32_t OSAtomicDecrement32(volatile int32_t *theValue);

CF_EXPORT int32_t OSAtomicAdd32( int32_t theAmount, volatile int32_t *theValue );
CF_EXPORT int32_t OSAtomicAdd32Barrier( int32_t theAmount, volatile int32_t *theValue );
CF_EXPORT bool OSAtomicCompareAndSwap32Barrier( int32_t oldValue, int32_t newValue, volatile int32_t *theValue );

#if _WIN64
CF_EXPORT bool OSAtomicCompareAndSwap64( int64_t __oldValue, int64_t __newValue, volatile int64_t *__theValue );
CF_EXPORT bool OSAtomicCompareAndSwap64Barrier( int64_t __oldValue, int64_t __newValue, volatile int64_t *__theValue );

CF_EXPORT int64_t OSAtomicAdd64( int64_t __theAmount, volatile int64_t *__theValue );
CF_EXPORT int64_t OSAtomicAdd64Barrier( int64_t __theAmount, volatile int64_t *__theValue );
#endif

CF_EXPORT void OSMemoryBarrier();

// Spin locks
typedef int32_t CFSpinLock_t;

#define OSSpinLockLock(A) __CFSpinLock(A)
#define OSSpinLockUnlock(A) __CFSpinUnlock(A)
#define OSSpinLockTry(A) __CFSpinLockTry(A)

typedef int32_t OSSpinLock;
#define OS_SPINLOCK_INIT       0

// Use the OSSpinLock functions, these are the implementation details
CF_EXPORT void __CFSpinLock(volatile CFSpinLock_t *lock);
CF_EXPORT void __CFSpinUnlock(volatile CFSpinLock_t *lock);
CF_EXPORT Boolean __CFSpinLockTry(volatile CFSpinLock_t *lock);

// Math
CF_EXPORT int _NS_flsl( long mask );
CF_EXPORT int _NS_popcountll(long long x);

// Note: this section cannot define things that are defined in the Windows SDK or C library on Windows
#ifndef CF_DO_NOT_REDEFINE_FUNCTIONS

#define strlcat(a,b,c) _NS_strlcat(a,b,c)
#define strlcpy(a,b,c) _NS_strlcpy(a,b,c)
#define strtod_l(a,b,locale) _NS_strtod_l(a,b,locale)
#define strtoul_l(a,b,c,locale) _NS_strtoul_l(a,b,c,locale)
#define strtol_l(a,b,c,locale) _NS_strtol_l(a,b,c,locale)
#define strtoull_l(a,b,c,locale) _NS_strtoull_l(a,b,c,locale)
#define strtoll_l(a,b,c,locale) _NS_strtoll_l(a,b,c,locale)
#define strncasecmp_l(a, b, c, locale) _NS_strncasecmp_l(a, b, c, locale)
#define fprintf_l(a,locale,b,...) _NS_fprintf_l(a,locale,b, __VA_ARGS__)
#define snprintf_l(str,size,loc,fmt,...) _NS_snprintf_l(str,size,loc,fmt,__VA_ARGS__)
#define snprintf _snprintf
#define asprintf(a,b,...) _NS_asprintf(a,b,__VA_ARGS__)
#define asprintf_l(a,loc,fmt,...) _NS_asprintf_l(a,loc,fmt,__VA_ARGS__)
#define sscanf_l(a,loc,b,...) _NS_sscanf_l(a,loc,b,__VA_ARGS__)
#define vasprintf(a,b,c) _NS_vasprintf(a,b,c)
#define vasprintf_l(a,b,c,d) _NS_vasprintf_l(a,b,c,d)

#define flsl(mask) _NS_flsl(mask)
#define popcountll(x) _NS_popcountll(x)
#define index(x,y) _NS_index(x,y)

#endif

#endif // __CFPLATFORM_WINDOWS__
