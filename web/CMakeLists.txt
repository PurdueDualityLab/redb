
add_executable(redb-web
        main.cc server.cc server.h)
target_link_libraries(redb-web PRIVATE librereuse libmicrohttpd::libmicrohttpd)