#include "RgaProcessor.hpp"
#include <sstream>
#include <iostream>
RgaProcessor::RgaProcessor(RgaConfig* m_config):m_config(m_config)
{
    try {
        validate_config();
        setup_rga_attributes();
        
        const int ret = RK_MPI_RGA_CreateChn(m_config->chn, &m_attr);
        if (ret != 0) {
            std::ostringstream oss;
            oss << "RGA channel creation failed. Error code: " << ret;
            throw MediaException(
                MediaException::RGA_INIT_FAILURE,
                oss.str()
            );
        }
        m_initialized = true;
    } catch (...) {
        if (m_initialized) {
            RK_MPI_RGA_DestroyChn(m_config->chn);
        }
        throw;
    }
}

RgaProcessor::~RgaProcessor() {
    if (m_initialized) {
        RK_MPI_RGA_DestroyChn(m_config->chn);
    }
}

void RgaProcessor::validate_config() const {
    if (m_config->input_width <= 0 || m_config->input_height <= 0) {
        throw MediaException(
            MediaException::INVALID_PARAMETER,
            "Invalid input resolution: " + 
            std::to_string(m_config->input_width) + "x" +
            std::to_string(m_config->input_height)
        );
    }
    
    // if (m_config->buffer_count < 2) {
    //     throw MediaException(
    //         MediaException::INVALID_PARAMETER,
    //         "Buffer count must >= 2. Got: " +
    //         std::to_string(m_config->buffer_count)
    //     );
    // }
}

void RgaProcessor::setup_rga_attributes() {
    memset(&m_attr, 0, sizeof(RGA_ATTR_S));
    
    // 输入配置
    m_attr.stImgIn.u32X = m_config->input_x;
    m_attr.stImgIn.u32Y = m_config->input_y;
    m_attr.stImgIn.u32Width = m_config->input_width;
    m_attr.stImgIn.u32Height = m_config->input_height;
    m_attr.stImgIn.imgType = m_config->input_format;
    m_attr.stImgIn.u32HorStride = m_config->input_HorStride;
    m_attr.stImgIn.u32VirStride = m_config->input_HorStride;

    // 输出配置
    m_attr.stImgOut.u32X = m_config->output_x;
    m_attr.stImgOut.u32Y = m_config->output_y;
    m_attr.stImgOut.u32Width = m_config->output_width;
    m_attr.stImgOut.u32Height = m_config->output_height;
    m_attr.stImgOut.imgType = m_config->output_format;
    m_attr.stImgOut.u32HorStride = m_config->output_HorStride;
    m_attr.stImgOut.u32VirStride = m_config->output_VirStride;

    // rga参数
    m_attr.bEnBufPool = m_config->EnBufPool;
    m_attr.u16BufPoolCnt = m_config->BufPoolCnt;
    m_attr.u16Rotaion = m_config->Rotaion;
}
int RgaProcessor::rga_resize_init()
{

    rga_ctx.rga_handle = dlopen("/usr/lib/librga.so", RTLD_LAZY);
    if (!rga_ctx.rga_handle)
    {
        // printf("dlopen /usr/lib/librga.so failed\n");
        std::cout<<"dlopen /usr/lib/librga.so failed"<<std::endl;
        return -1;
    }
    rga_ctx.init_func = (FUNC_RGA_INIT)dlsym(rga_ctx.rga_handle, "c_RkRgaInit");
    rga_ctx.deinit_func = (FUNC_RGA_DEINIT)dlsym(rga_ctx.rga_handle, "c_RkRgaDeInit");
    rga_ctx.blit_func = (FUNC_RGA_BLIT)dlsym(rga_ctx.rga_handle, "c_RkRgaBlit");
    rga_ctx.init_func();
        std::cout<<"rga_resize_init success"<<std::endl;
    return 0;
}
void RgaProcessor::rga_resize( int src_fd, void *src_virt, int src_w, int src_h, int  dst_fd, void *dst_virt, int dst_w, int dst_h)
{
    //  std::cout<<17<<std::endl; 
      if (rga_ctx.rga_handle)
    {
        int ret = 0;
        rga_info_t src, dst;

        memset(&src, 0, sizeof(rga_info_t));
        src.fd = src_fd;
        src.mmuFlag = 1;
        src.virAddr = (void *)src_virt;

        memset(&dst, 0, sizeof(rga_info_t));
        dst.fd = dst_fd;
        dst.mmuFlag = 1;
        dst.virAddr = dst_virt;
        dst.nn.nn_flag = 0;

        rga_set_rect(&src.rect, 0, 0, src_w, src_h, src_w, src_h, RK_FORMAT_RGB_888);
        rga_set_rect(&dst.rect, 0, 0, dst_w, dst_h, dst_w, dst_h, RK_FORMAT_RGB_888);

        ret = rga_ctx.blit_func(&src, &dst, NULL);
        if (ret)
        {
            // printf("c_RkRgaBlit error : %s\n", strerror(errno));
            std::cout<<"c_RkRgaBlit error :"<<strerror(errno)<<std::endl;
        }

        return;
    }
    return;
}
int RgaProcessor::rga_resize_deinit()
{
    if(rga_ctx.rga_handle)
    {
        dlclose(rga_ctx.rga_handle);
        rga_ctx.rga_handle = NULL;
    }
}
/*
MediaBuffer RgaProcessor::process(const MediaBuffer& input) {
    if (!m_initialized) {
        throw MediaException(
            MediaException::RGA_INIT_FAILURE,
            "Processor not initialized"
        );
    }

    // 发送输入帧
    int ret = RK_MPI_RGA_SendFrame(0, &input.frame_info, -1);
    if (ret != 0) {
        std::ostringstream oss;
        oss << "Failed to send frame to RGA. Error code: " << ret;
        throw MediaException(
            MediaException::RUNTIME_PROCESSING,
            oss.str()
        );
    }

    // 获取输出帧
    MediaBuffer output;
    ret = RK_MPI_RGA_GetFrame(0, &output.frame_info, -1);
    if (ret != 0) {
        std::ostringstream oss;
        oss << "Failed to get frame from RGA. Error code: " << ret;
        throw MediaException(
            MediaException::RUNTIME_PROCESSING,
            oss.str()
        );
    }

    return output;
}
*/