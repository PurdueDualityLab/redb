//
// Created by charlie on 12/21/21.
//

#ifndef _SERVER_H
#define _SERVER_H

#include <microhttpd.h>

namespace redb::web {
    class Server {
        explicit Server(int port);
        ~Server();

    private:
        MHD_Daemon *daemon;
    };
}

#endif //_SERVER_H
