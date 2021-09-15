//
// Created by chenlong on 2021/9/14.
//
#define TAG "BufferHeader"

#include <memory>
#include "x_log_buffer_header.h"
#include "../io_log.h"

xlog::BufferHeader::BufferHeader() : magic_(0), version_(0), header_len_(0), buffer_pos_(0),
                                     buffer_(nullptr) {}

xlog::BufferHeader::BufferHeader(char *buffer) : buffer_(buffer) {
    LOGI("init header...%p", buffer);
    uint32_t magic;
    memcpy(&magic, magic_ptr(buffer), sizeof(uint32_t));
    uint32_t version;
    memcpy(&version, version_ptr(buffer), sizeof(uint32_t));
    if (magic == kMagic && version == kVersion) {
        read_exist_header();
    } else {
        buffer_pos_ = 0;
        make_buffer_header();
    }
}

xlog::BufferHeader::~BufferHeader() = default;

uint32_t xlog::BufferHeader::get_header_len() const {
    return kHeaderLen;
}

uint32_t xlog::BufferHeader::get_buffer_pos() const {
    return buffer_pos_;
}

void xlog::BufferHeader::increase_buffer_pos(uint32_t pos) {
    buffer_pos_ += pos;
    memcpy(buffer_pos_ptr(buffer_), &buffer_pos_, sizeof(uint32_t));
}

void xlog::BufferHeader::reset_buffer() {
    buffer_pos_ = 0;
    memcpy(buffer_pos_ptr(buffer_), &buffer_pos_, sizeof(uint32_t));
}

void xlog::BufferHeader::read_exist_header() {
    memcpy(&magic_, magic_ptr(buffer_), sizeof(uint32_t));
    memcpy(&version_, version_ptr(buffer_), sizeof(uint32_t));
    memcpy(&header_len_, header_len_ptr(buffer_), sizeof(uint32_t));
    memcpy(&buffer_pos_, buffer_pos_ptr(buffer_), sizeof(uint32_t));
    LOGI("---------------------------------------------");
    LOGI("magic:0x%x", magic_);
    LOGI("version:%d", version_);
    LOGI("header_len:%d", header_len_);
    LOGI("buffer_pos:%d", buffer_pos_);
    LOGI("---------------------------------------------");
}

void xlog::BufferHeader::make_buffer_header() {
    memcpy(magic_ptr(buffer_), &magic_, sizeof(uint32_t));
    memcpy(version_ptr(buffer_), &version_, sizeof(uint32_t));
    memcpy(header_len_ptr(buffer_), &header_len_, sizeof(uint32_t));
    memcpy(buffer_pos_ptr(buffer_), &buffer_pos_, sizeof(uint32_t));
}
