#include " MediaException.hpp"
#include <iomanip>
#include <iostream>
#include<sstream>

using namespace std::chrono;

// 构造函数
MediaException::MediaException(ErrorType type, const std::string& context)
    : std::runtime_error(generateMessage(type, context)),
      m_type(type),
      m_context(context),
      m_timestamp(system_clock::now()) {
    logError();
}

// 访问器
MediaException::ErrorType MediaException::type() const noexcept {
    return m_type;
}

const char* MediaException::context() const noexcept {
    return m_context.c_str();
}

// 错误类型转字符串
const char* MediaException::errorTypeToString(ErrorType type) {
    switch(type) {
        case VI_INIT_FAILURE:    return "视频输入初始化失败";
        case RGA_INIT_FAILURE:   return "图像处理器初始化失败";
        case VO_INIT_FAILURE:    return "显示输出初始化失败";
        case RKNN_INIT_FAILURE:  return "神经网络初始化失败";
        case MODEL_LOAD_FAILURE: return "模型加载失败";
        case FRAME_ACQUIRE_FAILURE: return "帧获取失败";
        case BUFFER_ALLOC_FAILURE:  return "内存分配失败";
        case RUNTIME_PROCESSING:    return "通用处理错误";
        case THREAD_OPERATION:      return "线程操作错误";
        case DMA_OPERATION_FAILURE: return "DMA操作失败";
        case ISP_CONFIG_FAILURE:    return "ISP配置错误";
        case INVALID_PARAMETER:     return "无效参数";
        default:                    return "未知错误类型";
    }
}

// 生成错误信息
std::string MediaException::generateMessage(ErrorType type, const std::string& context) {
    std::ostringstream oss;
    oss << "[" << getCurrentTime() << "] "
        << errorTypeToString(type) 
        << " - " << context;
    return oss.str();
}

// 获取当前时间
std::string MediaException::getCurrentTime() {
    auto now = system_clock::to_time_t(m_timestamp);
    std::tm tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 记录错误日志
void MediaException::logError() {
    std::cout << "ERROR: " << what() << std::endl;
}