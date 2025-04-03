#ifndef __GET_TIME_H__
#define __GET_TIME_H__
#include <iostream>
#include <limits>
#include <mutex>
#include <time.h>

// 新增阶段枚举类型
enum class Stage {
    IMAGE_PREPROCESS,    // 图像获取与预处理
    INFERENCE,           // 推理执行
    POSTPROCESS_RENDER   // 后处理与渲染
};

class PerformanceMonitor {
public:
    PerformanceMonitor()
        : frame_counts_{0, 0, 0},      // 各阶段帧计数
          total_times_{0.0, 0.0, 0.0}, // 各阶段总时间
          max_times_{0.0, 0.0, 0.0},   // 各阶段最大时间
          min_times_{std::numeric_limits<double>::max(),
                      std::numeric_limits<double>::max(),
                      std::numeric_limits<double>::max()},
          current_stage_(Stage::IMAGE_PREPROCESS) // 当前阶段
    {
        // 初始化各阶段最小时间为最大值
        for (auto& min_time : min_times_) {
            min_time = std::numeric_limits<double>::max();
        }
    }

    // 开始计时（带阶段参数）
    void begin(Stage stage) {
        gettimeofday(&inference_time_begin, NULL);
        current_stage_ = stage;
    }

    // 结束计时（带阶段参数）
    void end() {
        gettimeofday(&inference_time_end, NULL);
        double elapsed = (inference_time_end.tv_sec - inference_time_begin.tv_sec) * 1000 +
                         (inference_time_end.tv_usec - inference_time_begin.tv_usec) / 1e3;
        update(elapsed, current_stage_);
    }

    // 更新统计数据（带阶段参数）
    void update(double time, Stage stage) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& stats = getStageStats(stage);
        stats.frame_count++;
        stats.total_time += time;
        stats.max_time = std::max(stats.max_time, time);
        stats.min_time = std::min(stats.min_time, time);

        // 检查打印间隔
        if (stats.frame_count >= frame_end) {
            print_statistics(stage);
            reset_statistics(stage);
        }
    }

    // 打印统计信息（带阶段参数）
    void print_statistics(Stage stage) {
        auto& stats = getStageStats(stage);
        
        std::cout << "[Performance Stats - ";
        switch (stage) {
            case Stage::IMAGE_PREPROCESS:
                std::cout << "Image Preprocess";
                break;
            case Stage::INFERENCE:
                std::cout << "Inference";
                break;
            case Stage::POSTPROCESS_RENDER:
                std::cout << "Postprocess & Render";
                break;
        }
        std::cout << "] "
                  << "Frames: " << stats.frame_count
                  << " | Avg: " << stats.total_time / stats.frame_count << " ms"
                  << " | Max: " << stats.max_time << " ms"
                  << " | Min: " << stats.min_time << " ms"
                  << "----------------------------------------"
                  << std::endl;
    }

private:
    // 获取阶段统计结构体（带线程安全）
    struct StageStats {
        int frame_count = 0;
        double total_time = 0.0;
        double max_time = 0.0;
        double min_time = 0.0;
    };

    StageStats& getStageStats(Stage stage) {
        return const_cast<StageStats&>(static_cast<const PerformanceMonitor*>(this)->getStageStats(stage));
    }

    const StageStats& getStageStats(Stage stage) const {
        return stage_stats_[static_cast<int>(stage)];
    }

    // 重置指定阶段的统计数据
    void reset_statistics(Stage stage) {
        auto& stats = getStageStats(stage);
        stats.total_time = 0.0;
        stats.max_time = 0.0;
        stats.min_time = std::numeric_limits<double>::max();
        stats.frame_count = 0;
    }

    // 成员变量
    struct timeval inference_time_begin;
    struct timeval inference_time_end;
    double elapsed;

    // 各阶段独立统计
    StageStats stage_stats_[3];  // 对应枚举的三个阶段
    int frame_end = 30;          // 统计间隔帧数
    
    // 线程安全保护
    mutable std::mutex mutex_;
    Stage current_stage_;        // 当前活跃阶段
};

#endif