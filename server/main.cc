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
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include "args_parser.h"
#include "endpoint_handler.h"
#include "query_handler.h"
#include "tracker_handler.h"

static int interrupted = 0;

static void handle_signal(int sig) {
    LOG(LL_INFO, ("Encountered signal %d, starting to terminated...", sig));
    interrupted = 1;
    std::flush(std::cout);
    std::flush(std::cerr);
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

static void handle(struct mg_connection *connection, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        // See if this event is a post to /query
        auto http_msg = (struct mg_http_message *) ev_data;
        std::string method(http_msg->method.ptr, http_msg->method.len);
        std::string uri(http_msg->uri.ptr, http_msg->uri.len);
        std::string body(http_msg->body.ptr, http_msg->body.len);
        LOG(LL_INFO, ("HTTP request: %s %s", method.c_str(), uri.c_str()));

        auto endpoints = static_cast<std::unordered_map<std::string, std::unique_ptr<EndpointHandler>>*>(fn_data);
        for (const auto &[endpoint, handler] : *endpoints) {
            // see if this handler has what we want
            if (mg_http_match_uri(http_msg, endpoint.c_str())) {
                LOG(LL_DEBUG, ("Found handler for %s", endpoint.c_str()));
                auto response = handler->handle(method, http_msg);
                response.enqueue(connection);
                LOG(LL_DEBUG, ("Finished handling connection"));
                // Flush any logs
                std::flush(std::cout);
                std::flush(std::cerr);
                return;
            }
        }

        // We didn't have a valid endpoint handler
        LOG(LL_ERROR, ("Invalid request to endpoint"));
        HttpResponse::invalid_endpoint_mapping().enqueue(connection);
        LOG(LL_DEBUG, ("Finished handling connection"));
        // Flush any logs
        std::flush(std::cout);
        std::flush(std::cerr);
    } else if (ev == MG_EV_HTTP_CHUNK) {
        LOG(LL_INFO, ("Encountered chunking event"));
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

    // Setup api
    Aws::Auth::EnvironmentAWSCredentialsProvider cred_provider;

    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);

    // Set up endpoint handlers
    auto *endpoint_mappings = new std::unordered_map<std::string, std::unique_ptr<EndpointHandler>>();
    (*endpoint_mappings)["/query"] = std::make_unique<QueryHandler>(repository);
    (*endpoint_mappings)["/track"] = std::make_unique<TrackerHandler>(cred_provider.GetAWSCredentials());

    // Get connection string
    std::stringstream connection_url;
    connection_url << "http://0.0.0.0:" << args.get_port();

    // Setup the server
    mg_mgr manager {};
    mg_log_set("3"); // LOG EVERYTHING!!!!
    mg_mgr_init(&manager);
    LOG(LL_INFO, ("Server starting to listen..."));
    mg_http_listen(&manager, connection_url.str().c_str(), &handle, (void *) endpoint_mappings);
    LOG(LL_INFO, ("Server started on port %d", args.get_port()));
    // Flush the buffers...
    std::flush(std::cout);
    std::flush(std::cerr);
    while (!interrupted)
        mg_mgr_poll(&manager, 1000);
    mg_mgr_free(&manager);
    delete endpoint_mappings;
    Aws::ShutdownAPI(options);
    return 0;
}
