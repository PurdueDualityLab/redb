//
// Created by charlie on 4/4/22.
//

#ifndef REDB_FEATURE_MATRIX_H
#define REDB_FEATURE_MATRIX_H

#include <map>
#include <string>
#include "feature_vector.h"
#include "string_vector.h"

class FeatureMatrix {
public:
    explicit FeatureMatrix(const std::vector<std::string> &patterns);

    const std::map<std::string, FeatureVector> &get_feature_vectors() const {
        return feature_vectors;
    }

private:
    std::map<std::string, FeatureVector> feature_vectors;
};

std::ostream &operator<<(std::ostream &os, const FeatureMatrix &matrix);

#endif //REDB_FEATURE_MATRIX_H
