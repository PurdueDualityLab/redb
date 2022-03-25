//
// Created by charlie on 3/9/22.
//

#include "query_handler.h"
#include "nlohmann/json.hpp"

static nlohmann::json filter_array_nulls(const nlohmann::json &list) {
    if (!list.is_array())
        return list;

    nlohmann::json list_cpy = list;

    // Remove all null values
    for (auto it = list_cpy.begin(); it != list_cpy.end();) {
        if (it->is_null()) {
            it = list_cpy.erase(it);
        } else {
            ++it;
        }
    }

    return list_cpy;
}

static std::shared_ptr<rereuse::query::BaseClusterQuery> read_query_from_body(struct mg_str body_string, unsigned int *err_code) {
    std::string body_buffer(body_string.ptr, body_string.len);
    nlohmann::json body_obj;
    try {
        body_obj = std::move(nlohmann::json::parse(body_buffer));
    } catch (nlohmann::json::parse_error &exe) {
        LOG(LL_ERROR, ("Error while parsing request body: %s", exe.what()));
        *err_code = 1;
        return {};
    }

    // Get positive_examples
    if (!body_obj.contains("positive") || !body_obj.contains("negative")) {
        *err_code = 2;
        return {}; // Return a null pointer
    }

    // Both must be arrays
    if (!body_obj["positive"].is_array() || !body_obj["negative"].is_array()) {
        *err_code = 3;
        return {};
    }

    auto positive = filter_array_nulls(body_obj["positive"]).get<std::unordered_set<std::string>>();
    auto negative = filter_array_nulls(body_obj["negative"]).get<std::unordered_set<std::string>>();
    return std::make_shared<rereuse::query::ClusterMatchQuery>(positive, negative);
}

QueryHandler::QueryHandler(rereuse::db::RegexClusterRepository *repo)
: repo(repo) {
}

HttpResponse QueryHandler::post(struct mg_http_message *http_message) {

    LOG(LL_VERBOSE_DEBUG, ("Starting to handle query request..."));
    // Get a query from the body
    unsigned int err_code = 0;
    auto query = read_query_from_body(http_message->body, &err_code);
    if (!query) {
        // Couldn't get the proper query
        nlohmann::json err;
        err["code"] = 400;
        if (err_code == 1) {
            err["message"] = "Could not parse body";
        } else if (err_code == 2) {
            err["message"] = "Could not build query object. Must specify both positive and negative examples";
        } else if (err_code == 3) {
            err["message"] = "Could not build query object. positive and negative must be arrays";
        } else {
            err["message"] = "Unexpected error occurred";
        }
        LOG(LL_ERROR, ("Error while evaluating query: %s", err["message"].get<std::string>().c_str()));

        // mg_http_reply(connection, 400, "Content-Type: application/json\r\n", err.dump().c_str());
        HttpResponse err_response(400, { {"Content-Type", "application/json"} }, err.dump());
        return err_response;
    }

    // Execute the query on the repository
    // auto results = ->query(query);
    auto results = this->repo->query(query);
    LOG(LL_VERBOSE_DEBUG, ("Found %lu results. Truncating to first 100", results.size()));
    std::unordered_set<std::string> truncated_results;
    auto end = results.begin();
    unsigned long items_to_take = std::min<unsigned long>(results.size(), 100);
    std::advance(end, items_to_take);
    std::move(results.begin(), end, std::inserter(truncated_results, truncated_results.begin()));

    // Serialize the data
    nlohmann::json results_obj;
    results_obj["results"] = truncated_results;
    results_obj["total_results_size"] = results.size();
    results_obj["truncated_results_size"] = truncated_results.size();
    std::stringstream results_buffer;
    results_buffer << results_obj;
    LOG(LL_INFO, ("Found results: %s\n", results_buffer.str().c_str()));

    return HttpResponse(200, {{"Content-Type", "application/json"}}, results_buffer.str());
}

HttpResponse QueryHandler::options(struct mg_http_message *http_message) {
    HttpResponse response(204);
    std::unordered_map<std::string, std::string> headers = {
            { "Access-Control-Allow-Origin", "*" },
            { "Access-Control-Allow-Methods", "GET, POST" },
            { "Access-Control-Allow-Headers", "Accept, Content-Type, Referer, User-Agent" },
            { "Access-Control-Max-Age", "86400" }
    };

    response.headers = std::move(headers);

    return response;
}
