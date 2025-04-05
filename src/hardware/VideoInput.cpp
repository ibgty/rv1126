#include "VideoInput.hpp"
#include <sstream>
// #include "MediaException.hpp"


VideoInput::VideoInput(ViConfig* m_config) :m_config(m_config) 
{
    try {
        initialize();
        m_initialized = true;
    } catch (...) {
        if (m_initialized) {
            RK_MPI_VI_DisableChn(m_config->m_camId, m_config->chn);
        }
        throw;
    }
}

VideoInput::~VideoInput() {
    if (m_initialized) {
        RK_MPI_VI_DisableChn(m_config->m_camId, m_config->chn);
    }
}

void VideoInput::initialize() {
    VI_CHN_ATTR_S attr;
    memset(&attr,m_config->chn, sizeof(attr));
    
    // 配置设备参数
    attr.pcVideoNode =m_config->pDeviceName_01;  // 转换为C风格字符串
    attr.u32BufCnt = m_config->u32BufCnt;
    attr.u32Width = m_config->video_width;
    attr.u32Height = m_config->video_height;
    attr.enPixFmt = m_config->image_type;
    attr.enBufType = m_config->vi_buf_type;
    attr.enWorkMode = m_config->vi_work_mode;

    // 设置通道属性
    int ret = RK_MPI_VI_SetChnAttr(m_config->m_camId, m_config->chn, &attr);
    if (ret != 0) {
        std::ostringstream oss;
        oss << "VI SetChnAttr failed: camId=" << m_config->m_camId
            << " dev=" << m_config->pDeviceName_01 
            << " res=" << m_config->video_width << "x" << m_config->video_height
            << " errcode=" << ret;
        throw MediaException(
            MediaException::VI_INIT_FAILURE,
            oss.str()
        );
    }

    // 启用通道
    ret = RK_MPI_VI_EnableChn(m_config->m_camId, m_config->chn);
    if (ret != 0) {
        std::ostringstream oss;
        oss << "VI EnableChn failed: camId=" << m_config->m_camId
            << " errcode=" << ret;
        throw MediaException(
            MediaException::VI_INIT_FAILURE,
            oss.str()
        );
    }
}