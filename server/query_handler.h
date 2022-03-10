//
// Created by charlie on 3/9/22.
//

#ifndef REDB_QUERY_HANDLER_H
#define REDB_QUERY_HANDLER_H

#include <memory>
#include "db/regex_cluster_repository.h"
#include "http_response.h"
#include "endpoint_handler.h"
#include <mongoose.h>

class QueryHandler : public EndpointHandler {
public:
    explicit QueryHandler(rereuse::db::RegexClusterRepository *repo);
    HttpResponse post(struct mg_http_message *http_message) override;
    HttpResponse options(struct mg_http_message *http_message) override;

private:
    std::unique_ptr<rereuse::db::RegexClusterRepository> repo;
};


#endif //REDB_QUERY_HANDLER_H
