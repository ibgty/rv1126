#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include "rkmedia_api.h"
#include " MediaException.hpp"
#include <memory>
#include "config.hpp"
#include <dlfcn.h> 
#include "RgaApi.h"
// #include "rga.h"
#include "im2d.h"

typedef int(* FUNC_RGA_INIT)();
typedef void(* FUNC_RGA_DEINIT)();
typedef int(* FUNC_RGA_BLIT)(rga_info_t *, rga_info_t *, rga_info_t *);


typedef struct _rga_context{
    void *rga_handle;
    FUNC_RGA_INIT init_func;
    FUNC_RGA_DEINIT deinit_func;
    FUNC_RGA_BLIT blit_func;
} rga_context;


class RgaProcessor {
public:
    /*
     * @brief 初始化RGA图像处理器
     * @param config 图像处理配置参数
     * @throw MediaException 初始化失败时抛出
     */
    explicit RgaProcessor(RgaConfig* m_config);
    
    ~RgaProcessor();

    // 禁用拷贝和赋值
    RgaProcessor(const RgaProcessor&) = delete;
    RgaProcessor& operator=(const RgaProcessor&) = delete;
    int rga_resize_init();
    void rga_resize( int src_fd, void *src_virt, int src_w, int src_h, int  dst_fd, void *dst_virt, int dst_w, int dst_h);
    void letter_box_resize( int src_fd, void *src_virt, int src_w, int src_h, int  dst_fd, void *dst_virt, int dst_w, int dst_h);
    int rga_resize_deinit();
    rga_context* get_rga_context()
    {
      return &rga_ctx; 
    }
    RK_U32 get_rga_width()
    {
      return m_config->input_width;
    }
    RK_U32 get_rga_height()
    {
      return m_config->input_height;
    }
    RgaConfig *get_rga_config()
    {
      return m_config;
    }
     /*
     * @brief 处理图像数据
     * @param input 输入媒体缓冲区
     * @return 处理后的媒体缓冲区
     * @throw MediaException 处理失败时抛出
     */
    // MediaBuffer process(const MediaBuffer& input);

private:
    void validate_config() const;
    void setup_rga_attributes();

    rga_context rga_ctx; 
    RgaConfig* m_config;
    // RgaConfig m_config;
    RGA_ATTR_S m_attr;
    bool m_initialized = false;
    
};



#endif // IMAGE_PROCESSOR_HPP