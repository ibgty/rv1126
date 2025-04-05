#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdexcept>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

     /*
     * @brief 推送元素到队列（移动语义）
     * @param item 要推送的元素
     */
    void push(T&& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(item));
        m_cond.notify_one();
    }

     /*
     * @brief 尝试从队列弹出元素（非阻塞）
     * @param item 接收弹出元素的引用
     * @return 是否成功弹出元素
     */
    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) {
            return false;
        }
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

     /*
     * @brief 阻塞弹出元素
     * @param item 接收弹出元素的引用
     * @param timeout 超时时间（默认无限等待）
     * @return 是否在超时前获得元素
     */
    template<typename Rep, typename Period>
    bool pop(T& item, const std::chrono::duration<Rep, Period>& timeout = std::chrono::seconds(0)) {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (timeout.count() > 0) {
            // 带超时等待
            if (!m_cond.wait_for(lock, timeout, [this] { 
                return !m_queue.empty() || m_closed; 
            })) {
                return false; // 超时
            }
        } else {
            // 无限等待
            m_cond.wait(lock, [this] { 
                return !m_queue.empty() || m_closed; 
            });
        }

        if (m_closed) {
            throw std::runtime_error("Queue is closed");
        }

        if (m_queue.empty()) {
            return false; // 防止虚假唤醒
        }

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

     /*
     * @brief 关闭队列，唤醒所有等待线程
     */
    void shutdown() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_closed = true;
        m_cond.notify_all();
    }

     /*
     * @brief 检查队列是否为空
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

     /*
     * @brief 获取队列元素数量
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<T> m_queue;
    bool m_closed = false;
};

#endif // THREAD_SAFE_QUEUE_HPP