/*	CFPThread.h
	Copyright (c) 2012, Apple Inc. All rights reserved.
	Responsibility: Tony Parker
*/

#if !defined(__COREFOUNDATION_CFPTHREAD__)
#define __COREFOUNDATION_CFPTHREAD__ 1

#include <CoreFoundation/CFBase.h>

#if DEPLOYMENT_TARGET_WINDOWS

#define PTHREAD_MUTEX_NORMAL		0
#define PTHREAD_MUTEX_ERRORCHECK	1
#define PTHREAD_MUTEX_RECURSIVE		2
#define PTHREAD_MUTEX_DEFAULT		PTHREAD_MUTEX_NORMAL

#ifndef HAVE_STRUCT_TIMESPEC
#define HAVE_STRUCT_TIMESPEC 1
struct timespec { long tv_sec; long tv_nsec; };
#endif

typedef void* pthread_t;

typedef CRITICAL_SECTION pthread_mutex_t;

typedef struct {
    HANDLE events[2];    // event 1 = signal event, event 2 = broadcast event
    CFIndex numberOfWaiters;     // number of waiters
} pthread_cond_t;

// we don't use this
typedef int pthread_condattr_t;

// This is a stub. We don't actually implement any attributes.
typedef int pthread_mutexattr_t;

// This is a stub.
typedef int pthread_attr_t;

CF_EXPORT pthread_t _NS_pthread_self();
CF_EXPORT int _NS_pthread_main_np();
CF_EXPORT int _NS_pthread_equal(pthread_t t1, pthread_t t2);
CF_EXPORT int _NS_pthread_key_init_np(int key, void (*destructor)(void *));
CF_EXPORT void _NS_pthread_setname_np(const char *name);

CF_EXPORT int _NS_pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
CF_EXPORT int _NS_pthread_mutex_destroy(pthread_mutex_t *);

CF_EXPORT int _NS_pthread_mutex_lock(pthread_mutex_t *);
CF_EXPORT int _NS_pthread_mutex_trylock(pthread_mutex_t *);
CF_EXPORT int _NS_pthread_mutex_unlock(pthread_mutex_t *);

CF_EXPORT int _NS_pthread_mutexattr_init(pthread_mutexattr_t *);
CF_EXPORT int _NS_pthread_mutexattr_settype(pthread_mutexattr_t *, int);
CF_EXPORT int _NS_pthread_mutexattr_destroy(pthread_mutexattr_t *);

CF_EXPORT int _NS_pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *);
CF_EXPORT int _NS_pthread_cond_destroy(pthread_cond_t *cond);
CF_EXPORT int _NS_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *external_mutex);
CF_EXPORT int _NS_pthread_cond_timedwait_relative_np(pthread_cond_t *cond, pthread_mutex_t *external_mutex, const struct timespec *ts);
CF_EXPORT int _NS_pthread_cond_signal(pthread_cond_t *cond);
CF_EXPORT int _NS_pthread_cond_broadcast(pthread_cond_t *cond);

// map use of pthread APIs to internal API
#define pthread_self _NS_pthread_self
#define pthread_key_init_np _NS_pthread_key_init_np
#define pthread_equal _NS_pthread_equal
#define pthread_main_np _NS_pthread_main_np
#define pthread_setname_np _NS_pthread_setname_np

#define pthread_mutex_init _NS_pthread_mutex_init
#define pthread_mutex_destroy _NS_pthread_mutex_destroy
#define pthread_mutex_lock _NS_pthread_mutex_lock
#define pthread_mutex_trylock _NS_pthread_mutex_trylock
#define pthread_mutex_unlock _NS_pthread_mutex_unlock

#define pthread_mutexattr_init _NS_pthread_mutexattr_init
#define pthread_mutexattr_settype _NS_pthread_mutexattr_settype
#define pthread_mutexattr_destroy _NS_pthread_mutexattr_destroy

#define pthread_cond_init _NS_pthread_cond_init
#define pthread_cond_destroy _NS_pthread_cond_destroy
#define pthread_cond_wait _NS_pthread_cond_wait
#define pthread_cond_timedwait_relative_np _NS_pthread_cond_timedwait_relative_np
#define pthread_cond_signal _NS_pthread_cond_signal
#define pthread_cond_broadcast _NS_pthread_cond_broadcast

// we devolve rwlocks into regular mutexes
typedef pthread_mutex_t pthread_rwlock_t;

#define pthread_rwlock_init _NS_pthread_mutex_init
#define pthread_rwlock_wrlock _NS_pthread_mutex_lock
#define pthread_rwlock_rdlock _NS_pthread_mutex_lock
#define pthread_rwlock_unlock _NS_pthread_mutex_unlock

#endif

#endif
