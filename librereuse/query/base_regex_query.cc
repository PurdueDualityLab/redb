//
// Created by charlie on 8/30/21.
//

#include "base_regex_query.h"

rereuse::query::BaseRegexQuery::BaseRegexQuery(rereuse::query::AbstractScore *score)
        : scorer(std::unique_ptr<rereuse::query::AbstractScore>(score)) {
}

rereuse::query::BaseRegexQuery::BaseRegexQuery(std::unique_ptr<AbstractScore> score)
        : scorer(std::move(score)) {
}

bool rereuse::query::BaseRegexQuery::test(const RE2 &regex) {
    double score = this->score(regex);
    return this->scorer->score_passes(score);
}
