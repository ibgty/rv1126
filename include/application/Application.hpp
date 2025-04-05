#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "VideoInput.hpp"
#include "RgaProcessor.hpp"
#include "VideoOutput.hpp"
#include "rkmedia_api.h"
// #include "ObjectDetector.hpp"
// #include "ThreadSafeQueue.hpp"
#include " MediaException.hpp"
#include "sample_common.h"
#include "config.hpp"
// #include "im2d.h"
// #include "rga.h"
// #include "drm_func.h"
// #include "rga_func.h"

#include <atomic>
#include <thread>
#include <memory>
#include<iostream>

// struct AppConfig {
//     struct {
//         int cameraId = 0;
//         std::string deviceName = "rkispp_scale0";
//         int width = 1920;
//         int height = 1080;
//     } capture;

//     struct {
//         int inputWidth = 640;
//         int inputHeight = 640;
//         int outputWidth = 1920;
//         int outputHeight = 1080;
//     } processing;

//     // struct {
//     //     std::string modelPath = "yolov5s.rknn";
//     //     float confThreshold = 0.5;
//     //     float nmsThreshold = 0.6;
//     // } detection;

// //     struct {
// //         std::string deviceNode = "/dev/dri/card0";
// //         int displayWidth = 1920;
// //         int displayHeight = 1080;
// //     } display;
// // };

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
    
    void show_info();
    void isp_init();
    std::atomic_bool m_running{false};
    // AppConfig m_config;

    // std::unique_ptr<VideoInput> m_camera;
    // std::unique_ptr<RgaProcessor> m_Rga;
    // std::unique_ptr<VideoOutput> m_display;
    VideoInput* m_camera;
    RgaProcessor* m_Rga;
    VideoOutput* m_display; 

    ViConfig* m_camera_config;
    RgaConfig* m_Rga_config;
    DisplayConfig* m_display_config;
    IspConfig* m_isp_config;
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