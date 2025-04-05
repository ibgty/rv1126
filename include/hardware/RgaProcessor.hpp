#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include "rkmedia_api.h"
#include " MediaException.hpp"
#include <memory>
#include "config.hpp"


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
    
    RgaConfig* m_config;
    // RgaConfig m_config;
    RGA_ATTR_S m_attr;
    bool m_initialized = false;
};



#endif // IMAGE_PROCESSOR_HPP