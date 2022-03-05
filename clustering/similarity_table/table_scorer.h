//
// Created by charlie on 3/4/22.
//

#ifndef REDB_TABLE_SCORER_H
#define REDB_TABLE_SCORER_H


#include "../ThreadPool.h"
#include "base_similarity_scorer.h"

/**
 * Class dedicated for doing the computational workload for scoring the similarity table.
 * The goal of this class is to do that process in the most resource-efficient way possible.
 */
class TableScorer {
public:
    using task_result = std::tuple<size_t, std::vector<double>>;
    using task_type = std::function<task_result(const std::vector<std::shared_ptr<BaseSimilarityScorer>>&)>;

    TableScorer(ThreadPool &thread_pool, const std::vector<std::shared_ptr<BaseSimilarityScorer>> &scorers);

    std::vector<std::vector<double>> score();

private:
    ThreadPool &thread_pool;
    const std::vector<std::shared_ptr<BaseSimilarityScorer>> &scorers;
};


#endif //REDB_TABLE_SCORER_H
