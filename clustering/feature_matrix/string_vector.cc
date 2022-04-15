//
// Created by charlie on 4/4/22.
//

#include "string_vector.h"
#include "../ThreadPool.h"
#include "../program_options.h"
#include <egret.h>
#include <unistd.h>
#include <sys/wait.h>

std::vector<std::string> invoke_egret(const std::string &pattern) {
    int communications[2];
    int ret = pipe(communications);
    pid_t pid = fork();
    if (pid == 0) {
        // Close the read end
        close(communications[0]);

        nlohmann::json strings_obj;
        try {
            auto strings = run_engine(pattern, "clustering");
            strings_obj = strings;
        } catch (std::runtime_error &exe) {
            strings_obj = std::vector<std::string> {};
        }

        // Write the data over
        auto strings_obj_payload = strings_obj.dump();
        write(communications[1], strings_obj_payload.c_str(), strings_obj_payload.size());

        // Close up everything
        close(communications[1]);

        exit(0);
    } else {
        close(communications[1]);

        // Parent: read the strings back
        int child_status;
        waitpid(pid, &child_status, 0);

        if (WIFEXITED(child_status)) {
            // There was no problem. Read the strings and return them
            std::stringstream result_buffer;
            std::array<char, 256> block {0};
            ssize_t bytes_read = 0;
            while ((bytes_read = read(communications[0], block.data(), block.size())) > 0) {
                result_buffer << block.data();
                block.fill(0);
            }

            // Read into a json obj
            nlohmann::json strings_obj;
            result_buffer >> strings_obj;

            close(communications[0]);

            // Return obj as vector
            return strings_obj.get<std::vector<std::string>>();
        } else {
            // There was an error, so ignore
            close(communications[0]);

            // Return an empty vector
            return {};
        }
    }
}

StringVector::StringVector(const std::vector<std::string> &patterns) {
    // Try out using the global workers variable
    ThreadPool workers(ProgramOptions::instance().workers);

    std::vector<std::future<std::vector<std::string>>> tasks;
    for (const auto &pattern : patterns) {
        auto fut = workers.enqueue(invoke_egret, pattern);

        tasks.push_back(std::move(fut));
    }

    // Collect all the strings, make sure only duplicates are kept
    for (auto &task : tasks) {
        task.wait();
        auto result = task.get();
        auto all_strings_inserter = std::inserter(this->all_strings, this->all_strings.end());
        std::move(result.begin(), result.end(), all_strings_inserter);
    }
}

std::vector<std::string> StringVector::create_vector() const {
    // Assuming that set gives an ordered list
    std::vector<std::string> string_vector;
    std::copy(this->all_strings.cbegin(), this->all_strings.cend(), std::back_inserter(string_vector));
    return string_vector;
}
