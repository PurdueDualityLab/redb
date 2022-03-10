
add_executable(redb-server
        main.cc
        args_parser.cc args_parser.h
        query_handler.cc query_handler.h
        http_response.cc http_response.h
        endpoint_handler.cc endpoint_handler.h)

target_link_libraries(redb-server PRIVATE mongoose librereuse nlohmann_json::nlohmann_json re2::re2)