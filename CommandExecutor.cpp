#include "CommandExecutor.h"

#include <iostream>

using namespace std;


#define ensure(condition, message) if (!(condition)) throw runtime_error(message);

void printHelpFn();

vector<tuple<string, string, function<void(GraphStorage& graphStorage, const vector<string>& args)>>> handlers = {
        {"help", "", [](GraphStorage& graphStorage, const vector<string>& args) {
            printHelpFn();
        }},
        {"load", "repoName name", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.load(args[0], args[1]) << endl;
        }},
        {"save", "id repoName", [](GraphStorage& graphStorage, const vector<string>& args) {
            graphStorage.save(stoi(args[0]), args[1]);
        }},
        {"createExplicit", "name", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.createExplicit(args[0]) << endl;
        }},
        {"createSimilarity", "name", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.createSimilarity(args[0]) << endl;
        }},
        {"createModuleDistance", "", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.createModuleDistance() << endl;
        }},
        {"createCached", "id", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.createCached(stoi(args[0])) << endl;
        }},
        {"createCombination", "ids...", [](GraphStorage& graphStorage, const vector<string>& args) {
            ensure(!args.empty(), "Need to specify at least one graph!")
            vector<int> graphs;
            for (const auto& arg: args) {
                graphs.push_back(stoi(arg));
            }
            cout << graphStorage.createCombination(graphs) << endl;
        }},
        {"createCombinationWeights", "ids... weights...", [](GraphStorage& graphStorage, const vector<string>& args) {
            ensure(!args.empty(), "Need to specify at least one graph!")
            ensure(args.size() % 2 == 0, "Unequal amount of parameters for combination graph!")
            int graphAmount = (int)args.size() / 2;
            vector<int> graphs;
            vector<float> weights;
            rep(i, graphAmount) {
                graphs.push_back(stoi(args[i]));
            }
            for (int i = graphAmount; i < args.size(); ++i) {
                weights.push_back(stof(args[i]));
            }
            cout << graphStorage.createCombination(graphs, weights) << endl;
        }},
        {"getNormalizedSupport", "id node", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.get(args[0])->getNormalizedSupport(args[1]) << endl;
        }},
        {"getNormalizedCoupling", "id a b", [](GraphStorage& graphStorage, const vector<string>& args) {
            cout << graphStorage.get(args[0])->getNormalizedCoupling(args[1], args[2]) << endl;
        }},
        {"getNodeSet", "id", [](GraphStorage& graphStorage, const vector<string>& args) {
            auto nodes = graphStorage.get(args[0])->getNodeSet();
            rep(i, nodes.size()) {
                if (i > 0) cout << " ";
                cout << nodes[i];
            }
            cout << endl;
        }},
};

void printHelpFn() {
    cout << "Available commands: " << endl;
    for (const auto& command: handlers) {
        cout << " - " << get<0>(command) << " " << get<1>(command) << endl;
    }
}


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
    string cmd = parts[0];
    parts.erase(parts.begin());
    int argumentCount = (int)parts.size();

    for (const auto& h: handlers) {
        if (cmd == get<0>(h)) {
            string argumentPattern = get<1>(h);
            int minArgs = 0;
            bool exact = true;
            for (const auto& word: split(argumentPattern, " ")) {
                if (word.size() > 3 && word.compare(word.size() - 3, 3, "...") == 0) {
                    exact = false;
                } else {
                    minArgs++;
                }
            }
            if (argumentCount < minArgs || (exact && argumentCount > minArgs)) {
                cerr << "Expected " << (exact ? "" : "at least ") << to_string(minArgs)
                     << " arguments for command " << cmd
                     << ", but got " << to_string(argumentCount)
                     << "!" << endl;
                return;
            }
            get<2>(h)(graphStorage, parts);
            return;
        }
    }
    cerr << "Unknown command: " << cmd << endl;
}
