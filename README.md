- [基于瑞芯微RV1126的yolov5s部署](#基于瑞芯微rv1126的yolov5s部署)
  - [文件目录结构描述](#文件目录结构描述)
  - [多线程处理](#多线程处理)
  - [模型量化](#模型量化)
  - [性能测试](#性能测试)
  - [参考链接](#参考链接)


# 基于瑞芯微RV1126的yolov5s部署

本仓库通过调用瑞芯微的**音视频处理库rkmedia**，结合瑞芯微官方的**量化工具rknn_toolkit**,成功利用rv1126上的，训练使用的数据集为**wider_face数据集**，显示平均帧率为**13fps/s**，推理一帧图像**npu耗时**大约为**90ms**。



## 文件目录结构描述
```plaintext

├── build	//编译二进制文件及makefile

├── CMakeLists.txt 
├── include
│   ├── 3rdparty	
│   │   ├── rga		//RGA (Raster Graphic Acceleration Unit)头文件
│   │   └── rkmedia_api.h		//视频帧处理头文件
│   ├── algorithm
│   │   ├── model_config.hpp		//模型推理及视频参数配置头文件
│   │   ├── ObjectDetector.hpp	//视频推理头文件

│   │   └── postprocess.h	//模型后推理头文件
│   ├── application
│   │   └── Application.hpp	//解耦模块头文件
│   ├── core
│   │   └──  MediaException.hpp	//异常抛出类
│   ├── hardware
│   │   ├── config.hpp	//摄像头，lcd显示屏，isp配置头文件
│   │   ├── interface.hpp
│   │   ├── RgaProcessor.hpp	//rkmedia rga模块
│   │   ├── VideoInput.hpp	//rkmedia vi模块
│   │   └── VideoOutput.hpp	//rkmedia vo模块
│   └── utils	//一些通用模块
│       ├── get_fps.hpp	//计算帧数
│       └── performance_monitor.hpp	//统计性能表现
├── lib	//rkmedia动态链接库
├── readme.md
├── rknn_convertor	
│   ├── img
│   │   ├── dataset.txt
│   │   └── test.jpg	//推理一帧该图像得到量化误差统计
│   ├── labels
│   │   ├── data_oneline.txt
│   │   └── wider_face_list.txt	//量化校正标签
│   ├── log
│   │   ├── entire_qnt	//保存整个量化模型完整运行时每一层的结果（已转成 float32）
│   │   ├──entire_qnt_error_analysis.txt	

//保存整个量化模型完整运行时每一层的余弦距离/欧式距离、归一化后的余弦距离/欧式距离

│   │   ├──fp32	//保存整个量化模型完整运行时每一层的结果（已转成 float32）

│   │   ├── individual_qnt	//保存每层单独运行时的结果（已转成 float32）

│   │   └── individual_qnt_error_analysis.txt	

//保存每层单独运行时的结果的余弦距离/欧式距离、归一化后的余弦距离/欧式距离
│   ├── onnx_model
│   ├── pytorch_model
│   ├── rknn_model	//rknn模型， person_5_s_relu_onnx.rknn为最终模型
│   ├── utils	
│   │   ├── accuary_analysis.py	//量化误差分析
│   │   ├── convert-onnx-to-rknn.py	//onnx转rknn
│   │   ├── convert-pytorch-to-rknn.py	//pt转rknn
│   │   ├── dump_data_distribute
│   │   ├── log_feedback_to_the_rknn_toolkit_dev_team.log
│   │   └── precompile_rknn_model.py	//rknn模型预编译
│   └── utils_old	//废弃的转换工具
├── src	//上述头文件的源程序实现
└── video	//rv1126开发板测试视频
```

## 多线程处理

该项目分为视频预处理，视频推理，视频后处理三个部分

采用流水线的结构能够提高npu利用率，同时考虑到其同步关系，因此采用生产者消费者模式，通过条件变量和互斥锁实现，利用队列将相应资源发送给相应消费者

具体而言就是，图像预处理线程，利用瑞芯微的rkmedia音视频处理框架获得一帧图像，利用rga等硬件对图像进行裁剪和缩放以达到模型输入尺寸，然后将原图像输入图像后处理队列，预处理后的图像输入视频推理队列，然后视频推理线程从视频推理队列得到处理后的图像，并将类别，置信度，坐标及长宽发送给图像后处理队列，然后图像后处理队列进行非极大值抑制，图像坐标转换，标注框描绘等操作



## 模型量化

因能力有限，故模型量化误较差，希望通过以后的持续学习能够更进一步

量化使用的是瑞芯微官方的rknntoolkit工具，先进行模型预处理，然后从wider_face测试集随机抽取100张图片进行量化校正，采用非对称量化量化成int8类型。

量化成int8类型的模型量化误差如下

![individual_qnt_error_analysis](https://github.com/ibgty/rv1126/blob/main/rknn_convertor/log/individual_qnt_error_analysis.png)

![entire_qnt_error_analysis](https://github.com/ibgty/rv1126/blob/main/rknn_convertor/log/entire_qnt_error_analysis.png)

## 性能测试

测试环境为寝室，拍摄设备为自用手机，测试视频如下

![test](https://github.com/ibgty/rv1126/blob/main/video/test.gif)

## 参考链接

 

【【正点原子】AI人工智能深度学习(RV1126/RK3568/RK3588)-第3期 AI模型部署与项目实战】https://www.bilibili.com/video/BV1bC4y177FT?vd_source=035442767045d0ca23fb8129e7c68c62

https://github.com/kaylorchen/rk3588-yolo-demo

https://github.com/dogewu/rv1126-YoloV5-Deepsort