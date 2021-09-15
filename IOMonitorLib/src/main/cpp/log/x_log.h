//
// Created by chenlong on 2021/9/7.
//

#ifndef IOMONITOR_X_LOG_H
#define IOMONITOR_X_LOG_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include "x_log_buffer.h"
namespace xlog {
    class XLog {
    private:
        LogBuffer buffer_;
        std::mutex mutex_;
        std::thread async_thread_;
        bool is_release_ = false;
        const char* log_dir_;
        FILE *current_log_file_;
        std::condition_variable dump_log_cv_;

    private:
        void dump_to_file();
    public:
        XLog(const char* dir_path);
        ~XLog();
        XLog(const XLog&) = delete;
        XLog& operator=(const XLog&) = delete;
        void release();
        void write(const char* msg);
        void flush();
    };
}


#endif //IOMONITOR_X_LOG_H
