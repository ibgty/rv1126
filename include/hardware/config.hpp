#ifndef CONFIG_HPP
#define CONFIG_HPP
#include " MediaException.hpp"
#include "sample_common.h"
typedef struct ViConfig {
  RK_CHAR *pDeviceName_01 = "rkispp_scale0";
  RK_CHAR *pDeviceName_02 = "rkispp_scale1";
  RK_U32 u32BufCnt=16;
  RK_U32 video_width=2592;
  RK_U32 video_height=1944;
  IMAGE_TYPE_E image_type=IMAGE_TYPE_NV12;
  VI_CHN_WORK_MODE vi_work_mode=VI_WORK_MODE_NORMAL;
  VI_CHN_BUF_TYPE vi_buf_type=VI_CHN_BUF_TYPE_MMAP;
  int m_camId=0;
  VI_CHN chn=0; 
}ViConfig;
typedef struct RgaConfig {
  RK_BOOL EnBufPool = RK_TRUE;
  RK_U16 BufPoolCnt = 16;
  RK_U16 Rotaion = 0;
  RK_U32 input_x = 0;
  RK_U32 input_y = 0;
  RK_U32 input_width = 2592;
  RK_U32 input_height = 1944;
  RK_U32 input_HorStride = 2592;
  RK_U32 input_VirStride = 1944;
  IMAGE_TYPE_E input_format = IMAGE_TYPE_NV12;
  RK_U32 output_x = 0;
  RK_U32 output_y = 0;
  IMAGE_TYPE_E output_format = IMAGE_TYPE_RGB888;
  RK_U32 output_width = 2592;
  RK_U32 output_height=1944;
  RK_U32 output_HorStride = 2592;
  RK_U32 output_VirStride = 1944;
  RGA_CHN chn=0;
}RgaConfig;
typedef struct DisplayConfig {
  RK_CHAR* deviceNode = "/dev/dri/card0";
  VO_PLANE_TYPE_E planeType = VO_PLANE_OVERLAY;
  IMAGE_TYPE_E imgType = IMAGE_TYPE_RGB888;
  RK_U16  zpos = 0;           // Z轴层级
  RK_S32 imgRect_x=756;
  RK_S32 imgRect_y=12;
  RK_S32 imgRect_width=1080;
  RK_S32 imgRect_height=1920; 
  RK_S32 dispRect_x=0;
  RK_S32 dispRect_y=0;
  RK_S32 dispRect_width=720;
  RK_S32 dispRect_height=1280; 
  VO_CHN chn=0;
}DisplayConfig;
typedef struct IspConfig{
  rk_aiq_working_mode_t mode=RK_AIQ_WORKING_MODE_NORMAL;
  char *iq_file_dir="/etc/iqfiles";
  int isp_frame=25;
  int m_camId=0;
}IspConfig;

#endif