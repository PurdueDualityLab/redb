//
// Created by charlie on 2/9/22.
//

#ifndef _REX_WRAPPER_H
#define _REX_WRAPPER_H

#include <string>
#include <utility>
#include <vector>

class RexWrapper {
public:
    RexWrapper(std::string rex_path, std::string wine_path)
    : rex_path(std::move(rex_path))
    , wine_path(std::move(wine_path))
    { }

    std::vector<std::string> generate_strings(const std::string &pattern, unsigned int count) const;

private:
    std::string rex_path;
    std::string wine_path;
};


#endif //_REX_WRAPPER_H
