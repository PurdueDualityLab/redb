
cmake_minimum_required(VERSION 3.16.3)
project(redb
        VERSION 0.0.0
        DESCRIPTION "Tools for reusing regular expressions")

set(CMAKE_CXX_STANDARD 17)

# options to turn off some build stuff
option(SKIP_EXPERIMENTS "Don't build any experiments" FALSE)
option(SKIP_TESTS "Don't build any tests" FALSE)
option(AWS_EXTERN_PROJ "AWS Sdk external project. Disable this if the aws sdk is already installed globally" FALSE)

# Add 3rd party dependencies
if(SKIP_TESTS)
    # if we skip all tests, then we also want to skip re2 tests
    set(RE2_BUILD_TESTING OFF)
endif()
add_subdirectory(deps)

# Contains librereuse, which is the library with all of the reuse stuff in it
add_subdirectory(librereuse)

if (NOT SKIP_EXPERIMENTS)
    add_subdirectory(clustering_experiment)
endif()

# Run code to replicate chapman and stolee semantic clustering
add_subdirectory(clustering)

# Add server subdirectory
add_subdirectory(server)

# add stuff for google test
if(NOT SKIP_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(test)
endif()
