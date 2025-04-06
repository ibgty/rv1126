#include "Application.hpp"
// #include <chrono>
#include <ratio>
#include<memory>
#include<iostream>
// using namespace std::chrono::literals;  // 正确写法

Application::Application() 
{
    try {
        initializePipeline();
        std::cout<<4<<std::endl;
    } catch (...) {
        // stop();
        throw;
    }
}

Application::~Application() {

    // stop();
    // delete[]hardware;
}

void Application::run() {
    m_running = true;
    while(1)
    {
      std::cout<<0<<std::endl;
      detect->preprocess(0);
      std::cout<<1<<std::endl;
      detect->detect();
      std::cout<<2<<std::endl;
      detect->postprocess(0);
      std::cout<<3<<std::endl;
    }
    // startThreads();
}

// void Application::stop() {
//     if (m_running) {
//         m_running = false;
//         stopThreads();
//     }
// }

void Application::initializePipeline() {
  std::cout<<5<<std::endl;
   hardware=interface::getInstance();
  std::cout<<6<<std::endl;
   detect=ObjectDetector::getInstance();
  std::cout<<7<<std::endl;
}



// void Application::startThreads() {
//     m_processingThread = std::thread(&Application::processingLoop, this);
//     m_renderingThread = std::thread(&Application::renderingLoop, this);
// }

// void Application::stopThreads() {
//     if (m_processingThread.joinable()) {
//         m_processingThread.join();
//     }
//     if (m_renderingThread.joinable()) {
//         m_renderingThread.join();
//     }
// }

// void Application::processingLoop() {
//     using clock = std::chrono::high_resolution_clock;
    
//     while (m_running) {
//         try {
//             // 获取原始帧
//             auto frame = m_camera->captureFrame(100ms); // 带超时的帧捕获
            
//             // 图像预处理
//             auto processed = m_processor->process(frame);
            
//             // 执行目标检测
//             auto result = m_detector->detect(processed);
            
//             // 打包处理结果
//             AnnotatedFrame annotated{processed, result};
            
//             // 推送到渲染队列
//             m_processedFrameQueue.push(std::move(annotated));
            
//         } catch (const core::exceptions::MediaException& e) {
//             if (e.type() == core::exceptions::MediaException::FRAME_ACQUIRE_TIMEOUT) {
//                 continue; // 忽略超时错误
//             }
//             throw;
//         }
//     }
// }

// void Application::renderingLoop() {
//     while (m_running) {
//         try {
//             // 获取带标注的帧
//             AnnotatedFrame annotated;
//             if (m_processedFrameQueue.try_pop(annotated, 50ms)) {
//                 // 渲染到显示设备
//                 m_display->render(annotated.frame);
                
//                 // 更新性能统计
//                 fpsCounter.update();
//             }
            
//             // 显示性能信息
//             if (fpsCounter.elapsed() > 1s) {
//                 drawPerformanceOverlay(annotated.frame, fpsCounter.fps());
//                 fpsCounter.reset();
//             }
            
//         } catch (const core::exceptions::MediaException& e) {
//             if (m_running) {
//                 // 记录非正常错误
//                 errorHandler.log(e);
//             }
//         }
//     }
// }