// #include "ObjectDetector.hpp"
// #include "rknn_api.h"
// #include "post_process.h"
// #include <fstream>
// #include <sstream>

// using namespace algorithm;
// using namespace core::exceptions;

// namespace {
//     // 辅助函数：加载模型文件
//     std::vector<uint8_t> loadModelFile(const std::string& path) {
//         std::ifstream file(path, std::ios::binary);
//         if (!file) {
//             throw MediaException(
//                 MediaException::MODEL_LOAD_FAILURE,
//                 "Cannot open model file: " + path
//             );
//         }
        
//         file.seekg(0, std::ios::end);
//         const size_t size = file.tellg();
//         file.seekg(0, std::ios::beg);
        
//         std::vector<uint8_t> modelData(size);
//         if (!file.read(reinterpret_cast<char*>(modelData.data()), size)) {
//             throw MediaException(
//                 MediaException::MODEL_LOAD_FAILURE,
//                 "Failed to read model file: " + path
//             );
//         }
//         return modelData;
//     }
// }

// ObjectDetector::ObjectDetector(const ModelConfig& config)
//     : m_config(config)
// {
//     try {
//         loadModel();
//         initializeIOMemory();
//         m_initialized = true;
//     } catch (...) {
//         if (m_initialized) cleanup();
//         throw;
//     }
// }

// ObjectDetector::~ObjectDetector() {
//     if (m_initialized) cleanup();
// }

// void ObjectDetector::loadModel() {
//     // 加载模型文件
//     auto modelData = loadModelFile(m_config.modelPath);
    
//     // 初始化RKNN上下文
//     const int ret = rknn_init(&m_ctx, 
//                             modelData.data(), 
//                             modelData.size(), 
//                             0);
//     if (ret != RKNN_SUCC) {
//         throw MediaException(
//             MediaException::RKNN_INIT_FAILURE,
//             "rknn_init failed. Error code: " + std::to_string(ret)
//         );
//     }

//     // 获取输入输出信息
//     rknn_query(m_ctx, RKNN_QUERY_IN_OUT_NUM, &m_ioNum, sizeof(m_ioNum));
    
//     // 查询输入输出属性
//     m_inputAttrs.resize(m_ioNum.n_input);
//     m_outputAttrs.resize(m_ioNum.n_output);
    
//     for (int i = 0; i < m_ioNum.n_input; ++i) {
//         m_inputAttrs[i].index = i;
//         rknn_query(m_ctx, RKNN_QUERY_INPUT_ATTR, &m_inputAttrs[i], sizeof(rknn_tensor_attr));
//     }
    
//     for (int i = 0; i < m_ioNum.n_output; ++i) {
//         m_outputAttrs[i].index = i;
//         rknn_query(m_ctx, RKNN_QUERY_OUTPUT_ATTR, &m_outputAttrs[i], sizeof(rknn_tensor_attr));
//     }
// }

// void ObjectDetector::initializeIOMemory() {
//     // 初始化输入内存
//     for (int i = 0; i < m_ioNum.n_input; ++i) {
//         m_inputMems.emplace_back(rknn_create_mem(m_ctx, m_inputAttrs[i].size));
//         if (!m_inputMems.back()) {
//             throw MediaException(
//                 MediaException::BUFFER_ALLOC_FAILURE,
//                 "Failed to allocate input memory for index " + std::to_string(i)
//             );
//         }
//         rknn_set_io_mem(m_ctx, m_inputMems[i], &m_inputAttrs[i]);
//     }

//     // 初始化输出内存
//     for (int i = 0; i < m_ioNum.n_output; ++i) {
//         m_outputMems.emplace_back(rknn_create_mem(m_ctx, m_outputAttrs[i].size));
//         if (!m_outputMems.back()) {
//             throw MediaException(
//                 MediaException::BUFFER_ALLOC_FAILURE,
//                 "Failed to allocate output memory for index " + std::to_string(i)
//             );
//         }
//         rknn_set_io_mem(m_ctx, m_outputMems[i], &m_outputAttrs[i]);
//     }
// }

