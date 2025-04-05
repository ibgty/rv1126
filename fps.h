#ifndef __FPS_H__
#define __FPS_H__
#include"debug.h"
#include <time.h>
#include <iostream>
class  FPSContext:public debug {
  private:
  struct timeval start_time;
  struct timeval current_time;
  double frame_count;
  double fps;
  public:
  FPSContext(double count,double fps):frame_count(count),fps(fps)
  {}
  void calculate_fps(FPSContext* ctx) {
    gettimeofday(&ctx->current_time, NULL);
  
    if (ctx->frame_count == 0) {
        ctx->start_time = ctx->current_time;
    }
  
    ctx->frame_count++;
    double elapsed = (ctx->current_time.tv_sec - ctx->start_time.tv_sec) +
                    (ctx->current_time.tv_usec - ctx->start_time.tv_usec) / 1e6;
    log()<<"time:"<<elapsed<<std::endl;
    log()<<"frame_count "<<frame_count<<std::endl;
    if (elapsed >= 0.5) {
        ctx->fps = ctx->frame_count / elapsed;
        // printf("FPS: %.2f\n", ctx->fps);
        log()<<"FPS"<< std::fixed << std::setprecision(2)<<ctx->fps<<std::endl;
        // 保留10%的历史数据（平滑过渡）
        // ctx->frame_count = ctx->frame_count * 0.1; 
        ctx->start_time = ctx->current_time;
        ctx->frame_count=0;
    }
  }
};
#endif