
add_executable(cluster_eval
        eval_main.cc
        program_args.cc program_args.h)
target_link_libraries(cluster_eval PRIVATE librereuse nlohmann_json::nlohmann_json re2::re2 egret)