//
// Created by charlie on 4/4/22.
//

#ifndef REDB_STRING_VECTOR_H
#define REDB_STRING_VECTOR_H

#include <set>
#include <vector>
#include <string>

class StringVector {
public:
    explicit StringVector(const std::vector<std::string> &patterns);

    [[nodiscard]] std::vector<std::string> create_vector() const;

private:
    std::set<std::string> all_strings;
};


#endif //REDB_STRING_VECTOR_H
