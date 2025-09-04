#include "performance_monitor.hpp"
#include <algorithm>
#define MONITOR_PRINT
PerformanceMonitor::PerformanceMonitor()
    : current_stage_(Stage::IM_PREPROCESS) {}

void PerformanceMonitor::begin(Stage stage) {
    gettimeofday(&inference_time_begin, nullptr);
    current_stage_ = stage;
}

void PerformanceMonitor::end() {
    gettimeofday(&inference_time_end, nullptr);
    double elapsed = (inference_time_end.tv_sec - inference_time_begin.tv_sec) * 1000 +
                     (inference_time_end.tv_usec - inference_time_begin.tv_usec) / 1000.0;
    update(elapsed, current_stage_);
}

void PerformanceMonitor::update(double time, Stage stage) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& stats = getStageStats(stage);
    stats.frame_count++;
    stats.total_time += time;
    stats.max_time = std::max(stats.max_time, time);
    stats.min_time = std::min(stats.min_time, time);

    if (stats.frame_count >= frame_end) {
      // #ifdef MONITOR_PRINT
        // #endif
        print_statistics(stage);
        reset_statistics(stage);
    }
}

void PerformanceMonitor::print_statistics(Stage stage) {
    auto& stats = getStageStats(stage);
    
    std::cout << "[Performance Stats - ";
    switch (stage) {
        case Stage::IM_PREPROCESS:
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
              << " ----------------------------------------"
              << std::endl;
}

void PerformanceMonitor::reset_statistics(Stage stage) {
    auto& stats = getStageStats(stage);
    stats.total_time = 0.0;
    stats.max_time = 0.0;
    stats.min_time = std::numeric_limits<double>::max();
    stats.frame_count = 0;
}

PerformanceMonitor::StageStats& PerformanceMonitor::getStageStats(Stage stage) {
    return const_cast<StageStats&>(static_cast<const PerformanceMonitor*>(this)->getStageStats(stage));
}

const PerformanceMonitor::StageStats& PerformanceMonitor::getStageStats(Stage stage) const {
    return stage_stats_[static_cast<int>(stage)];
}