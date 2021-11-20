#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

template <typename T>
void errorSignalHandler(T sig) {
    std::cout << "Exiting due to segmentation fault..." << std::endl;
    void *array[10];
    int size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(sig);
}

#define installErrorHandler signal(SIGSEGV, errorSignalHandler<int>)


