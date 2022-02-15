
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
        egret/src/egret.cpp egret/src/egret.h)
target_include_directories(egret PUBLIC egret/src)
