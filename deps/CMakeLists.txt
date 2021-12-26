
# Defines 3rd party dependencies
set(RE2_BUILD_TESTING OFF)
add_subdirectory(re2)
add_subdirectory(json)

# Setup mongoose dependency
add_library(mongoose
        mongoose/mongoose.c mongoose/mongoose.h)
target_include_directories(mongoose PUBLIC mongoose)