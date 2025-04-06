#include "interface.hpp"
interface::interface()
{

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
    m_Rga->rga_resize_init(); 

}

void interface::resize (int src_fd, void *src_virt, int src_w, int src_h, int  dst_fd, void *dst_virt, int dst_w, int dst_h)
{
   std::cout<<18<<std::endl; 
   std::cout<<dst_w<<"x"<<dst_h<<std::endl;
  m_Rga->rga_resize(src_fd,src_virt,src_w,src_h,dst_fd,dst_virt,dst_w, dst_h);
}
void interface::show_info(){
  std::cout<<"VI CameraIdx:"<<m_camera_config->m_camId<<std::endl;
  std::cout<<"pDeviceName"<<m_camera_config->pDeviceName_01<<std::endl;
  std::cout<<"Resolution"<<m_camera_config->video_width<<"x"<<m_camera_config->video_height<<std::endl;
  std::cout<<"VO pcDevNode: "<<m_display_config->deviceNode<<std::endl;
  std::cout<<"Resolution"<<m_display_config->dispRect_width<<"X"<<m_display_config->dispRect_height<<std::endl;
}
void interface::isp_init()
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


interface::~interface() 
{
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
  delete[] m_display_config;
  m_Rga->rga_resize_deinit(); 
  delete[] m_Rga_config;
}



