#pragma once

#include "GraphStorage.h"

class CommandExecutor {
private:
    GraphStorage graphStorage;

public:
    void tryExecuteCommand(const string& command);

private:
    void executeCommand(const string& command);

};


