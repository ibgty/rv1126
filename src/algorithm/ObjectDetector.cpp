#include "ObjectDetector.hpp"
#include "postprocess.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "rknn_api.h"
#include "rkmedia_api.h"
#include "opencv2/opencv.hpp"
#include "performance_monitor.hpp"
    // 辅助函数：加载模型文件
    std::vector<uint8_t> loadModelFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw MediaException(
                MediaException::MODEL_LOAD_FAILURE,
                "Cannot open model file: " + path
            );
        }
        
        file.seekg(0, std::ios::end);
        const size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> modelData(size);
        if (!file.read(reinterpret_cast<char*>(modelData.data()), size)) {
            throw MediaException(
                MediaException::MODEL_LOAD_FAILURE,
                "Failed to read model file: " + path
            );
        }
        return modelData;
    }


ObjectDetector::ObjectDetector()
{
    try {

  // std::cout<<8<<std::endl;
        loadModel();
  // std::cout<<9<<std::endl;
        get_model_width_height();
          // std::cout<<"m_config.inputWidth,m_config.inputHeight"<<m_config.inputWidth<<","<<m_config.inputHeight<<std::endl;
  // std::cout<<10<<std::endl;
        initializeIOMemory();
  // std::cout<<11<<std::endl;
        m_initialized = true;
        // for (int i=0;i<3;i++)
        // {
        //   src_mb[i]=nullptr;
        // }
        src_mb=nullptr;
    } catch (...) {
        if (m_initialized) cleanup();
        throw;
    }
    m_interface=interface::getInstance();
    first_frame=-1;
    num_buffer=10;
}

ObjectDetector::~ObjectDetector() {
    if (m_initialized) cleanup();
}




void ObjectDetector::loadModel() {
    // 加载模型文件
    auto modelData = loadModelFile(m_config.modelPath);
    
  // std::cout<<12<<std::endl;
    // 初始化RKNN上下文
    int ret = rknn_init(&m_ctx, 
                            modelData.data(), 
                            modelData.size(), 
                            0);
    if (ret != RKNN_SUCC) {
        throw MediaException(
            MediaException::RKNN_INIT_FAILURE,
            "rknn_init failed. Error code: " + std::to_string(ret)
        );
    }

    // 获取输入输出信息
    ret=rknn_query(m_ctx, RKNN_QUERY_IN_OUT_NUM, &m_ioNum, sizeof(m_ioNum));
        if (ret != RKNN_SUCC) {
        throw MediaException(
            MediaException::RKNN_INIT_FAILURE,
            "RKNN_QUERY_IN_OUT_NUM failed. Error code: " + std::to_string(ret)
        );
    }
    // std::cout<<"model input num: "<<m_ioNum.n_input<<std::endl;
    // std::cout<<", output num: "<< m_ioNum.n_output<<std::endl;
    
    // 查询输入输出属性
    std::cout<<"input tensors: "<<std::endl;
    m_inputAttrs.resize(m_ioNum.n_input);
    m_outputAttrs.resize(m_ioNum.n_output);
    
    for (int i = 0; i < m_ioNum.n_input; ++i) {
        m_inputAttrs[i].index = i;
        ret=rknn_query(m_ctx, RKNN_QUERY_INPUT_ATTR, &m_inputAttrs[i], sizeof(rknn_tensor_attr));
                if (ret != RKNN_SUCC) {
        throw MediaException(
            MediaException::RKNN_INIT_FAILURE,
            "RKNN_QUERY_INPUT_ATTR failed. Error code: " + std::to_string(ret)
        );
    }
    print_tensor(&m_inputAttrs[i]);
    }
    
    for (int i = 0; i < m_ioNum.n_output; ++i) {
        m_outputAttrs[i].index = i;
        ret=rknn_query(m_ctx, RKNN_QUERY_OUTPUT_ATTR, &m_outputAttrs[i], sizeof(rknn_tensor_attr));
                if (ret != RKNN_SUCC) {
        throw MediaException(
            MediaException::RKNN_INIT_FAILURE,
            "RKNN_QUERY_OUTPUT_ATTR failed. Error code: " + std::to_string(ret)
        );
    }
    print_tensor(&m_outputAttrs[i]);
    }
}





