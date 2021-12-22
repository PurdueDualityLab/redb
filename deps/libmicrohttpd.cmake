
include(ExternalProject)

set(LIBMICROHTTPD_LIBRARY ${CMAKE_BINARY_DIR}/)

set(LIBMICROHTTPD_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libmicrohttpd)

ExternalProject_Add(libmicrohttpd
        SOURCE_DIR ${LIBMICROHTTPD_PROJECT_DIR}
        CONFIGURE_COMMAND ${LIBMICROHTTPD_PROJECT_DIR}/configure --disable-docs --disable-examples --prefix=${CMAKE_BINARY_DIR}/bin
        CONFIGURE_HANDLED_BY_BUILD ON
        BUILD_COMMAND make -j6
        INSTALL_COMMAND make install
        LOG_CONFIGURE true
        LOG_BUILD true)

ExternalProject_Add_Step(libmicrohttpd
        bootstrap
        COMMAND autoreconf -fi
        DEPENDERS configure
        WORKING_DIRECTORY ${LIBMICROHTTPD_PROJECT_DIR}
        COMMENT "Building out configure scripts")

add_library(libmicrohttpd::libmicrohttpd INTERFACE IMPORTED GLOBAL)
target_include_directories(libmicrohttpd::libmicrohttpd INTERFACE ${CMAKE_BINARY_DIR}/bin/include/)
target_include_directories(libmicrohttpd::libmicrohttpd INTERFACE ${CMAKE_BINARY_DIR}/bin/lib/libmicrohttpd.a)
add_dependencies(libmicrohttpd::libmicrohttpd libmicrohttpd)