//
// Created by charlie on 2/10/22.
//

#include "similarity_table.h"

SimilarityTable::SimilarityTable(const std::unordered_map<unsigned long, std::string> &patterns, unsigned int workers)
        : has_temp_abc_file(false) {
    RexWrapper rex_wrapper("/home/charlie/Downloads/Rex/Rex.exe", "/usr/bin/wine");
    ThreadPool thread_pool(workers);
    std::vector<std::future<std::shared_ptr<BaseSimilarityScorer>>> scorer_futures;
    for (const auto &[id, pattern] : patterns) {
        auto new_score_future = thread_pool.enqueue(
                [](const std::string &pat, unsigned long pat_id, RexWrapper &rex)
                {
                    try {
                        return std::shared_ptr<BaseSimilarityScorer>(new RexSimilarityScorer(pat, pat_id, rex));
                    } catch (std::runtime_error &exe) {
                        return std::shared_ptr<BaseSimilarityScorer>();
                    }
                },
                pattern, id, rex_wrapper);
        scorer_futures.push_back(std::move(new_score_future));
    }

    // take all the futures and put them in scorers
    for (auto &&future : scorer_futures) {
        future.wait();
        auto new_scorer = std::move(future.get());
        if (new_scorer)
            this->scorers.push_back(std::move(new_scorer));
    }
    std::cout << "Created " << this->scorers.size() << " scorers" << std::endl;

    // Now compute the similarity matrix
    this->scores = std::vector<std::vector<double>>(this->scorers.size());
    for (size_t row = 0; row < this->scorers.size(); row++) {
        this->scores[row] = std::vector<double>(this->scorers.size());
    }

    // Make a bunch of tasks to compute the similarity matrix
    std::vector<std::future<std::tuple<size_t, size_t, double>>> scoring_tasks;
    for (size_t row = 0; row < this->scorers.size(); row++) {
        auto row_scorer = this->scorers[row];
        for (size_t col = 0; col < this->scorers.size(); col++) {
            auto col_scorer = this->scorers[col];
            auto task = thread_pool.enqueue([row, col, row_scorer, col_scorer] {
                // Compute the score
                double score = row_scorer->score(col_scorer);
                std::cout << "Scored (" << row << ',' << col << ')' << std::endl;
                return std::tuple<size_t, size_t, double> { row, col, score };
            });
            scoring_tasks.push_back(std::move(task));
        }
    }

    // Get all the scores
    for (auto &future : scoring_tasks) {
        future.wait();
        auto [row, col, score] = future.get();
        this->scores[row][col] = score;
    }
}

SimilarityTable::~SimilarityTable() {
    // Try deleting the abc file
    if (has_temp_abc_file)
        remove(this->abc_file.c_str());
}

void SimilarityTable::to_similarity_graph() {
    for (int row = 0; row < this->scores.size(); row++) {
        int col;
        for (col = 0; col <= row; col++) {
            double average = (this->scores[row][col] + this->scores[col][row]) / 2;
            this->scores[row][col] = average;
        }

        for (col = row + 1; col < this->scores.size(); col++) {
            this->scores[row][col] = 0;
        }
    }

    for (int diag = 0; diag < this->scores.size(); diag++) {
        this->scores[diag][diag] = 1;
    }
}

std::string SimilarityTable::to_abc() {
    this->abc_file = "/tmp/abc_graph_" + std::to_string(random()) + ".abc";
    this->has_temp_abc_file = true;

    return this->to_abc(this->abc_file);
}

std::string SimilarityTable::to_abc(const std::string &abc_graph_output) {
    std::ofstream abc_fstream(abc_graph_output);

    for (int row = 0; row < this->scores.size(); row++) {
        for (int col = 0; col < this->scores.size(); col++) {
            if (this->scores[row][col] > 0) {
                abc_fstream << this->scorers[row]->get_id() << '\t' << this->scorers[col]->get_id() << '\t' << this->scores[row][col] << std::endl;
            }
        }
    }

    abc_fstream.close();

    return abc_graph_output;
}

void SimilarityTable::prune(double threshold) {
    for (auto &row : this->scores) {
        for (auto &score : row) {
            if (score < threshold)
                score = 0.0;
        }
    }
}

std::optional<std::string> SimilarityTable::get_pattern(unsigned long id) const {
    for (const auto &scorer : this->scorers) {
        if (scorer->get_id() == id) {
            return { scorer->get_pattern() };
        }
    }

    return {};
}
