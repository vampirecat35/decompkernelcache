/*
 * Created..: 31 October 2014
 * Filename.: lzvn.h
 * Author...: Pike R. Alpha, Minuszwei, Andy Vandijck
 * Purpose..: LZVN compression library.
 */

#include <stdint.h>
#include <sys/types.h>

extern size_t lzvn_encode(void * dst, size_t dst_size, const void * src, size_t src_size, void * work_space);
extern size_t lzvn_decode(void * dst, size_t dst_size, const void * src, size_t src_size);
extern size_t lzvn_encode_work_size(void);
