#ifndef MODEL_CONFIG_HPP
#define MODEL_CONFIG_HPP
#include "rknn_api.h"
struct ModelConfig {
    std::string modelPath="/demo/bin/pre_yolov5s_face.rknn";          // 模型路径
    int inputWidth=0;                 // 模型输入宽度
    int inputHeight=0;                // 模型输入高度
    float confThreshold = 0.3;      // 置信度阈值
    float nmsThreshold = 0.5;       // NMS阈值
    // int numClasses = 1;            // 类别数量
    float visThreshold=0.1f;
};

#endif