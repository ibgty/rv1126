#include "VideoOutput.hpp"
#include <sstream>

VideoOutput::VideoOutput(DisplayConfig*m_config):m_config(m_config)
{
    try {
        validateConfig();
        initializeVoChannel();
        m_initialized = true;
    } catch (...) {
        if (m_initialized) {
            RK_MPI_VO_DestroyChn(m_config->chn);
        }
        throw;
    }
}

VideoOutput::~VideoOutput() {
    if (m_initialized) {
        RK_MPI_VO_DestroyChn(m_config->chn);
    }
}

void VideoOutput::validateConfig() const {
    // const auto& [w, h] = std::tie(m_config->dispRect_width, m_config->dispRect_height);
    auto w=m_config->dispRect_width;
    auto h=m_config->dispRect_height;
    
    if (w <= 0 || h <= 0) {
        throw MediaException(
            MediaException::INVALID_PARAMETER,
            "Invalid display resolution: " + 
            std::to_string(w) + "x" + std::to_string(h)
        );
    }

    if (m_config->dispRect_width <= 0 || m_config->dispRect_height <= 0) {
        throw MediaException(
            MediaException::INVALID_PARAMETER,
            "Invalid image rectangle"
        );
    }
}

void VideoOutput::initializeVoChannel() {
    VO_CHN_ATTR_S attr = {0};
    
    // 基础配置
    attr.pcDevNode = m_config->deviceNode;
    attr.emPlaneType = m_config->planeType;
    attr.enImgType = m_config->imgType;
    attr.u16Zpos = m_config->zpos;

    // 源图像区域
    attr.stImgRect.s32X = m_config->imgRect_x;
    attr.stImgRect.s32Y = m_config->imgRect_y;
    attr.stImgRect.u32Width = m_config->imgRect_width;
    attr.stImgRect.u32Height = m_config->imgRect_height;

    // 显示区域
    attr.stDispRect.s32X = m_config->dispRect_x;
    attr.stDispRect.s32Y = m_config->dispRect_y;
    attr.stDispRect.u32Width = m_config->dispRect_width;
    attr.stDispRect.u32Height = m_config->dispRect_height;

    const int ret = RK_MPI_VO_CreateChn(m_config->chn, &attr);
    if (ret != 0) {
        std::ostringstream oss;
        oss << "VO channel creation failed. Error code: " << ret 
            << " Device: " << m_config->deviceNode
            << " Display: " << m_config->dispRect_width << "x" << m_config->dispRect_height;
        throw MediaException(
            MediaException::VO_INIT_FAILURE,
            oss.str()
        );
    }
}

// void VideoOutput::render(const MediaBuffer& buffer) {
//     if (!m_initialized) {
//         throw MediaException(
//             MediaException::VO_INIT_FAILURE,
//             "Display channel not initialized"
//         );
//     }

//     const int ret = RK_MPI_VO_SendFrame(0, &buffer.frame_info, -1);
//     if (ret != 0) {
//         std::ostringstream oss;
//         oss << "Frame rendering failed. Error code: " << ret;
//         throw MediaException(
//             MediaException::RUNTIME_PROCESSING,
//             oss.str()
//         );
//     }
// }