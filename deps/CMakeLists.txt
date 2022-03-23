
# Defines 3rd party dependencies
set(RE2_BUILD_TESTING OFF)
add_subdirectory(re2)
add_subdirectory(json)

# Setup mongoose dependency
add_library(mongoose
        mongoose/mongoose.c mongoose/mongoose.h)
target_include_directories(mongoose PUBLIC mongoose)

# Setup egret dependency
add_library(egret
        egret/src/Backref.cpp egret/src/Backref.h
        egret/src/CharSet.cpp egret/src/CharSet.h
        egret/src/Checker.cpp egret/src/Checker.h
        egret/src/Edge.cpp egret/src/Edge.h
        egret/src/ParseTree.cpp egret/src/ParseTree.h
        egret/src/Path.cpp egret/src/Path.h
        egret/src/RegexLoop.cpp egret/src/RegexLoop.h
        egret/src/RegexString.cpp egret/src/RegexString.h
        egret/src/Scanner.cpp egret/src/Scanner.h
        egret/src/Stats.cpp egret/src/Stats.h
        egret/src/TestGenerator.cpp egret/src/TestGenerator.h
        egret/src/Util.cpp egret/src/Util.h
        egret/src/NFA.cpp egret/src/NFA.h
        egret/src/egret.cpp egret/src/egret.h)
target_include_directories(egret PUBLIC egret/src)

# Add aws sdk support
if(AWS_EXTERN_PROJ)
include(ExternalProject)
set(AWSSDK_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-install)
ExternalProject_Add(awssdk
        # Configure
        PREFIX ${AWSSDK_INSTALL_DIR}
        LOG_DOWNLOAD TRUE
        # Checkout
        GIT_REPOSITORY https://github.com/aws/aws-sdk-cpp.git
        GIT_TAG 1.9.219
        GIT_SUBMODULES_RECURSE TRUE
        GIT_SHALLOW TRUE
        # Configure
        CMAKE_ARGS -DBUILD_ONLY=dynamodb -DCMAKE_INSTALL_PREFIX=${AWSSDK_INSTALL_DIR} -DBUILD_SHARED_LIBS=OFF -DENABLE_TESTING=OFF
        # Build
        BUILD_COMMAND ninja -j8
        # Install
        INSTALL_COMMAND ninja install)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${AWSSDK_INSTALL_DIR}/lib/cmake/ ${AWSSDK_INSTALL_DIR}/lib/aws-crt-cpp/cmake PARENT_SCOPE)
endif()