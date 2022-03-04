//
// Created by charlie on 3/3/22.
//

#ifndef REDB_TIME_TASK_H
#define REDB_TIME_TASK_H

#include <functional>
#include <chrono>
#include <utility>

namespace rereuse::util {

    template <typename DurationT, typename RetT, typename ... ArgsT>
    std::pair<RetT, DurationT> time_task(std::function<RetT(ArgsT...)> task, ArgsT ...args) {

        // start timing the task
        auto start = std::chrono::high_resolution_clock::now();
        RetT result = std::invoke(task, args...);
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the duration
        auto dur = std::chrono::duration_cast<DurationT>(end - start);

        return { result, dur };
    }
}

#endif //REDB_TIME_TASK_H
