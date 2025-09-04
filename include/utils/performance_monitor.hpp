#ifndef PERFORMANCE_MONITOR_HPP
#define PERFORMANCE_MONITOR_HPP

#include <iostream>
#include <limits>
#include <mutex>
#include <sys/time.h>

enum class Stage {
    IM_PREPROCESS,    // 图像获取与预处理
    INFERENCE,        // 推理执行
    POSTPROCESS_RENDER// 后处理与渲染
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    void begin(Stage stage);
    void end();
    void update(double time, Stage stage);
    
private:
    struct StageStats {
        int frame_count = 0;
        double total_time = 0.0;
        double max_time = 0.0;
        double min_time = std::numeric_limits<double>::max();
    };

    StageStats& getStageStats(Stage stage);
    const StageStats& getStageStats(Stage stage) const;
    void print_statistics(Stage stage);
    void reset_statistics(Stage stage);

    struct timeval inference_time_begin;
    struct timeval inference_time_end;
    StageStats stage_stats_[3];
    int frame_end = 5;
    mutable std::mutex mutex_;
    Stage current_stage_;
};

#endif // PERFORMANCE_MONITOR_HPP