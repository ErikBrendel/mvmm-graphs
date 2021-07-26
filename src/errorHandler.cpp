#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

template <typename T>
void errorSignalHandler(T sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

#define installErrorHandler signal(SIGSEGV, errorSignalHandler<int>)


