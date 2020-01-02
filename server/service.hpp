#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
class Service
{
    int pid_fd_ = -1;
    std::string pid_file_name_;
    bool bConsoleMode_ = false;
public:
    Service() = default;
    int start()
    {
        if (!bConsoleMode_)
            forkService();  
            
        return run();
    }
protected:
    virtual int run() = 0;
    void setConsoleMode(bool bConsoleMode)
    {
        bConsoleMode_ = bConsoleMode;
    }

private:
//    void handle_signal(int sig)
//    {
//        if (sig == SIGINT) 
//        {
//            fprintf(log_stream, "Debug: stopping daemon ...\n");
//            /* Unlock and close lockfile */
//            if (pid_fd_ != -1) 
//            {
//                lockf(pid_fd_, F_ULOCK, 0);
//                close(pid_fd_);
//            }
//            /* Try to delete lockfile */
//            if (!pid_file_name_.empty())
//                unlink(pid_file_name_);
//
//            running = 0;
//            /* Reset signal handling to default behavior */
//            signal(SIGINT, SIG_DFL);
//        }
//        else if (sig == SIGHUP) 
//        {
//            fprintf(log_stream, "Debug: reloading daemon config file ...\n");
//            read_conf_file(1);
//        }
//        else if (sig == SIGCHLD) 
//        {
//            fprintf(log_stream, "Debug: received SIGCHLD signal\n");
//        }
//    }
    void forkService()
    {
        pid_t pid = fork();
        if (pid < 0)
            exit(EXIT_FAILURE);

        /* Success: Let the parent terminate */
        if (pid > 0)
            exit(EXIT_SUCCESS);

        /* On success: The child process becomes session leader */
        if (setsid() < 0)
            exit(EXIT_FAILURE);

        /* Ignore signal sent from child to parent process */
        signal(SIGCHLD, SIG_IGN);

        /* Fork off for the second time*/
        pid = fork();

        if (pid < 0)
            exit(EXIT_FAILURE);

        if (pid > 0)
            exit(EXIT_SUCCESS);

        umask(0);
        chdir("/");

        /* Close all open file descriptors */
        for (int fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--)
            close(fd);

        /* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
        stdin = fopen("/dev/null", "r");
        stdout = fopen("/dev/null", "w+");
        stderr = fopen("/dev/null", "w+");

        /* Try to write PID of daemon to lockfile */
        if (!pid_file_name_.empty())
        {
            char str[256];
            pid_fd_ = open(pid_file_name_.c_str(), O_RDWR | O_CREAT, 0640);
            if (pid_fd_ < 0) {
                /* Can't open lockfile */
                exit(EXIT_FAILURE);
            }
            if (lockf(pid_fd_, F_TLOCK, 0) < 0) {
                /* Can't lock file */
                exit(EXIT_FAILURE);
            }
            /* Get current PID */
            sprintf(str, "%d\n", getpid());
            /* Write PID to lockfile */
            write(pid_fd_, str, strlen(str));
        }
    }
};