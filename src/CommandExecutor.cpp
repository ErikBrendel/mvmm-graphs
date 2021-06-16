#include "CommandExecutor.h"

#include <iostream>

#include "disagreementAnalysis.h"

using namespace std;


void printHelpFn();

const string RESULT = "#result ";

const vector<tuple<string, string, function<void(DataStorage& dataStorage, const vector<string>& args)>>> handlers = {
        {"help", "", [](DataStorage& dataStorage, const vector<string>& args) {
            printHelpFn();
        }},
        {"load", "repoName name storageDir", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.load(args[0], args[1], args[2]);
            cout << RESULT << result << endl;
        }},
        {"save", "graphId repoName storageDir", [](DataStorage& dataStorage, const vector<string>& args) {
            dataStorage.save(stoi(args[0]), args[1], args[2]);
        }},
        {"getSaveLocation", "repoName name storageDir", [](DataStorage& dataStorage, const vector<string>& args) {
            string result = CouplingGraph::saveLocation(args[0], args[1], args[2]);
            cout << RESULT << result << endl;
        }},

        {"createExplicit", "name", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.createExplicit(args[0]);
            cout << RESULT << result << endl;
        }},
        {"createSimilarity", "name", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.createSimilarity(args[0]);
            cout << RESULT << result << endl;
        }},
        {"createModuleDistance", "", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.createModuleDistance();
            cout << RESULT << result << endl;
        }},
        {"createCached", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.createCached(stoi(args[0]));
            cout << RESULT << result << endl;
        }},
        {"createCombination", "graphIds...", [](DataStorage& dataStorage, const vector<string>& args) {
            ensure(!args.empty(), "Need to specify at least one graph!")
            vector<int> graphs;
            for (const auto& arg: args) {
                graphs.push_back(stoi(arg));
            }
            int result = dataStorage.createCombination(graphs);
            cout << RESULT << result << endl;
        }},
        {"createCombinationWeights", "graphIds... weights...", [](DataStorage& dataStorage, const vector<string>& args) {
            ensure(!args.empty(), "Need to specify at least one graph!")
            ensure(args.size() % 2 == 0, "Unequal amount of parameters for combination graph!")
            int graphAmount = (int)args.size() / 2;
            vector<int> graphs;
            vector<double> weights;
            rep(i, graphAmount) {
                graphs.push_back(stoi(args[i]));
            }
            for (int i = graphAmount; i < args.size(); ++i) {
                weights.push_back(stof(args[i]));
            }
            int result = dataStorage.createCombination(graphs, weights);
            cout << RESULT << result << endl;
        }},

        {"getGraphName", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& result = dataStorage.getG(args[0])->getName();
            cout << RESULT << result << endl;
        }},
        {"getNormalizedSupport", "graphId node", [](DataStorage& dataStorage, const vector<string>& args) {
            double result = dataStorage.getG(args[0])->getNormalizedSupport(args[1]);
            cout << RESULT << result << endl;
        }},
        {"getNormalizedCoupling", "graphId a b", [](DataStorage& dataStorage, const vector<string>& args) {
            double result = dataStorage.getG(args[0])->getNormalizedCoupling(args[1], args[2]);
            cout << RESULT << result << endl;
        }},
        {"getGraphNodeSet", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& nodes = dataStorage.getG(args[0])->getNodeSet();
            cout << RESULT;
            rep(i, nodes.size()) {
                if (i > 0) cout << "|";
                cout << nodes[i];
            }
            cout << endl;
        }},
        {"printStatistics", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            dataStorage.getG(args[0])->printStatistics();
        }},

        {"saveNodeSet", "graphId", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.addNodeSet(make_shared<vector<string>>(dataStorage.getG(args[0])->getNodeSet()));
            cout << RESULT << result << endl;
        }},
        {"createNodeSet", "nodes...", [](DataStorage& dataStorage, const vector<string>& args) {
            int result = dataStorage.addNodeSet(make_shared<vector<string>>(args));
            cout << RESULT << result << endl;
        }},
        {"getNodeSet", "nodeSetId", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& nodes = *dataStorage.getNodeSet(args[0]);
            cout << RESULT;
            rep(i, nodes.size()) {
                if (i != 0) cout << "|";
                cout << nodes[i];
            }
            cout << endl;
        }},

        {"howWellPredictsMissingNode", "graphId nodeSetId nodeMissingFromSet nodeSet...", [](DataStorage& dataStorage, const vector<string>& args) {
            auto& g = dataStorage.getG(args[0]);
            auto& ns = dataStorage.getNodeSet(args[1]);
            vector<string> nodeSet(args.begin() + 3, args.end());
            double result = g->howWellPredictsMissingNode(*ns, args[2], nodeSet);
            cout << RESULT << result << endl;
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
        {"explicitRemoveSmallComponents", "graphId minimumComponentSize", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& explicitGraph = dynamic_pointer_cast<ExplicitCouplingGraph>(graph);
            ensure(explicitGraph != nullptr, "Can only be used on explicit graphs!")
            explicitGraph->removeSmallComponents(stoi(args[1]));
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
            vector<double> coords;
            for (int i = 2; i < args.size() - 1; i++) {
                coords.push_back(stof(args[i]));
            }
            similarityGraph->addNode(args[1], coords, stof(args.back()));
        }},
        {"combinedSetWeights", "graphId weights...", [](DataStorage& dataStorage, const vector<string>& args) {
            const auto& graph = dataStorage.getG(args[0]);
            const auto& combinedGraph = dynamic_pointer_cast<WeightCombinedGraph>(graph);
            ensure(combinedGraph != nullptr, "Can only be used on weight-combined graphs!")
            vector<double> weights;
            for (int i = 1; i < args.size(); i++) {
                weights.push_back(stof(args[i]));
            }
            combinedGraph->setWeights(weights);
        }},
        
        /*{"createBestResultSet", "dimensions resultKeepSize", [](DataStorage& dataStorage, const vector<string>& args) {
            auto result = dataStorage.createBestResultSet(stoi(args[0]), stoi(args[1]));
            cout << RESULT << result << endl;
        }},
        {"getFromBestResultSet", "id weights...", [](DataStorage& dataStorage, const vector<string>& args) {
            auto brs = dataStorage.getBestResultSet(stoi(args[0]));
            ensure(args.size() == brs->getDimensions() + 1, "Weight data needs to match previously configured amount of dimensions!")
            vector<double> weights;
            for (int i = 1; i < args.size(); ++i) {
                weights.push_back(stod(args[i]));
            }
            auto result = brs->getBest(weights);
            cout << RESULT << result << endl;
        }},
        {"trimBestResultSet", "id", [](DataStorage& dataStorage, const vector<string>& args) {
            dataStorage.getBestResultSet(stoi(args[0]))->trim();
        }},*/
        {"findDisagreements", "nodeSetId resultSize graphAmount graphs... patternsComponents...", [](DataStorage& dataStorage, const vector<string>& args) {
            cout << "Starting disagreement analysis!" << endl;
            const auto& ns = *dataStorage.getNodeSet(args[0]);
            int resultSize = stoi(args[1]);
            int graphAmount = stoi(args[2]);
            ensure(graphAmount >= 2, "At least two graphs are required!")
            ensure((args.size() - 3) % graphAmount == 0, "Argument amount does not match!")
            vector<shared_ptr<CouplingGraph>> graphs;
            rep(i, graphAmount) {
                graphs.push_back(dataStorage.getG(args[3 + i]));
            }
            cout << "Got the graphs!" << endl;
            vector<vector<double>> patterns;
            for (int p = 3 + graphAmount; p < args.size(); p += graphAmount) {
                patterns.emplace_back();
                rep(i, graphAmount) {
                    patterns.back().push_back(stod(args[p + i]));
                }
            }
            cout << "Got the patterns!" << endl;
            auto results = analyzeDisagreements(ns, graphs, patterns, resultSize);
            cout << "Got the result!" << endl;

            cout << RESULT;
            bool first = true;
            rep(r, results.size()) {
                const auto& result = results[r];
                if (r != 0) cout << "|"; // which will lead to an empty string, separating the BRSs
                for (const auto& [coords, userData]: result->getAllData()) {
                    for (double v: coords) {
                        if (first) first = false; else cout << "|";
                        cout << v;
                    }
                    cout << "|" << get<0>(userData);
                    cout << "|" << get<1>(userData);
                    for (double v: get<2>(userData)) {
                        cout << "|" << v;
                    }
                }
            }
            cout << endl;
        }},
};

void printHelpFn() {
    cout << "Available commands: " << endl;
    for (const auto& command: handlers) {
        cout << " - " << get<0>(command) << " " << get<1>(command) << endl;
    }
    cout.flush();
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
        cerr.flush();
    }
}

void CommandExecutor::executeCommand(const string& command) {
    if (command.empty()) return;
    auto parts = split(command, '|');
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
            cout.flush();
            cerr.flush();
            return;
        }
    }
    cerr << "Unknown command: " << cmd << endl;
    cerr.flush();
}
