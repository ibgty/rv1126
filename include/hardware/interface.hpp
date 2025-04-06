#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include "RgaProcessor.hpp"
#include "VideoInput.hpp"
#include "VideoOutput.hpp"
#include " MediaException.hpp"
#include "rkmedia_api.h"
#include "rknn_api.h"
// #include "MediaException.hpp"
// #include "model_config.hpp"
#include <vector>
#include <string>
#include <memory>
class interface {
private:
    interface();
   ~interface();
    interface(const interface&) = delete;
    interface operator=(const interface&) = delete;
    

    VideoInput* m_camera;
    RgaProcessor* m_Rga;
    VideoOutput* m_display; 

    ViConfig* m_camera_config;
    RgaConfig* m_Rga_config;
    DisplayConfig* m_display_config;
    IspConfig* m_isp_config;
    

    void show_info();
    void isp_init();


public:
   
    static interface* getInstance() {
        static interface instance;  // C++11保证局部静态变量初始化线程安全
        return &instance;
    }
    rga_context* get_rga_context()
    {
      return m_Rga->get_rga_context();
    }
    RK_U32 get_rga_width()
    {
      return m_Rga->get_rga_width();
    }
    RK_U32 get_rga_height()
    {
      return m_Rga->get_rga_height();
    }
    void resize (int src_fd, void *src_virt, int src_w, int src_h, int  dst_fd, void *dst_virt, int dst_w, int dst_h);
};
#endif