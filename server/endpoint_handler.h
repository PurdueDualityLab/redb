//
// Created by charlie on 3/10/22.
//

#ifndef REDB_ENDPOINT_HANDLER_H
#define REDB_ENDPOINT_HANDLER_H

#include <mongoose.h>
#include "http_response.h"

class EndpointHandler {
public:
    virtual HttpResponse get(struct mg_http_message *http_msg) {
        return HttpResponse::invalid_endpoint_verb();
    }
    virtual HttpResponse post(struct mg_http_message *http_msg) {
        return HttpResponse::invalid_endpoint_verb();
    }
    virtual HttpResponse options(struct mg_http_message *http_msg) {
        return HttpResponse::invalid_endpoint_verb();
    }

    HttpResponse handle(const std::string &verb, struct mg_http_message *msg) {
        if (verb == "POST")
            return this->post(msg);
        else if (verb == "GET")
            return this->get(msg);
        else if (verb == "OPTIONS")
            return this->options(msg);
        else
            return HttpResponse::invalid_endpoint_verb();
    }
};


#endif //REDB_ENDPOINT_HANDLER_H
