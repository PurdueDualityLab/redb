//
// Created by charlie on 3/22/22.
//

#ifndef REDB_TRACKER_HANDLER_H
#define REDB_TRACKER_HANDLER_H

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include "endpoint_handler.h"

class TrackerHandler : public EndpointHandler {
public:
    explicit TrackerHandler(const Aws::Auth::AWSCredentials& credentials);
    HttpResponse get(struct mg_http_message *http_msg) override;
    HttpResponse post(struct mg_http_message *http_message) override;

private:
    Aws::Client::ClientConfiguration client_config;
    Aws::DynamoDB::DynamoDBClient dynamo_client;
    static constexpr std::string_view TABLE_NAME = "regextools-participant-tracking";
};

#endif //REDB_TRACKER_HANDLER_H
