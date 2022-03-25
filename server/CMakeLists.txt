
message(STATUS "Current search path: ${CMAKE_PREFIX_PATH}")
find_package(AWSSDK REQUIRED COMPONENTS dynamodb)

add_executable(redb-server
        main.cc
        args_parser.cc args_parser.h
        query_handler.cc query_handler.h
        http_response.cc http_response.h
        endpoint_handler.cc endpoint_handler.h tracker_handler.cc tracker_handler.h)
target_include_directories(redb-server PRIVATE ${AWSSDK_INCLUDE_DIRS})
target_link_libraries(redb-server PRIVATE mongoose librereuse nlohmann_json::nlohmann_json re2::re2 ${AWSSDK_LIBRARIES})