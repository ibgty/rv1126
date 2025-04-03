#ifndef __GET_TIME_H__
#define __GET_TIME_H__
#include <iostream>
#include <limits>
#include <mutex>
#include <time.h>
enum class Stage {
  IMAGE_PREPROCESS,    // 图像获取与预处理
  INFERENCE,           // 推理执行
  POSTPROCESS_RENDER   // 后处理与渲染
};
class PerformanceMonitor {
public:
    // 构造函数
    PerformanceMonitor()
        : frame_count_(0),
          frame_end(30),
          total_time_(0.0),
          max_time_(0.0),
          min_time_(std::numeric_limits<double>::max()),
          inference_time_begin{0,0},
          inference_time_end{0,0},
          elapsed(0)
    {}
    void begin()
    {
      gettimeofday(&inference_time_begin, NULL);
    }
    void end()
    {
      gettimeofday(&inference_time_end, NULL);
      elapsed=(inference_time_end.tv_sec-inference_time_begin.tv_sec)*1000+(inference_time_end.tv_usec-inference_time_begin.tv_usec)/1e3;
      update(elapsed);
    }

    // 更新统计数据
    void update(double inference_time) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 更新统计信息
        frame_count_++;
        total_time_ += inference_time;
        max_time_ = std::max(max_time_, inference_time);
        min_time_ = std::min(min_time_, inference_time);

        // 检查打印间隔
        if (frame_count_>=frame_end) {
            print_statistics();
            reset_statistics();
        }
    }

    // 打印统计信息并重置
    void print_statistics() {
        if (frame_count_ < frame_end) return;

        double avg_time = total_time_ / frame_count_;
        std::cout << "[Performance Stats] "
                  << "Frames: " << frame_count_
                  << " | Avg: " << avg_time << " ms"
                  << " | Max: " << max_time_ << " ms"
                  << " | Min: " << min_time_ << " ms"
                  << "----------------------------------------"
                  <<std::endl;
    }

private:
    // 重置统计数据的辅助方法
    void reset_statistics() {
        total_time_ = 0.0;
        max_time_ = 0.0;
        min_time_ = std::numeric_limits<double>::max();
        frame_count_=0;
    }

    // 成员变量
    int frame_count_;
    int frame_end;
    double total_time_;
    double max_time_;
    double min_time_;
    struct timeval  inference_time_begin;
    struct timeval  inference_time_end;
    double elapsed;
    // 线程安全保护
    mutable std::mutex mutex_;
};

#endif