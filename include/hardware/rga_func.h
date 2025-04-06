#ifndef __RGA_FUNC_H__
#define __RGA_FUNC_H__


//#include <vector>

#ifdef __cplusplus
extern "C" {
#endif





int RGA_init(rga_context* rga_ctx);

// void img_resize_fast(rga_context *rga_ctx, int src_fd, int src_w, int src_h, uint64_t dst_phys, int dst_w, int dst_h);

// void img_resize_slow(rga_context *rga_ctx, void *src_virt, int src_w, int src_h, void *dst_virt, int dst_w, int dst_h);

void rga_resize(rga_context *rga_ctx, int src_fd, void *src_virt, int src_w, int src_h, int  dst_fd, void *dst_virt, int dst_w, int dst_h);

int RGA_deinit(rga_context* rga_ctx);


#ifdef __cplusplus
}
#endif
#endif/*__RGA_FUNC_H__*/
