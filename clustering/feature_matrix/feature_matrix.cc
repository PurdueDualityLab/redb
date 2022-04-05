//
// Created by charlie on 4/4/22.
//

#include "feature_matrix.h"
#include "nlohmann/json.hpp"

FeatureMatrix::FeatureMatrix(const std::vector<std::string> &patterns) {
    // Create the string vector
    StringVector strings(patterns);

    // Build feature vectors for each one
    for (const auto &pattern : patterns) {
        try {
            FeatureVector feature_vec(pattern, strings);
            this->feature_vectors[pattern] = std::move(feature_vec);
        } catch (std::runtime_error &exe) {
            // Skip this vector...
        }
    }
}

std::ostream &operator<<(std::ostream &os, const FeatureMatrix &matrix) {
    nlohmann::json complete_obj;
    for (const auto &[pattern, vec] : matrix.get_feature_vectors()) {
        complete_obj[pattern] = vec.get_match_num_vector();
    }

    os << complete_obj;
    return os;
}
