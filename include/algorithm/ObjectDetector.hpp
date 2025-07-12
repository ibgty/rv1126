#ifndef OBJECT_DETECTOR_HPP
#define OBJECT_DETECTOR_HPP

#include "rknn_api.h"
#include " MediaException.hpp"
#include "model_config.hpp"
#include <vector>
#include <string>
#include <memory>
#include "interface.hpp"
#include "postprocess.h"
#include "rkmedia_api.h"
#include "model_config.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class ObjectDetector {
public:
    /*
     * @brief 初始化目标检测器
     * @param config 模型配置参数
     * @throw MediaException 初始化失败时抛出
     */


    // 禁用拷贝和赋值

    static ObjectDetector* getInstance() {
        static ObjectDetector instance;  // C++11保证局部静态变量初始化线程安全
        return &instance;
    }

     /*
     * @brief 执行目标检测
     * @param frame 输入视频帧
     * @return 检测结果
     * @throw MediaException 检测失败时抛出
     */
    void detect();
    void preprocess(int index);
    void postprocess(int index);
private:
    explicit ObjectDetector();
   ~ObjectDetector();
    ObjectDetector(const ObjectDetector&) = delete;
    ObjectDetector operator=(const ObjectDetector&) = delete;
    void loadModel();

    
    void cleanup();
  
    // void get_model_width_height(int *width,int *height);
    void get_frame(int index);
    void initializeIOMemory();
    void print_tensor(rknn_tensor_attr*attr);
    void get_model_width_height();
    

    rknn_context m_ctx;
    ModelConfig m_config;
    rknn_input_output_num m_ioNum;
    std::vector<rknn_tensor_attr> m_inputAttrs;
    std::vector<rknn_tensor_attr> m_outputAttrs;
    std::vector<rknn_tensor_mem*> m_inputMems;
    std::vector<rknn_tensor_mem*> m_outputMems;
     MEDIA_BUFFER src_mb;
    bool m_initialized = false;
    interface* m_interface;
    detect_result_group_t result;


    std::mutex M_Processing_Mutex;
    std::mutex M_Detect_Mutex;
    std::mutex M_PostPocess_Mutex;
    std::condition_variable Pre_Process_Cond, Detect_Cond,Post_Process_Cond;


    std::queue<MEDIA_BUFFER> input_buffer;
    std::queue<detect_result_group_t> inference_result;
    int detect_flag;
    int  output_flag;
    int num_buffer;

};


#endif // OBJECT_DETECTOR_HPP