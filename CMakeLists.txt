
cmake_minimum_required(VERSION 3.20.2)
project(redb
        VERSION 0.0.0
        DESCRIPTION "Tools for reusing regular expressions")

set(CMAKE_CXX_STANDARD 17)

# Add 3rd party dependencies
add_subdirectory(deps)

# Contains librereuse, which is the library with all of the reuse stuff in it
add_subdirectory(librereuse)

add_subdirectory(clustering_experiment)

# Build a microservice
add_subdirectory(web)

# add stuff for google test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(test)
