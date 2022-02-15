#include <iostream>

#include "util.h"
#include "CommandExecutor.h"
#include "errorHandler.cpp"

using namespace std;


void mainLoop() {
    CommandExecutor executor;
    string line;
    while (getline(cin, line)) {
        executor.tryExecuteCommand(line);
    }
}

int main() {
    installErrorHandler;
    cout.precision(10);
    cout << fixed;
    mainLoop();

    return 0;
}
