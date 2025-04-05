#include "RgaProcessor.hpp"
#include <sstream>

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