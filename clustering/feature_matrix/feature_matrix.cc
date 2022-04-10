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
            FeatureVector feature_vec(pattern, "none", strings);
            this->feature_vectors[pattern] = std::move(feature_vec);
        } catch (std::runtime_error &exe) {
            // Skip this vector...
        }
    }
}

FeatureMatrix::FeatureMatrix(const std::unordered_map<std::string, std::vector<std::string>> &labeled_clusters) {

    // Flatten all the patterns into a single vector for string vector generation
    std::vector<std::string> all_patterns;
    for (const auto &[_, patterns] : labeled_clusters) {
        std::copy(patterns.cbegin(), patterns.cend(), std::back_inserter(all_patterns));
    }

    StringVector strings(all_patterns);

    for (const auto &[cluster, patterns] : labeled_clusters) {
        for (const auto &pattern : patterns) {
            try {
                FeatureVector vec(pattern, cluster, strings);
                this->feature_vectors[pattern] = std::move(vec);
            } catch (std::runtime_error &exe) {
                // This vector should not be added
            }
        }
    }
}

std::ostream &operator<<(std::ostream &os, const FeatureMatrix &matrix) {
    nlohmann::json complete_obj;
    for (const auto &[pattern, vec] : matrix.get_feature_vectors()) {
        nlohmann::json feature_vec_obj;
        feature_vec_obj["pattern"] = vec.get_pattern();
        feature_vec_obj["label"] = vec.get_classification();
        feature_vec_obj["similarity_vector"] = vec.get_match_num_vector();
        complete_obj.push_back(feature_vec_obj);
    }

    os << complete_obj;
    return os;
}
