#include "../inc/matt_daemon.hpp"

int users;
Tintin_reporter logger;
std::vector<int> pids;

//https://github.com/torvalds/linux/blob/master/include/linux/signal.h

int default_action(int sig)
{
    if (sig != SIGCHLD && sig != SIGCONT && sig != SIGURG && sig != SIGWINCH)
        return (TERMINATE);
    return (IGNORE);
}

void signal_handler(int sig)
{
    int pid;
    int status = 0;
    std::string signame;

    signame = strsignal(sig);
    logger.info("Matt_daemon: Signal handler (" + signame + ").\n");
    if (default_action(sig) == TERMINATE)
    {
        for (unsigned long i = 0; i < pids.size(); i++)
            kill(pids[i], SIGKILL);
        logger.info("Matt_daemon: Quitting.\n");
        exit(EXIT_SUCCESS);
    }
    else
    {
        if (sig == SIGCHLD)
        {
            pid = waitpid(0, &status, WNOHANG);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 42)
            {
                printf("here\n");
                for (unsigned long i = 0; i < pids.size(); i++)
                {
                    if (pids[i] == pid)
                        pids.erase(pids.begin() + i);
                }
                users--;
            }
            printf("Users : %d\n", users);
        }
    }
}

void signals(void)
{
    static int signals[] = {
        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGIOT, SIGBUS, SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE,
        SIGALRM, SIGTERM, SIGCHLD, SIGCONT, SIGCONT, SIGTSTP, SIGTTIN, SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF,
        /*SIGPOLL,*/ SIGIO, SIGSYS, /*SIGUNUSED, SIGSTKFLT,*/ SIGWINCH, /*SIGPWR, SIGRTMIN-SIGRTMAX,*/ SIGEMT};

    for (int i = 0; signals[i]; i++)
    {
        if (signal(signals[i], signal_handler) == SIG_ERR)
            exit(EXIT_FAILURE);
    }
}