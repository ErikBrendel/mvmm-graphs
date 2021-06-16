#include <iostream>

#include "util.h"
#include "CommandExecutor.h"

using namespace std;


void mainLoop() {
    CommandExecutor executor;
    string line;
    while (getline(cin, line)) {
        executor.tryExecuteCommand(line);
    }
}

int main() {
    cout.precision(10);
    cout << fixed;
    mainLoop();

    // BestResultSet<int> brs(4, 100);
    // brs.trim();

    return 0;
}
