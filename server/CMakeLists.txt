
add_executable(redb-server
        main.cc
        args_parser.cc args_parser.h)

target_link_libraries(redb-server PRIVATE mongoose librereuse nlohmann_json::nlohmann_json re2::re2)