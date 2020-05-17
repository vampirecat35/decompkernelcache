#include <sys/types.h>

extern size_t lzvn_encode_partial(void * dst, size_t dst_size, const void * src, size_t src_size, void * work_space, size_t work_size);

size_t lzvn_encode(void * dst, size_t dst_size, const void * src, size_t src_size, void * work_space)
{
    size_t ret = (size_t)work_space;
    ret = lzvn_encode_partial(dst, dst_size, src, src_size, work_space, ret);

    return ret;
}
