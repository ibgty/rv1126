#ifndef VIDEO_OUTPUT_HPP
#define VIDEO_OUTPUT_HPP

#include <string>
#include "rkmedia_api.h"
#include " MediaException.hpp"
#include "config.hpp"


class VideoOutput {
public:
     /*
     * @brief 创建视频输出通道
     * @param config 显示配置参数
     * @throw MediaException 初始化失败时抛出
     */
    explicit VideoOutput(DisplayConfig*m_config);
    
    ~VideoOutput();

    // 禁用拷贝和赋值
    VideoOutput(const VideoOutput&) = delete;
    VideoOutput& operator=(const VideoOutput&) = delete;

     /*
     * @brief 渲染视频帧
     * @param buffer 要渲染的媒体缓冲区
     * @throw MediaException 渲染失败时抛出
     */
    // void render(const MediaBuffer& buffer);

private:
    void validateConfig() const;
    void initializeVoChannel();
    
    DisplayConfig*m_config; 
    // const DisplayConfig m_config;
    bool m_initialized = false;
};





#endif // VIDEO_OUTPUT_HPP