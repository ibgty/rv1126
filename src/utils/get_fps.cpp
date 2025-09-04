#include "get_fps.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

FPSContext::FPSContext() 
    : frame_count(0), fps(0)
{

}

void FPSContext::calculate_fps() 
{
    gettimeofday(&current_time, nullptr);
    
    // 初始化起始时间
    if (frame_count == 0) {
        start_time = current_time;
    }
    
    frame_count++;
    // period_frame_count++;

    // 计算时间差（秒）
    double elapsed = (current_time.tv_sec - start_time.tv_sec) +
                    (current_time.tv_usec - start_time.tv_usec) / 1e6;

    // 每0.5秒统计一次实时FPS
    if (elapsed >= 0.5) {
      
        fps = frame_count / elapsed;
        five_second_log.push_back(fps);
        std::cout<<"time(time > 0.5):"<<elapsed<<std::endl;
        std::cout<<"fps(per second in 0.5 second):"<<fps<<std::endl;
        // 保持最近10个0.5秒记录（5秒数据）
        if (five_second_log.size() >= 10) {

            calculate_statistics();
        }

        // 重置计数器
        start_time = current_time;
        frame_count = 0;
    }

}

void FPSContext::calculate_statistics() 
{
    if (five_second_log.empty()) 
    {
      std::cout<<"five_second_log is empty"<<std::endl;
      return;
    }

    // std::pair<
    //     std::vector<double>::iterator, 
    //     std::vector<double>::iterator
    // > minmax = std::minmax_element(
    //     five_second_log.begin(), 
    //     five_second_log.end()
    // );

    double total_fps = 0;
     double min_fps =100000; 
    double max_fps =0;
    for(const auto& fps : five_second_log) {
        total_fps += fps;
        if(fps<min_fps)
        {
        min_fps=fps;
        }
        if(max_fps<fps)
        {
        max_fps=fps;
        }
    }
    
    double avg = total_fps / five_second_log.size();

    
    // 输出统计结果
    std::cout << "\n==== 5秒统计 ====\n"
              << "平均 FPS: " << std::fixed << std::setprecision(1) << avg << "\n"
              << "最大瞬时 FPS: " << max_fps << "\n"
              << "最小瞬时 FPS: " << min_fps << "\n"
              << "=================\n";


    five_second_log.clear();

}