void ObjectDetector::get_frame(int index) { src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 0, -1); if(!src_mb) { throw MediaException( MediaException::FRAME_ACQUIRE_FAILURE, "RK_MPI_SYS_GetMediaBuffer failed. ");
        }
  // if(!src_mb)
  // {
  // std::cout<<"rk_mpi_sys_getmediabuffer fail"<<std::endl;
  // }
}

void ObjectDetector::initializeIOMemory() {
    // 初始化输入内存
    // for (int i = 0; i < m_ioNum.n_input; ++i) {
    //     m_inputMems.emplace_back(rknn_create_mem(m_ctx, m_inputAttrs[i].size));
    //     if (!m_inputMems.back()) {
    //         throw MediaException(
    //             MediaException::BUFFER_ALLOC_FAILURE,
    //             "Failed to allocate input memory for index " + std::to_string(i)
    //         );
    //     }
            

    // }
  //  std::cout<<13<<std::endl; 

    // 初始化输出内存
    for (int i = 0; i < m_ioNum.n_output; ++i) {
        m_outputMems.emplace_back(rknn_create_mem(m_ctx, m_outputAttrs[i].size));
        if (!m_outputMems.back()) {
            throw MediaException(
                MediaException::BUFFER_ALLOC_FAILURE,
                "Failed to allocate output memory for index " + std::to_string(i)
            );
        }
         
     

    }
      //  std::cout<<14<<std::endl; 
}
void ObjectDetector::print_tensor(rknn_tensor_attr*attr)
{
    std::cout << "index=" << attr->index 
          << " name=" << attr->name 
          << " n_dims=" << attr->n_dims 
          << " dims=[" << attr->dims[3] << " " << attr->dims[2] 
          << " " << attr->dims[1] << " " << attr->dims[0] << "]" 
          << " n_elems=" << attr->n_elems 
          << " size=" << attr->size 
          << " fmt=0"  // Hardcoded 0 from original printf
          << " type=" << attr->type 
          << " qnt_type=" << attr->qnt_type 
          << " fl=" << attr->fl 
          << " zp=" << attr->zp 
          << " scale=" << std::setprecision(6) << attr->scale 
          << std::endl;
}

void ObjectDetector::get_model_width_height()
{
  int *width, *height;
    if (m_inputAttrs[0].fmt == RKNN_TENSOR_NCHW)
  {
      printf("model is NCHW input fmt\n");
      width = (int*)&m_inputAttrs[0].dims[0];
      height =(int*)&m_inputAttrs[0].dims[1];
  }
  else
  {
      printf("model is NHWC input fmt\n");
      width = (int*)&m_inputAttrs[0].dims[1];
      height = (int*)&m_inputAttrs[0].dims[2];
  }
  m_config.inputWidth=*width;
  m_config.inputHeight= *height;
std::cout << "model input height=" << *height 
          << ", width=" << *width 
          << ", channel=" << 3 
          << std::endl;
}



