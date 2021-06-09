#include "CommandExecutor.h"

#include <iostream>

using namespace std;


#define ensure(condition, message) if (!(condition)) throw runtime_error(message);

void printHelpFn();

static const vector<tuple<string, string, function<void(DataStorage& dataStorage, const vector<string>& args)>>> handlers = {
        {"help", "", [](DataStorage& dataStorage, const vector<string>& args) {
            printHelpFn();
        }},
        {"load", "repoName name", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.load(args[0], args[1]) << endl;
        }},
        {"save", "graphId repoName", [](DataStorage& dataStorage, const vector<string>& args) {
            dataStorage.save(stoi(args[0]), args[1]);
        }},
        {"plaintextSave", "graphId repoName", [](DataStorage& dataStorage, const vector<string>& args) {
            dataStorage.getG(args[0])->plaintextSave(args[1]);
        }},

        {"createExplicit", "name", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.createExplicit(args[0]) << endl;
        }},
        {"createSimilarity", "name", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.createSimilarity(args[0]) << endl;
        }},
        {"createModuleDistance", "", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.createModuleDistance() << endl;
        }},
        {"createCached", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.createCached(stoi(args[0])) << endl;
        }},
        {"createCombination", "graphIds...", [](DataStorage& dataStorage, const vector<string>& args) {
            ensure(!args.empty(), "Need to specify at least one graph!")
            vector<int> graphs;
            for (const auto& arg: args) {
                graphs.push_back(stoi(arg));
            }
            cout << dataStorage.createCombination(graphs) << endl;
        }},
        {"createCombinationWeights", "graphIds... weights...", [](DataStorage& dataStorage, const vector<string>& args) {
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
            cout << dataStorage.createCombination(graphs, weights) << endl;
        }},

        {"getNormalizedSupport", "graphId node", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.getG(args[0])->getNormalizedSupport(args[1]) << endl;
        }},
        {"getNormalizedCoupling", "graphId a b", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.getG(args[0])->getNormalizedCoupling(args[1], args[2]) << endl;
        }},
        {"getGraphNodeSet", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            auto nodes = dataStorage.getG(args[0])->getNodeSet();
            rep(i, nodes.size()) {
                if (i > 0) cout << " ";
                cout << nodes[i];
            }
            cout << endl;
        }},
        {"printStatistics", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            dataStorage.getG(args[0])->printStatistics();
        }},

        {"saveNodeSet", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.addNodeSet(make_shared<vector<string>>(dataStorage.getG(args[0])->getNodeSet())) << endl;
        }},
        {"createNodeSet", "nodes...", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.addNodeSet(make_shared<vector<string>>(args)) << endl;
        }},
        {"getNodeSet", "nodeSetId", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << dataStorage.getNodeSet(args[0]) << endl;
        }},

        {"howWellPredictsMissingNode", "graphId nodeSetId nodeMissingFromSet nodeSet...", [](DataStorage& dataStorage, const vector<string>& args) {
            auto& g = dataStorage.getG(args[0]);
            auto& ns = dataStorage.getNodeSet(args[1]);
            vector<string> nodeSet(args.begin() + 3, args.end());
            float result = g->howWellPredictsMissingNode(*ns, args[2], nodeSet);
            cout << result << endl;
        }},

        {"explicitAdd", "graphId a b delta", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->add(args[1], args[2], stof(args[3]));
        }},
        {"explicitAddSupport", "graphId node delta", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->addSupport(args[1], stof(args[2]));
        }},
        {"explicitAddAndSupport", "graphId a b delta", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->addAndSupport(args[1], args[2], stof(args[3]));
        }},
        {"explicitCutoffEdges", "graphId minimumWeight", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->cutoffEdges(stof(args[1]));
        }},
        {"explicitPropagateDown", "graphId layers weightFactor", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->propagateDown(stoi(args[1]), stof(args[2]));
        }},
        {"explicitDilate", "graphId iterations weightFactor", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->dilate(stoi(args[1]), stof(args[2]));
        }},
        {"similarityAddNode", "graphId node coords... support", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& similarityGraph = dynamic_pointer_cast<SimilarityCouplingGraph>(graph);
            ensure(similarityGraph != nullptr, "Can only be used on similarity graphs!")
            vector<float> coords;
            for (int i = 2; i < args.size() - 1; i++) {
                coords.push_back(stof(args[i]));
            }
            similarityGraph->addNode(args[1], coords, stof(args.back()));
        }},
        {"combinedSetWeights", "graphId weights...", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& combinedGraph = dynamic_pointer_cast<WeightCombinedGraph>(graph);
            ensure(combinedGraph != nullptr, "Can only be used on weight-combined graphs!")
            vector<float> weights;
            for (int i = 1; i < args.size(); i++) {
                weights.push_back(stof(args[i]));
            }
            combinedGraph->setWeights(weights);
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
            get<2>(h)(dataStorage, parts);
            return;
        }
    }
    cerr << "Unknown command: " << cmd << endl;
}
