//
// Created by charlie on 3/9/22.
//

#include "egret_filtration.h"
#include <unistd.h>
#include <sys/wait.h>
#include <egret.h>
#include <stdexcept>

static int check_egret(const std::string &pattern) {
    try {
        auto strings = run_engine(pattern, "evil");
        return !strings.empty() ? 0 : 1;
    } catch (std::runtime_error &exe) {
        return 1; // exception found
    }
}

bool egret_compatible(const std::string &pattern) {

    pid_t checking_process = fork();
    if (checking_process == 0) {
        // we're in the child process, so we're good
        int ret_code = check_egret(pattern);
        exit(ret_code);
    }

    int status = 0;
    waitpid(checking_process, &status, 0);
    if (WIFSIGNALED(status)) {
        // waited_child was killed
        int stop_sig = WSTOPSIG(status);
        if (stop_sig == SIGSEGV) {
            // Process segfaulted, so false
            return false;
        } else if (stop_sig == SIGKILL)
            return false;
        else return true;
    } else if (WIFEXITED(status)) {
        // make sure we exited with 0
        int exit_status = WEXITSTATUS(status);
        return exit_status == 0;
    } else {
        return false;
    }
}
