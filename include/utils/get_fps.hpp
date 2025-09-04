#ifndef FPS_H
#define FPS_H

#include <sys/time.h>
#include <vector>
#include <deque>

class FPSContext{
private:
    // 基础计时
    struct timeval start_time;
    struct timeval current_time;
    
    // 实时统计
    double frame_count;
    double fps;
    
    std::deque<double> five_second_log; // 完整周期记录
     void calculate_statistics();
public:
    FPSContext();
    
     void calculate_fps();
    
  


};

#endif // FPS_H