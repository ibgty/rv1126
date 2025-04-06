#ifndef APPLICATION_HPP
#define APPLICATION_HPP


// #include "im2d.h"
// #include "rga.h"
// #include "drm_func.h"
// #include "rga_func.h"
#include "Application.hpp"
#include "ObjectDetector.hpp"

#include <atomic>
#include <thread>
#include <memory>
#include<iostream>

class Application {
public:
    explicit Application();
    ~Application();

    void run();
    // void stop();

private:
    void initializePipeline();
    // void startThreads();
    // void stopThreads();


    // void processingLoop();
    // void renderingLoop();
    interface* hardware;
    ObjectDetector* detect;
    std::atomic_bool m_running{false};
    // AppConfig m_config;

    // std::unique_ptr<VideoInput> m_camera;
    // std::unique_ptr<RgaProcessor> m_Rga;
    // std::unique_ptr<VideoOutput> m_display;

    // std::unique_ptr<ViConfig> m_camera_config;
    // std::unique_ptr<RgaConfig> m_Rga_config;
    // std::unique_ptr<DisplayConfig> m_display_config;
    // std::unique_ptr<IspConfig> m_isp_config;
    // std::unique_ptr<algorithm::ObjectDetector> m_detector;

    // ThreadSafeQueue<MediaBuffer> m_rawFrameQueue;
    // ThreadSafeQueue<AnnotatedFrame> m_processedFrameQueue;

    // std::thread m_processingThread;
    // std::thread m_renderingThread;
};

#endif // APPLICATION_HPP