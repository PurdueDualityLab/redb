//
// Created by charlie on 3/4/22.
//

#include "table_scorer.h"

#include <iostream>
#include <chrono>
using namespace std::chrono_literals;

TableScorer::TableScorer(ThreadPool &thread_pool, const std::vector<std::shared_ptr<BaseSimilarityScorer>> &scorers)
: thread_pool(thread_pool)
, scorers(scorers)
, rows_scored(0) {
}

template <typename R>
static bool is_ready(const std::future<R> &fut) {
    auto status = fut.wait_for(0s);
    return status == std::future_status::ready;
}

template <typename TaskType>
static int has_ready_task(std::vector<std::future<TaskType>> &tasks) {
    for (int idx = 0; idx < tasks.size(); idx++) {
        if (is_ready(tasks[idx])) {
            return idx;
        }
    }

    return -1;
}

std::vector<std::vector<double>> TableScorer::score() {

    // Start with a small vector of tasks
    std::vector<std::future<task_result>> tasks;
    // Initialize similarity matrix
    std::vector<std::vector<double>> table(this->scorers.size());

    // For each row in the scorer table, enqueue a scoring task, but enqueue
    // lazily
    for (size_t row = 0; row < this->scorers.size(); row++) {
        // first, see if tasks is full
        if (tasks.size() == this->thread_pool.thread_count()) {
            // tasks are full, so wait for a task to become ready
            int ready_task;
            // Block until a task is ready
            while ((ready_task = has_ready_task(tasks)) < 0)
                ;

            // ready_task is now set with the task that is done, so grab it
            auto [scored_row, scores] = tasks[ready_task].get();
            table[scored_row] = std::move(scores);

            // A row has been scored, update accordingly
            std::cout << "Scored " << ++this->rows_scored << "/" << this->scorers.size() << " rows" << std::endl;

            // Remove that task and slide everything down
            auto ready_task_loc = tasks.begin() + ready_task;
            tasks.erase(ready_task_loc);
        }

        // There is space in the tasks, so create a new scoring task...
        auto row_scorer = this->scorers[row];
        task_type new_task = [row, row_scorer](const std::vector<std::shared_ptr<BaseSimilarityScorer>> &all_scorers) {
            std::vector<double> row_scores(all_scorers.size());
            // Score everything in the row
            for (unsigned int col = 0; col < all_scorers.size(); col++) {
                row_scores[col] = row_scorer->score(all_scorers[col]);
            }

            return std::make_tuple(row, row_scores);
        };

        // ... and enqueue it
        auto task = this->thread_pool.enqueue(new_task, this->scorers);

        // Push the task to the back. Pushing to the back should not overfill the vector because
        // completed tasks are removed
        tasks.push_back(std::move(task));
    }

    // wait on the rest of the tasks
    while (!tasks.empty()) {
        auto uncollected_task = std::move(tasks.back());
        tasks.pop_back();

        auto [row, scores] = uncollected_task.get();
        table[row] = std::move(scores);
    }

    // Table is done
    return table;
}
