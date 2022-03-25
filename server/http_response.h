//
// Created by charlie on 3/9/22.
//

#ifndef REDB_HTTP_RESPONSE_H
#define REDB_HTTP_RESPONSE_H

#include <unordered_map>
#include <string>
#include <utility>

#include <mongoose.h>
#include "nlohmann/json.hpp"

struct HttpResponse {

    static HttpResponse invalid_endpoint_verb() {
        nlohmann::json err_obj;
        err_obj["code"] = 400;
        err_obj["message"] = "The verb requested for this endpoint is not supported";
        std::stringstream buffer;
        buffer << err_obj;

        return HttpResponse(400, {{"Content-Type", "application/json"}}, buffer.str());
    }

    static HttpResponse invalid_endpoint_mapping() {
        nlohmann::json err_obj;
        err_obj["code"] = 400;
        err_obj["message"] = "The endpoint you requested is not mapped";
        std::stringstream buffer;
        buffer << err_obj;

        return HttpResponse(400, {{"Content-Type", "application/json"}}, buffer.str());
    }

    explicit HttpResponse(int status)
    : status(status) { }

    HttpResponse(int status, std::unordered_map<std::string, std::string> headers, std::string body)
    : status(status)
    , headers(std::move(headers))
    , body_payload(std::move(body))
    { }

    void enqueue(struct mg_connection *connection);

    int status;
    std::unordered_map<std::string, std::string> headers;
    std::string body_payload;
};

#endif //REDB_HTTP_RESPONSE_H