void ObjectDetector::cleanup() {
    // 释放输入内存
    // for (auto& mem : m_inputMems) {
    //     rknn_destroy_mem(m_ctx, mem);
    // }
    
    // 释放输出内存
    for (auto& mem : m_outputMems) {
        rknn_destroy_mem(m_ctx, mem);
    }
        
    // 销毁RKNN上下文
    if (m_ctx) {
        rknn_destroy(m_ctx);
    }
}
void ObjectDetector::preprocess(int index) {
  if(first_frame==-1)
  {
  first_frame=1;
  }
  else if(first_frame==1)
  {
  first_frame=0; 
  }
  {
  //  std::cout<<"debug1"<<std::endl;
   std::unique_lock<std::mutex> lock(M_Processing_Mutex);
  while(input_buffer.size()>num_buffer) 
  {
    Pre_Process_Cond.wait(lock);
  }
  //  std::cout<<"debug2"<<std::endl;
    get_frame(index);
    // RK_MPI_SYS_SendMediaBuffer(RK_ID_VO, 0, src_mb);
    // if(first_frame==0)
    // {
  input_buffer.push(src_mb);
    // }
    Post_Process_Cond.notify_one(); 
}
// get_frame(index);
// RK_MPI_SYS_SendMediaBuffer(RK_ID_VO, 0, src_mb);
// // std::this_thread::sleep_for(std::chrono::seconds(1));
// RK_MPI_MB_ReleaseBuffer(src_mb);
perform_pre_process.begin(Stage::IM_PREPROCESS);
    unsigned char *input_data = (unsigned char *)RK_MPI_MB_GetPtr(src_mb); 
    void *buffer=malloc(m_config.inputHeight*m_config.inputWidth*3);
    m_interface->resize(-1, input_data, m_interface->get_rga_width(), m_interface->get_rga_height(), 0, buffer, m_config.inputWidth,m_config.inputHeight);
perform_pre_process.end();
    {
  //  std::cout<<"debug3"<<std::endl;
      std::unique_lock<std::mutex> lock(M_Detect_Mutex);
     while(resize_buffer.size()>num_buffer) 
     {
       Pre_Process_Cond.wait(lock);
     }
  //  std::cout<<"debug4"<<std::endl;
    resize_buffer.push(buffer);
    Detect_Cond.notify_one();
    }
    //  rknn_set_io_mem(m_ctx, m_inputMems[0], &m_inputAttrs[0]);


      //  std::cout<<16<<std::endl; 
}
void ObjectDetector::detect() {
  void*buffer;
  {
  //  std::cout<<"debug5"<<std::endl;
  std::unique_lock<std::mutex> lock(M_Detect_Mutex);
  while(resize_buffer.empty()) 
  {
    Detect_Cond.wait(lock);
  }
  //  std::cout<<"debug6"<<std::endl;
  buffer=resize_buffer.front();
  resize_buffer.pop();
  Pre_Process_Cond.notify_one();  
}
perform_detect.begin(Stage::INFERENCE);
  for (int i = 0; i < m_ioNum.n_output; ++i) {
    rknn_set_io_mem(m_ctx, m_outputMems[i], &m_outputAttrs[i]);
 }
 rknn_input inputs[1];
 memset(inputs, 0, sizeof(inputs));
 inputs[0].index = 0;
 inputs[0].type = RKNN_TENSOR_UINT8;
 inputs[0].size = m_config.inputHeight*m_config.inputWidth*3;
 inputs[0].fmt = RKNN_TENSOR_NHWC;
 inputs[0].buf = buffer;
 int ret = rknn_inputs_set(m_ctx, m_ioNum.n_input, inputs);

    if (!m_initialized) {
        throw MediaException(
            MediaException::RKNN_INIT_FAILURE,
            "Detector not initialized"
        );
    }
    if (ret < 0)
    {
      printf("ERROR: rknn_inputs_set fail! ret=%d\n", ret);
      return;
    }
    

        // 预处理
        // src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 0, -1);
      // 零拷贝: 直接使用输入内存
        // unsigned char *input_data = (unsigned char *)RK_MPI_MB_GetPtr(src_mb);
        // preprocess(input_data);
        
        // 执行推理
        std::cout<<"rknn run"<<std::endl;

        ret = rknn_run(m_ctx, nullptr);
        if (ret != RKNN_SUCC) {
            throw MediaException(
                MediaException::RUNTIME_PROCESSING,
                "rknn_run failed. Error code: " + std::to_string(ret)
            );
        }
        
        std::vector<float> outScales;
        std::vector<uint8_t> outZps;
        
        for (const auto& attr : m_outputAttrs) {
            outScales.push_back(attr.scale);
            outZps.push_back(attr.zp);
        }
    
        // 调用后处理函数
        
        memset(&result, 0, sizeof(result));
        int min=std::min( m_interface->get_rga_width(), m_interface->get_rga_height()); 
        
        post_process(
            reinterpret_cast<uint8_t*>(m_outputMems[0]->logical_addr),
            reinterpret_cast<uint8_t*>(m_outputMems[1]->logical_addr),
            reinterpret_cast<uint8_t*>(m_outputMems[2]->logical_addr),
            m_config.inputHeight,
            m_config.inputWidth,
            m_config.confThreshold,
            m_config.nmsThreshold,
            m_config.visThreshold,
            (float)m_config.inputWidth /min , // 实际应传入原始帧尺寸
            (float)m_config.inputHeight / min,
            outZps,
            outScales,
            &result
        ); 
        free(buffer);
perform_detect.end();
        // 后处理
         {
  // std::cout<<"debug7"<<std::endl;
   std::unique_lock<std::mutex> lock(M_PostPocess_Mutex);
  while(inference_result.size()>num_buffer) 
  {
    Detect_Cond.wait(lock);
  }
  // std::cout<<"debug8"<<std::endl;
    // 假设frame已经是RGB888格式且尺寸匹配
    // 实际应添加格式转换和缩放逻辑
    inference_result.push(result);
  Post_Process_Cond.notify_one(); 
} 

  // std::cout<<"debug"<<2<<std::endl;
    
   
}
void ObjectDetector::postprocess(int index) {

  detect_result_group_t result;
  MEDIA_BUFFER input_dat;
 {
  //  std::cout<<"debug9"<<std::endl;
  std::unique_lock<std::mutex> lock(M_PostPocess_Mutex);
  while(inference_result.empty()) 
  {
   Post_Process_Cond.wait(lock);
  }
  //  std::cout<<"debug10"<<std::endl;
   result=inference_result.front();
  inference_result.pop();
  Detect_Cond.notify_one(); 
 }
 {
  //  std::cout<<"debug11"<<std::endl;
   std::unique_lock<std::mutex> lock(M_Processing_Mutex);
   while(input_buffer.empty()) 
   {
    Post_Process_Cond.wait(lock);
   }
  //  std::cout<<"debug12"<<std::endl;
    input_dat=input_buffer.front();
   input_buffer.pop();
   Pre_Process_Cond.notify_one(); 
  }
  perform_post_process.begin(Stage::POSTPROCESS_RENDER);
// printf("resutl_count:%d\n",result.count);
for (int i = 0; i <result.count; ++i) {
   auto& det_result = result.results[i];
   // result.boxes.emplace_back(BBox{
   //     det.box.left,
   //     det.box.top,
   //     det.box.right,
   //     det.box.bottom,
   //     det.prop,
   //     det.classId
   // });
   
   // 类别标签映射（示例）
   // result.labels.emplace_back(std::to_string(det.classId));
         int left = det_result.box.left;
 int top = det_result.box.top;
 int right = det_result.box.right;
 int bottom = det_result.box.bottom;
 int w = (det_result.box.right - det_result.box.left) ;
 int h = (det_result.box.bottom - det_result.box.top) ;
       if (left < 0)
 {
   left = 0;
 }
 if (top < 0)
 {
   top = 0;
 }

 while ((uint32_t)(left + w) >= m_interface->get_rga_width())
 {
   w -= 16;
 }
 while ((uint32_t)(top + h) >= m_interface->get_rga_height())
 {
   h -= 16;
 }
 std::cout << "border=(" << left << " " << top << " " << w << " " << h << ")\n";
 using namespace cv;
 Mat orig_img = Mat(m_interface->get_rga_height(),  m_interface->get_rga_width(), CV_8UC3, RK_MPI_MB_GetPtr(input_dat));//黑白灰图案
 cv::rectangle(orig_img,cv::Point(left, top),cv::Point(right, bottom),cv::Scalar(0,255,255),5,8,0);
 putText(orig_img, result.results[i].name, Point(left, top-16), FONT_HERSHEY_TRIPLEX, 3, Scalar(0,0,255),4,8,0);
}
// printf("ok\n");
   RK_MPI_SYS_SendMediaBuffer(RK_ID_VO, 0, input_dat);
RK_MPI_MB_ReleaseBuffer(input_dat);

input_dat = nullptr;
perform_post_process.end();
}