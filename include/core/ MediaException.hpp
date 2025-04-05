#ifndef __MEDIAEXCEPTION_HPP__
#define __MEDIAEXCEPTION_HPP__
#include <stdexcept>
#include <string>
#include <chrono>

class MediaException : public std::runtime_error {
public:
    enum ErrorType {
        VI_INIT_FAILURE,       // 视频输入初始化失败
        RGA_INIT_FAILURE,      // 图像处理器初始化失败
        VO_INIT_FAILURE,       // 显示输出初始化失败
        RKNN_INIT_FAILURE,     // 神经网络初始化失败
        MODEL_LOAD_FAILURE,    // 模型加载失败
        FRAME_ACQUIRE_FAILURE, // 帧获取失败
        BUFFER_ALLOC_FAILURE,  // 内存分配失败
        RUNTIME_PROCESSING,    // 通用处理错误
        THREAD_OPERATION,      // 线程操作错误
        DMA_OPERATION_FAILURE, // DMA操作失败
        ISP_CONFIG_FAILURE,    // ISP配置错误
        INVALID_PARAMETER      // 无效参数
    };

    // 构造函数
    explicit MediaException(ErrorType type, const std::string& context);

    // 访问器
    ErrorType type() const noexcept;
    const char* context() const noexcept;

    // 静态工具函数
    static const char* errorTypeToString(ErrorType type);

private:
    ErrorType m_type;
    std::string m_context;
    std::chrono::system_clock::time_point m_timestamp;

    // 内部辅助函数（声明）
    std::string generateMessage(ErrorType type, const std::string& context);
    std::string getCurrentTime();
    void logError();
};

#endif // MEDIAEXCEPTION_HPP