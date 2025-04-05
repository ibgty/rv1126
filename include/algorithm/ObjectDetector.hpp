// #ifndef OBJECT_DETECTOR_HPP
// #define OBJECT_DETECTOR_HPP

// #include "rknn_api.h"
// #include "core/MediaException.hpp"
// #include <vector>
// #include <string>
// #include <memory>

// namespace algorithm {

// struct ModelConfig {
//     std::string modelPath;          // 模型路径
//     int inputWidth;                 // 模型输入宽度
//     int inputHeight;                // 模型输入高度
//     float confThreshold = 0.3;      // 置信度阈值
//     float nmsThreshold = 0.5;       // NMS阈值
//     int numClasses = 80;            // 类别数量
// };

// struct BBox {
//     int left;
//     int top;
//     int right;
//     int bottom;
//     float score;
//     int classId;
// };

// class ObjectDetector {
// public:
//     /*
//      * @brief 初始化目标检测器
//      * @param config 模型配置参数
//      * @throw MediaException 初始化失败时抛出
//      */
//     explicit ObjectDetector(const ModelConfig& config);
    
//     ~ObjectDetector();

//     // 禁用拷贝和赋值
//     ObjectDetector(const ObjectDetector&) = delete;
//     ObjectDetector& operator=(const ObjectDetector&) = delete;

//     struct DetectionResult {
//         std::vector<BBox> boxes;
//         std::vector<std::string> labels;
//     };

//      /*
//      * @brief 执行目标检测
//      * @param frame 输入视频帧
//      * @return 检测结果
//      * @throw MediaException 检测失败时抛出
//      */
//     DetectionResult detect(const MediaBuffer& frame);

// private:
//     void loadModel();
//     void initializeIOMemory();
//     void preprocess(const MediaBuffer& frame);
//     void postprocess(DetectionResult& result);
    
//     rknn_context m_ctx;
//     ModelConfig m_config;
//     rknn_input_output_num m_ioNum;
//     std::vector<rknn_tensor_attr> m_inputAttrs;
//     std::vector<rknn_tensor_attr> m_outputAttrs;
//     std::vector<rknn_tensor_mem*> m_inputMems;
//     std::vector<rknn_tensor_mem*> m_outputMems;
//     bool m_initialized = false;
// };

// } // namespace algorithm

// #endif // OBJECT_DETECTOR_HPP