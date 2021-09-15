//
// Created by chenlong on 2021/9/7.
//
#define TAG "IOBUFFER"
#include "x_log_buffer.h"
#include "x_log_constant.h"
#include <fcntl.h>
#include <unistd.h>
#include "../io_log.h"

#include <sys/mman.h>

xlog::LogBuffer::LogBuffer(const char* file_dir) {
    char file_path[PATH_MAX];
    snprintf(file_path, sizeof(file_path), "%s/%s", file_dir,"buffer.mmap");
    char *mmap_buffer = static_cast<char*>(open_mmap_file(file_path, kBUFFER_SIZE));
    if (mmap_buffer != nullptr) {
        use_mmap_ = true;
    } else {
        use_mmap_ = false;
        mmap_buffer = static_cast<char*>(malloc(kBUFFER_SIZE));
    }
    msg_buffer_ = mmap_buffer;
    header_ = BufferHeader(msg_buffer_);
    current_ptr_ = (char *)msg_buffer_ + header_.get_header_len() + header_.get_buffer_pos();
}

xlog::LogBuffer::~LogBuffer() {
    if (use_mmap_) {
        munmap(msg_buffer_, kBUFFER_SIZE);
    } else {
        free(msg_buffer_);
    }
    msg_buffer_ = nullptr;
    current_ptr_ = nullptr;
}

int xlog::LogBuffer::length() {
    return header_.get_buffer_pos();
}

void *xlog::LogBuffer::open_mmap_file(const char *path, size_t buffer_size) {
    LOGD("mmap file, %s", path);
    int fd = open(path, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0) {
        LOGI("%s", strerror(errno));
        return nullptr;
    }
    ftruncate(fd, buffer_size);
    lseek(fd, 0, SEEK_SET);
    void* map_buffer = mmap(nullptr, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (map_buffer != MAP_FAILED) {
        LOGI("map success");
        return map_buffer;
    } else {
        LOGI("mmap fail, %s", strerror(errno));
        return nullptr;
    }
}

bool xlog::LogBuffer::append(const char *msg) {
    size_t msg_len = strlen(msg);
    memcpy(current_ptr_, msg, msg_len);
    memcpy(current_ptr_ + msg_len, "\n", 1);
    current_ptr_ = current_ptr_ + msg_len + 1;
    header_.increase_buffer_pos(msg_len + 1);
    return true;
}

void xlog::LogBuffer::flush(void *target) {
    int len = header_.get_buffer_pos();
    char *data = msg_buffer_ + header_.get_header_len();
    memcpy(target, data, len);
    header_.reset_buffer();
    current_ptr_ = data;
    memset(current_ptr_, 0, kBUFFER_SIZE - header_.get_header_len());
}



