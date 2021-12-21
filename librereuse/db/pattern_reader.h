//
// Created by charlie on 9/19/21.
//

#ifndef _PATTERNREADER_H
#define _PATTERNREADER_H

#include <istream>
#include <string>
#include <vector>

namespace rereuse::db {
    std::vector<std::string> read_patterns(std::istream &input_stream);
    std::vector<std::string> read_patterns_from_path(const std::string &path);
}

#endif //_PATTERNREADER_H
