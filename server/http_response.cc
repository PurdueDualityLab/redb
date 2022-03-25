//
// Created by charlie on 3/9/22.
//

#include <sstream>
#include "http_response.h"

void HttpResponse::enqueue(struct mg_connection *connection) {

    // stringify the headers
    std::stringstream headers_string;
    for (const auto &[header, value] : this->headers) {
        headers_string << header << ": " << value << "\r\n";
    }

    mg_http_reply(connection, this->status, headers_string.str().c_str(), this->body_payload.c_str());
}
