//
// Created by chenlong on 2021/9/7.
//
#define TAG "XLOG"

#include "x_log.h"
#include "x_log_constant.h"
#include <thread>
#include <chrono>
#include "../io_log.h"


xlog::XLog::XLog(const char *dir_path) :
        log_dir_(dir_path),
        current_log_file_(nullptr),
        buffer_(dir_path),
        async_thread_(std::bind(&XLog::dump_to_file, this)) {
    time_t raw_time;
    time(&raw_time);
    struct tm *timestamp = localtime(&raw_time);
    char log_file_name[64] = {0};
    strftime(log_file_name, sizeof(log_file_name), "log_%Y%m%d.txt", timestamp);
    char log_file_path[PATH_MAX] = {0};
    snprintf(log_file_path, sizeof(log_file_path), "%s/%s", dir_path, log_file_name);
    current_log_file_ = fopen(log_file_path, "a+");
}

xlog::XLog::~XLog() {
    LOGI("%s", "delete");
}

void xlog::XLog::release() {
    std::unique_lock<std::mutex> lck(mutex_);
    is_release_ = true;
    dump_log_cv_.notify_all();
    lck.unlock();
    if (async_thread_.joinable()) {
        async_thread_.join();
    }
}

void xlog::XLog::dump_to_file() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        int len = buffer_.length();
        if (len > 0) {
            LOGI("dump log to file");
            int buf_len = len + 1;
            char *dup = (char *) malloc(buf_len);
            buffer_.flush(dup);
            lock.unlock();
            memset(dup + len, '\0', 1);
            if (current_log_file_ != nullptr) {
                fwrite(dup, buf_len, 1, current_log_file_);
                fflush(current_log_file_);
            }
            free(dup);
            lock.lock();
        }
        if (is_release_) {
            LOGI("release dump thread...");
            break;
        }
        dump_log_cv_.wait(lock);
    }
}

void xlog::XLog::write(const char *msg) {
    std::unique_lock<std::mutex> lck(mutex_);
    buffer_.append(msg);
    int len = buffer_.length();
    if (len > xlog::kDUMP_LEN) {
        dump_log_cv_.notify_all();
    }
}

void xlog::XLog::flush() {
    std::unique_lock<std::mutex> lck(mutex_);
    dump_log_cv_.notify_all();
    LOGI("%s", "notify flush");
}
