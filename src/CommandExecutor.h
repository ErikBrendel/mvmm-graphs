#pragma once

#include "DataStorage.h"

class CommandExecutor {
private:
    DataStorage dataStorage;

public:
    void tryExecuteCommand(const string& command);

private:
    void executeCommand(const string& command);

};


