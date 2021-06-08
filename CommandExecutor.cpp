#include "CommandExecutor.h"

#include <iostream>

using namespace std;

void CommandExecutor::tryExecuteCommand(const string& command) {
    try {
        executeCommand(command);
    } catch (exception& e) {
        string commandDisplay = command;
        if (commandDisplay.size() > 100) {
            commandDisplay = commandDisplay.substr(0, 97) + "...";
        }
        cerr << "Failed to execute command " << commandDisplay << "!" << endl;
        cerr << "Error: " << e.what() << endl;
    }
}

void CommandExecutor::executeCommand(const string& command) {
    if (command.empty()) return;
    auto parts = split(command, " ");
    if (parts.empty()) return;
    auto cmd = parts[0];
    int argumentCount = (int)parts.size() - 1;

#define ensure(condition, message) if (!(condition)) throw runtime_error(message);

    vector<tuple<string, int, function<void()>>> handlers = {
            {"load", 2, [&](){
                cout << graphStorage.load(parts[1], parts[2]) << endl;
            }},
            {"save", 2, [&](){
                graphStorage.save(stoi(parts[1]), parts[2]);
            }},
            {"createExplicit", 1, [&](){
                cout << graphStorage.createExplicit(parts[1]) << endl;
            }},
            {"createSimilarity", 1, [&](){
                cout << graphStorage.createSimilarity(parts[1]) << endl;
            }},
            {"createModuleDistance", 0, [&](){
                cout << graphStorage.createModuleDistance() << endl;
            }},
            {"createCached", 1, [&](){
                cout << graphStorage.createCached(stoi(parts[1])) << endl;
            }},
            {"createCombination", -1, [&]() {
                ensure(argumentCount > 0, "Need to specify at least one graph!")
                vector<int> graphs;
                rep(i, argumentCount) {
                    graphs.push_back(stoi(parts[i + 1]));
                }
                cout << graphStorage.createCombination(graphs) << endl;
            }},
            {"createCombinationWeights", -1, [&](){
                ensure(argumentCount % 2 == 0, "Unequal amount of parameters for combination graph!")
                int graphAmount = argumentCount / 2;
                ensure(graphAmount > 0, "Need to specify at least one graph!")
                vector<int> graphs;
                vector<float> weights;
                rep(i, graphAmount) {
                    graphs.push_back(stoi(parts[i + 1]));
                }
                for (int i = graphAmount; i < argumentCount; ++i) {
                    weights.push_back(stof(parts[i + 1]));
                }
                cout << graphStorage.createCombination(graphs, weights) << endl;
            }},
    };

    for (const auto& h: handlers) {
        if (cmd == get<0>(h)) {
            int expectedArguments = get<1>(h);
            if (expectedArguments >= 0 && expectedArguments != argumentCount) {
                cerr << "Expected " << to_string(expectedArguments)
                     << " arguments for command " << cmd
                     << ", but got" << to_string(argumentCount)
                     << "!" << endl;
                return;
            }
            get<2>(h)();
            return;
        }
    }
    cerr << "Unknown command: " << cmd << endl;
}
