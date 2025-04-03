#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <iostream>

class DebugProxy {
public:
    explicit DebugProxy(bool enabled) : enabled_(enabled) {}

    // 处理常规类型的输出
    template<typename T>
    DebugProxy& operator<<(const T& value) {
        if (enabled_) {
            std::cout << value;
        }
        return *this;
    }

    // 处理流操作符（如std::endl）
    DebugProxy& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (enabled_) {
            manip(std::cout);
        }
        return *this;
    }

private:
    bool enabled_;
};

class debug {
public:
    debug(bool enable=true) : enable_(enable) {}

    // 返回代理对象，支持链式<<操作
    DebugProxy log(bool on=true) {
        return DebugProxy(enable_ && on);
    }

private:
    bool enable_;
};

#endif // __DEBUG_H__