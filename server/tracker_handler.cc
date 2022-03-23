//
// Created by charlie on 3/22/22.
//

#include "tracker_handler.h"

#include <ctime>

#include <re2/re2.h>
#include <aws/dynamodb/model/GetItemRequest.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/ScanRequest.h>
#include <aws/core/utils/UUID.h>

static std::string generate_current_iso_timestamp() {
    const char *iso_timestamp_format = "%Y-%m-%dT%H:%M:%SZ";

    // Get the time
    time_t tm;
    time(&tm);

    // Create the buffer
    std::array<char, 30> timestamp_buf {};
    strftime(timestamp_buf.data(), timestamp_buf.size(), iso_timestamp_format, gmtime(&tm));

    return { timestamp_buf.data() };
}

TrackerHandler::TrackerHandler(const Aws::Auth::AWSCredentials& credentials)
: client_config()
, dynamo_client(credentials, this->client_config)
{
}

HttpResponse TrackerHandler::post(struct mg_http_message *http_message) {
    LOG(LL_VERBOSE_DEBUG, ("Starting to handle tracking POST request"));

    // Get the body out of the http message
    std::string body_buf(http_message->body.ptr, http_message->body.len);
    // Parse the json
    nlohmann::json event;
    try {
        event = std::move(nlohmann::json::parse(body_buf));
    } catch (nlohmann::json::parse_error &exe) {
        nlohmann::json err_obj;
        err_obj["code"] = 400;
        err_obj["message"] = "Could not parse body";
        return HttpResponse(400, { {"Content-Type", "application/json"} }, err_obj.dump());
    }

    // Make the create request
    Aws::DynamoDB::Model::PutItemRequest request;
    request.SetTableName(TrackerHandler::TABLE_NAME.data());
    Aws::DynamoDB::Model::AttributeValue time, eventId, taskId, participantId, positiveExamples, negativeExamples;
    // Create a new uuid
    auto uuid = Aws::Utils::UUID::RandomUUID();
    eventId.SetS(uuid);
    time.SetS(generate_current_iso_timestamp());
    taskId.SetS(event.at("taskId"));
    participantId.SetS(event.at("participantId"));
    for (const auto &ex : event.at("positiveExamples")) {
        positiveExamples.AddSItem(ex);
    }

    for (const auto &ex : event.at("negativeExamples")) {
        negativeExamples.AddSItem(ex);
    }

    request.AddItem("time", time);
    request.AddItem("eventId", eventId);
    request.AddItem("taskId", taskId);
    request.AddItem("participantId", participantId);
    request.AddItem("positiveExamples", positiveExamples);
    request.AddItem("negativeExamples", negativeExamples);

    const Aws::DynamoDB::Model::PutItemOutcome result = this->dynamo_client.PutItem(request);
    if (!result.IsSuccess()) {
        LOG(LL_ERROR, ("Error while POSTING request: %s", result.GetError().GetMessage().c_str()));
        nlohmann::json err_obj;
        err_obj["code"] = 500;
        err_obj["message"] = "Error while posting event to server";
        err_obj["error"] = result.GetError().GetMessage();
        return HttpResponse(500, { {"Content-Type", "application/json"} }, err_obj.dump());
    }

    // Otherwise successful
    LOG(LL_VERBOSE_DEBUG, ("Successfully logged query event"));
    nlohmann::json obj;
    for (const auto &[key, value] : result.GetResult().GetAttributes()) {
        obj[key] = value.GetS();
    }

    return HttpResponse(201);
}

static std::map<std::string, std::string> parse_query_params(const std::string& params) {
    static re2::LazyRE2 parser = {"(\\w+)=(\\w+)"};
    std::map<std::string, std::string> vars;
    std::string key, value;
    re2::StringPiece params_string(params);
    while (re2::RE2::FindAndConsume(&params_string, *parser, &key, &value)) {
        vars[key] = value;
    }

    return vars;
}

static nlohmann::json
prepare_participant_report(const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>>& items) {
    // Get the task and participant ids from the first item
    nlohmann::json report;
    report["taskId"] = items[0].at("taskId").GetS();
    report["participantId"] = items[0].at("participantId").GetS();

    // For each item, get the time and examples
    for (const auto &item : items) {
        // Make an object that has examples and time
        nlohmann::json queryObj;
        queryObj["positiveExamples"] = item.at("positiveExamples").GetSS();
        queryObj["negativeExamples"] = item.at("negativeExamples").GetSS();
        queryObj["time"] = item.at("time").GetS();

        report["queries"].push_back(std::move(queryObj));
    }

    // Done
    return report;
}

static inline std::string colonify(const std::string &key) {
    return ":" + key;
}

static std::string build_filter_string(const std::map<std::string, std::string> &query_params) {

    if (query_params.size() == 1) {
        std::stringstream expr;
        expr << query_params.begin()->first << "==" << colonify(query_params.begin()->first);
        return expr.str();
    }

    auto it = query_params.begin();
    auto secondToLast = query_params.end();
    --secondToLast;
    std::stringstream expr;
    for (; it != secondToLast; ++it) {
        expr << it->first << " = " << colonify(it->first) << " and ";
    }
    expr << it->first << " = " << colonify(it->first);

    return expr.str();
}

HttpResponse TrackerHandler::get(struct mg_http_message *http_msg) {
    LOG(LL_VERBOSE_DEBUG, ("Starting to handle event retrieval"));
    // Get the request data
    auto query_params = parse_query_params(std::string(http_msg->query.ptr, http_msg->query.len));

    // Actually perform the query
    Aws::DynamoDB::Model::ScanRequest query;
    query.SetTableName(TABLE_NAME.data());

    // Fill out of the expression replacement variables
    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> attrs;
    for (const auto &[key, value] : query_params) {
        Aws::DynamoDB::Model::AttributeValue awsValue;
        awsValue.SetS(value);
        attrs[colonify(key)] = awsValue;
    }
    query.SetExpressionAttributeValues(std::move(attrs));
    auto filter_expr = build_filter_string(query_params);
    query.SetFilterExpression(filter_expr);

    // Execute the query
    const Aws::DynamoDB::Model::ScanOutcome& result = this->dynamo_client.Scan(query);
    if (!result.IsSuccess()) {
        LOG(LL_ERROR, ("Error while GETTING request: %s", result.GetError().GetMessage().c_str()));
        nlohmann::json err_obj;
        err_obj["code"] = 500;
        err_obj["message"] = "Error while querying events from the server";
        err_obj["error"] = result.GetError().GetMessage();
        return HttpResponse(500, { {"Content-Type", "application/json"} }, err_obj.dump());
    }

    const Aws::Vector<Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>>& items = result.GetResult().GetItems();
    auto report = prepare_participant_report(items);
    return HttpResponse(200, {{"Content-Type", "application/json"}}, report.dump());
}
