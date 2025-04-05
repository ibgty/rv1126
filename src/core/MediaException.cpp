#include " MediaException.hpp"
#include <iomanip>
#include <iostream>
#include<sstream>

// 构造函数
MediaException::MediaException(ErrorType type, const std::string& context)
    : std::runtime_error(generateMessage(type, context)),
      m_type(type),
      m_context(context),
      m_timestamp(std::chrono::system_clock::now()) {
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
    static const char* typeStrings[] = {
        "VI_INIT_FAILURE", "RGA_INIT_FAILURE", "VO_INIT_FAILURE",
        "RKNN_INIT_FAILURE", "MODEL_LOAD_FAILURE", "FRAME_ACQUIRE_FAILURE",
        "BUFFER_ALLOC_FAILURE", "RUNTIME_PROCESSING", "THREAD_OPERATION",
        "DMA_OPERATION_FAILURE", "ISP_CONFIG_FAILURE", "INVALID_PARAMETER"
    };
    return typeStrings[static_cast<int>(type)];
}

// 生成错误信息
std::string MediaException::generateMessage(ErrorType type, const std::string& context) {
    std::ostringstream oss;
    oss << "[MediaSystem] "
        << "Type: " << errorTypeToString(type) << " | "
        << "Time: " << getCurrentTime() << " | "
        << "Context: " << context;
    return oss.str();
}

// 获取当前时间
std::string MediaException::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_r(&in_time_t, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 记录错误日志
void MediaException::logError() {
    std::cout << "ERROR: " << what() << std::endl;
}