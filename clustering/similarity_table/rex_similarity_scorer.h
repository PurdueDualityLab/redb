//
// Created by charlie on 2/10/22.
//

#ifndef _REX_SIMILARITY_SCORER_H
#define _REX_SIMILARITY_SCORER_H

#include "base_similarity_scorer.h"
#include "../rex_wrapper.h"

#include <memory>
#include "re2/re2.h"

struct RexStringsHasher {
    std::size_t operator()(const std::vector<std::string> &strings) const;
};

struct AggressiveVectorDeleter {
    void operator()(std::vector<std::string> *vec) {
        // Clear all the values of vec
        vec->clear();
        // Try this method of swapping with an empty vector
        vec->shrink_to_fit();
        // Delete the pointer
        delete vec;

        std::cout << "Aggressively deleted" << std::endl;
    }
};

class RexSimilarityScorer: public BaseSimilarityScorer {
public:
    RexSimilarityScorer(const std::string &pattern, unsigned long id, const RexWrapper &rex_wrapper);
    ~RexSimilarityScorer() override;
    double score(std::shared_ptr<BaseSimilarityScorer> other_scorer) override;

    bool test_string(const std::string &subject) const;

private:
    std::unique_ptr<std::vector<std::string>, AggressiveVectorDeleter> load_strings();
    std::string strings_file_path;
    // std::unique_ptr<re2::RE2> regex;
    std::size_t strings_hash;
};


#endif //_REX_SIMILARITY_SCORER_H
