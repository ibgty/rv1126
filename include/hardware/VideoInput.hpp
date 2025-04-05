#ifndef VIDEO_INPUT_HPP
#define VIDEO_INPUT_HPP

#include <string>
#include <stdexcept>
#include "rkmedia_api.h"
#include " MediaException.hpp"
#include "config.hpp"
#include <iostream>
#include <memory>

class VideoInput {
public:
    /*​
     * @brief 构造函数初始化视频输入通道
     * @throw MediaException 初始化失败时抛出
     */
    VideoInput(ViConfig* m_config);

    ~VideoInput();

    // 禁用拷贝和赋值
    VideoInput(const VideoInput&) = delete;
    VideoInput& operator=(const VideoInput&) = delete;

private:
    void initialize();
    ViConfig* m_config;
    // ViConfig m_config;
    bool m_initialized = false;
};
#endif // VIDEO_INPUT_HPP