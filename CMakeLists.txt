
cmake_minimum_required(VERSION 3.20.2)
project(redb
        VERSION 0.0.0
        DESCRIPTION "Tools for reusing regular expressions")

set(CMAKE_CXX_STANDARD 17)

# options to turn off some build stuff
option(SKIP_EXPERIMENTS "Don't build any experiments" FALSE)
option(SKIP_TESTS "Don't build any tests" FALSE)

# Add 3rd party dependencies
add_subdirectory(deps)

# Contains librereuse, which is the library with all of the reuse stuff in it
add_subdirectory(librereuse)

if (NOT SKIP_EXPERIMENTS)
    add_subdirectory(clustering_experiment)
endif()

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
