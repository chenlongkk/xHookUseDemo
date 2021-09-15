//
// Created by chenlong on 2021/9/8.
//

#ifndef IOMONITOR_X_LOG_CONSTANT_H
#define IOMONITOR_X_LOG_CONSTANT_H

namespace xlog {
    constexpr size_t kBUFFER_SIZE = 150 * 1024;
    constexpr int kDUMP_LEN = (int) (kBUFFER_SIZE * 1 / 3);
}
#endif //IOMONITOR_X_LOG_CONSTANT_H