// ObjectDetector::DetectionResult ObjectDetector::detect(const MediaBuffer& frame) {
//     if (!m_initialized) {
//         throw MediaException(
//             MediaException::RKNN_INIT_FAILURE,
//             "Detector not initialized"
//         );
//     }

//     DetectionResult result;
    
//     try {
//         // 预处理
//         preprocess(frame);
        
//         // 执行推理
//         const int ret = rknn_run(m_ctx, nullptr);
//         if (ret != RKNN_SUCC) {
//             throw MediaException(
//                 MediaException::RUNTIME_PROCESSING,
//                 "rknn_run failed. Error code: " + std::to_string(ret)
//             );
//         }
        
//         // 后处理
//         postprocess(result);
//     } catch (...) {
//         // 清空结果避免部分数据残留
//         result.boxes.clear();
//         result.labels.clear();
//         throw;
//     }
    
//     return result;
// }

// void ObjectDetector::preprocess(const MediaBuffer& frame) {
//     // 假设frame已经是RGB888格式且尺寸匹配
//     // 实际应添加格式转换和缩放逻辑
    
//     if (frame.width != m_config.inputWidth || 
//         frame.height != m_config.inputHeight) 
//     {
//         throw MediaException(
//             MediaException::INVALID_PARAMETER,
//             "Input size mismatch. Expected: " + 
//             std::to_string(m_config.inputWidth) + "x" + 
//             std::to_string(m_config.inputHeight) +
//             " Got: " + 
//             std::to_string(frame.width) + "x" + 
//             std::to_string(frame.height)
//         );
//     }
    
//     // 零拷贝: 直接使用输入内存
//     const int inputIndex = 0; // 假设单输入模型
//     memcpy(m_inputMems[inputIndex]->virt_addr, 
//           RK_MPI_MB_GetPtr(frame), 
//           m_inputAttrs[inputIndex].size);
// }

// void ObjectDetector::postprocess(DetectionResult& result) {
//     // 准备输出参数
//     std::vector<float> outScales;
//     std::vector<uint8_t> outZps;
    
//     for (const auto& attr : m_outputAttrs) {
//         outScales.push_back(attr.scale);
//         outZps.push_back(attr.zp);
//     }

//     // 调用后处理函数
//     detect_result_group_t detectResult;
//     post_process(
//         reinterpret_cast<uint8_t*>(m_outputMems[0]->virt_addr),
//         reinterpret_cast<uint8_t*>(m_outputMems[1]->virt_addr),
//         reinterpret_cast<uint8_t*>(m_outputMems[2]->virt_addr),
//         m_config.inputHeight,
//         m_config.inputWidth,
//         m_config.confThreshold,
//         m_config.nmsThreshold,
//         0.1f, // visThreshold
//         (float)m_config.inputWidth / frame.width,  // 实际应传入原始帧尺寸
//         (float)m_config.inputHeight / frame.height,
//         outZps,
//         outScales,
//         &detectResult
//     );

//     // 转换结果格式
//     for (int i = 0; i < detectResult.count; ++i) {
//         const auto& det = detectResult.results[i];
//         result.boxes.emplace_back(BBox{
//             det.box.left,
//             det.box.top,
//             det.box.right,
//             det.box.bottom,
//             det.prop,
//             det.classId
//         });
        
//         // 类别标签映射（示例）
//         result.labels.emplace_back(std::to_string(det.classId));
//     }
// }

// void ObjectDetector::cleanup() {
//     // 释放输入内存
//     for (auto& mem : m_inputMems) {
//         rknn_destroy_mem(m_ctx, mem);
//     }
    
//     // 释放输出内存
//     for (auto& mem : m_outputMems) {
//         rknn_destroy_mem(m_ctx, mem);
//     }
    
//     // 销毁RKNN上下文
//     if (m_ctx) {
//         rknn_destroy(m_ctx);
//         m_ctx = nullptr;
//     }
// }