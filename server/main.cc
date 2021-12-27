//
// Created by charlie on 12/25/21.
//

#include <iostream>
#include <csignal>

#include <nlohmann/json.hpp>
#include <mongoose.h>

#include <db/regex_cluster_repository.h>
#include <db/parallel_regex_cluster_repository.h>
#include <db/pattern_reader.h>
#include "args_parser.h"

static int interrupted = 0;

static void handle_signal(int sig) {
    LOG(LL_INFO, ("Encountered signal %d, starting to terminated...", sig));
    interrupted = 1;
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
    auto positive = body_obj["positive"].get<std::unordered_set<std::string>>();
    auto negative = body_obj["negative"].get<std::unordered_set<std::string>>();
    return std::make_shared<rereuse::query::ClusterMatchQuery>(positive, negative);
}

static void handle(struct mg_connection *connection, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        // See if this event is a post to /query
        auto http_msg = (struct mg_http_message *) ev_data;
        std::string method(http_msg->method.ptr, http_msg->method.len);
        std::string uri(http_msg->uri.ptr, http_msg->uri.len);
        LOG(LL_INFO, ("HTTP request: %s %s", method.c_str(), uri.c_str()));
        if (mg_http_match_uri(http_msg, "/query")) {
            if (method == "POST") {
                LOG(LL_VERBOSE_DEBUG, ("Starting to handle query request..."));
                auto repository = (rereuse::db::ParallelRegexClusterRepository *) fn_data;
                // Get a query from the body
                unsigned int err_code = 0;
                auto query = read_query_from_body(http_msg->body, &err_code);
                if (!query) {
                    // Couldn't get the proper query
                    nlohmann::json err;
                    err["code"] = 400;
                    if (err_code == 1) {
                        err["message"] = "Could not parse body";
                    } else if (err_code == 2) {
                        err["message"] = "Could not build query object. Must specify both positive and negative examples";
                    } else {
                        err["message"] = "Unexpected error occurred";
                    }
                    LOG(LL_ERROR, ("Error while evaluating query: %s", err["message"].get<std::string>().c_str()));

                    mg_http_reply(connection, 400, "Content-Type: application/json\r\n", err.dump().c_str());
                    return;
                }

                // Execute the query on the repository
                auto results = repository->query(query);
                // mg_log("Found %lu results", results.size());
                LOG(LL_VERBOSE_DEBUG, ("Found %lu results", results.size()));

                // Serialize the data
                nlohmann::json results_obj;
                results_obj["results"] = results;
                std::stringstream results_buffer;
                results_buffer << results_obj;

                // Return the data
                mg_http_reply(connection, 200, "Content-Type: application/json\r\n", results_buffer.str().c_str());
            } else if (method == "OPTIONS") {
                // Set a 200
                std::stringstream header_buffer;
                header_buffer << "Access-Control-Allow-Origin: *\r" << std::endl;
                header_buffer << "Access-Control-Allow-Methods: GET, POST\r" << std::endl;
                header_buffer << "Access-Control-Allow-Headers: Accept, Content-Type, Referer, User-Agent\r" << std::endl;
                header_buffer << "Access-Control-Max-Age: 86400\r" << std::endl;
                mg_http_reply(connection, 204, header_buffer.str().c_str(), nullptr);
            } else {
                LOG(LL_ERROR, ("Invalid request to endpoint"));
                nlohmann::json err_obj;
                err_obj["code"] = 400;
                err_obj["message"] = "The verb you requested is not handled for this endpoint";
                std::stringstream buffer;
                buffer << err_obj;
                mg_http_reply(connection, 400, "Content-Type: application/json\r\n", buffer.str().c_str());
            }
        } else {
            LOG(LL_ERROR, ("Invalid request to endpoint"));
            nlohmann::json err_obj;
            err_obj["code"] = 400;
            err_obj["message"] = "The endpoint you requested is not mapped";
            std::stringstream buffer;
            buffer << err_obj;
            mg_http_reply(connection, 400, "Content-Type: application/json\r\n", buffer.str().c_str());
        }
        // Flush any logs
        std::flush(std::cout);
        std::flush(std::cerr);
    }
}

int main(int argc, char **argv) {

    // parse up dem args
    redb::server::ArgsParser args(argc, argv);
    if (args.get_db_seed_path().empty()) {
        throw std::runtime_error("Path to db seed file not specified");
    }

    // install a signal handler to quit app when it's terminated
    struct sigaction event_handler;
    sigemptyset(&event_handler.sa_mask);
    event_handler.sa_handler = &handle_signal;
    event_handler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &event_handler, nullptr);
    sigaction(SIGTERM, &event_handler, nullptr);

    // Setup repository
    LOG(LL_INFO, ("Starting to load clusters..."));
    auto clusters = rereuse::db::read_semantic_clusters(args.get_db_seed_path());
    LOG(LL_INFO, ("Loaded clusters. Starting to set up repository..."));
    auto *repository = new rereuse::db::ParallelRegexClusterRepository(std::thread::hardware_concurrency() - 1);
    for (auto &cluster : clusters) {
        repository->add_cluster(std::move(cluster));
    }
    LOG(LL_INFO, ("Repository initialized"));

    // Get connection string
    std::stringstream connection_url;
    connection_url << "http://localhost:" << args.get_port();

    // Setup the server
    struct mg_mgr manager;
    mg_log_set("3"); // LOG EVERYTHING!!!!
    mg_mgr_init(&manager);
    LOG(LL_INFO, ("Server starting to listen..."));
    mg_http_listen(&manager, connection_url.str().c_str(), &handle, (void *) repository);
    LOG(LL_INFO, ("Server started on port %d", args.get_port()));
    // Flush the buffers...
    std::flush(std::cout);
    std::flush(std::cerr);
    while (!interrupted)
        mg_mgr_poll(&manager, 1000);
    mg_mgr_free(&manager);
    delete repository;
    return 0;
}
