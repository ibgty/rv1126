#include "Application.hpp"
// #include <chrono>
#include <ratio>
#include<memory>
// using namespace std::chrono::literals;  // 正确写法

Application::Application() 
{
    try {
        initializePipeline();
    } catch (...) {
        // stop();
        throw;
    }
}

Application::~Application() {
  MPP_CHN_S stSrcChn;
  MPP_CHN_S stDestChn;
  printf("UnBind VI[0:0] to RGA[0:0]....\n");
  std::cout<<"UnBind VI[0:0] to RGA[0:0]..."<<std::endl;

  stSrcChn.enModId = RK_ID_VI;
  stSrcChn.s32DevId = m_camera_config->m_camId;
  stSrcChn.s32ChnId = 0;
  stDestChn.enModId = RK_ID_RGA;
  stSrcChn.s32DevId = m_camera_config->m_camId;
  stDestChn.s32ChnId = 0;
  RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  
  delete[] m_camera;
  delete[] m_Rga;
  delete[] m_display;
  if (m_isp_config->iq_file_dir) 
  {
#if RKAIQ
SAMPLE_COMM_ISP_Stop(m_isp_config->m_camId);
#endif
  }
  delete[] m_isp_config;
  delete[] m_camera_config;
  delete[] m_Rga_config;
  delete[] m_display_config;
    // stop();
}

void Application::run() {
    m_running = true;
    while(1);
    // startThreads();
}

// void Application::stop() {
//     if (m_running) {
//         m_running = false;
//         stopThreads();
//     }
// }

void Application::initializePipeline() {
    // m_isp_config=std::make_unique<IspConfig>();
    // m_camera_config=std::make_unique<ViConfig>();;
    // m_Rga_config=std::make_unique<RgaConfig>();;
    // m_display_config=std::make_unique<RgaConfig>();;
    m_isp_config=new IspConfig();
    m_camera_config=new ViConfig();
    m_Rga_config=new RgaConfig();
    m_display_config=new DisplayConfig();
    
    isp_init();
    RK_MPI_SYS_Init();
    // m_camera = std::make_unique<VideoInput>(m_camera_config);
    // m_processor = std::make_unique<ImageProcessor>(m_Rga_config);
    // m_display = std::make_unique<DisplayOutput>(m_display_config);
    m_camera=new VideoInput(m_camera_config);
    m_Rga=new RgaProcessor(m_Rga_config);
    m_display=new VideoOutput(m_display_config);
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    std::cout<<"Bind VI[0:0] to RGA[0:0]..."<<std::endl;
    // printf("Bind VI[0:0] to RGA[0:0]....\n");
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId =m_camera_config-> m_camId;
    stSrcChn.s32ChnId = m_camera_config->chn;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32DevId =  m_camera_config-> m_camId;
    stDestChn.s32ChnId = m_Rga_config->chn;
    RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    show_info();
}

void Application::show_info(){
  std::cout<<"VI CameraIdx:"<<m_camera_config->m_camId<<std::endl;
  std::cout<<"pDeviceName"<<m_camera_config->pDeviceName_01<<std::endl;
  std::cout<<"Resolution"<<m_camera_config->video_width<<"x"<<m_camera_config->video_height<<std::endl;
  std::cout<<"VO pcDevNode: "<<m_display_config->deviceNode<<std::endl;
  std::cout<<"Resolution"<<m_display_config->dispRect_width<<"X"<<m_display_config->dispRect_height<<std::endl;
}
void Application::isp_init()
{
  if (m_isp_config->iq_file_dir) 
  {
#ifdef RKAIQ
  // printf("#Rkaiq XML DirPath: %s\n", iq_file_dir);
  std::cout<<"#Rkaiq XML DirPath: "<<m_isp_config->iq_file_dir<<std::endl;
  SAMPLE_COMM_ISP_Init(m_isp_config->m_camId,m_isp_config->mode, RK_FALSE,m_isp_config->iq_file_dir);
  SAMPLE_COMM_ISP_Run(m_isp_config->m_camId);
  SAMPLE_COMM_ISP_SetFrameRate(m_isp_config->m_camId,m_isp_config->isp_frame);
#endif
  }
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