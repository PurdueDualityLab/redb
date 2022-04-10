//
// Created by charlie on 4/4/22.
//

#ifndef REDB_FEATURE_VECTOR_H
#define REDB_FEATURE_VECTOR_H

#include <memory>
#include <re2/re2.h>
#include "string_vector.h"

class FeatureVector {
public:
    FeatureVector() {
        std::fill(this->match_vector.begin(), this->match_vector.end(), false);
    }

    FeatureVector(const std::string& pattern, std::string classification, const StringVector &string_vec);

    const std::vector<bool> &get_match_vector() const {
        return match_vector;
    }

    std::vector<unsigned char> get_match_num_vector() const {
        std::vector<unsigned char> ones_and_zeros;
        std::transform(this->match_vector.cbegin(), this->match_vector.cend(), std::back_inserter(ones_and_zeros),
                       [](bool v) { return v ? 1 : 0; });

        return ones_and_zeros;
    }

    std::string get_pattern() const {
        return this->regex->pattern();
    }

    const std::string &get_classification() const {
        return classification;
    }

private:
    std::shared_ptr<re2::RE2> regex;
    std::vector<bool> match_vector;
    std::string classification;
};


#endif //REDB_FEATURE_VECTOR_H
