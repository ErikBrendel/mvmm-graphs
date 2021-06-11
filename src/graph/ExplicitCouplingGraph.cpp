#include "ExplicitCouplingGraph.h"
#include "../ProgressDisplay.h"
#include "../util.h"

#include <iostream>
#include <algorithm>

ExplicitCouplingGraph::ExplicitCouplingGraph(const string& name)
    : NodeSetCouplingGraph(name) {}

const vector<string>& ExplicitCouplingGraph::getNodeSet() {
    return i2node;
}

void ExplicitCouplingGraph::add(const string& a, const string& b, double delta) {
    if (a == b) return;
    addI(getNodeIndex(a), getNodeIndex(b), delta);
}

void ExplicitCouplingGraph::addI(uint a, uint b, double delta) {
    if (a == b) return;
    auto& edgeList = adj[a];
    auto existingConnection = edgeList.find(b);
    if (existingConnection == edgeList.end()) {
        edgeList[b] = delta;
    } else {
        existingConnection->second += delta;
    }
    auto& edgeList2 = adj[b];
    auto existingConnection2 = edgeList2.find(a);
    if (existingConnection2 == edgeList2.end()) {
        edgeList2[a] = delta;
    } else {
        existingConnection2->second += delta;
    }
}

double ExplicitCouplingGraph::get(const string& a, const string& b) {
    auto ai = node2i.find(a);
    if (ai == node2i.end()) return 0;
    auto bi = node2i.find(b);
    if (bi == node2i.end()) return 0;
    auto& edgeList = adj[ai->second];
    auto conn = edgeList.find(bi->second);
    if (conn == edgeList.end()) {
        return 0;
    } else {
        return conn->second;
    }
}

double ExplicitCouplingGraph::getDirectCoupling(const string& a, const string& b) {
    return get(a, b);
}

vector<string> ExplicitCouplingGraph::getDirectlyCoupled(const string& node) {
    auto i = node2i.find(node);
    if (i == node2i.end()) return {};
    auto edgeList = adj[i->second];
    vector<string> result;
    result.reserve(edgeList.size());
    for (const auto& entry: edgeList) {
        result.push_back(i2node[entry.first]);
    }
    return result;
}

void ExplicitCouplingGraph::addSupport(const string& node, double delta) {
    supports[getNodeIndex(node)] += delta;
}

double ExplicitCouplingGraph::getSupport(const string& node) {
    auto found = node2i.find(node);
    if (found == node2i.end()) return 0;
    return supports[found->second];
}

double ExplicitCouplingGraph::getAbsoluteSelfSupport(const string& node) {
    return getSupport(node);
}

void ExplicitCouplingGraph::addAndSupport(const string& a, const string& b, double delta) {
    add(a, b, delta);
    addSupport(a, delta);
    addSupport(b, delta);
}

void ExplicitCouplingGraph::removeNode(const string& node) {
    auto found = node2i.find(node);
    if (found == node2i.end()) {
        return;
    }
    auto nodeIndex = found->second;
    node2i.erase(found);

    // remove all connections from / to the node that shall be removed
    for (const auto& [n2, w]: adj[nodeIndex]) {
        adj[n2].erase(nodeIndex);
    }
    adj[nodeIndex].clear();

    // the last node will be swapped in to fill the gap
    auto lastNodeIndex = adj.size() - 1;

    //redirect all pointers to the last node to its new location
    for (const auto& [n2, w]: adj[lastNodeIndex]) {
        adj[n2].erase(lastNodeIndex);
        adj[n2][nodeIndex] = w;
    }

    // do the node swap within all containers
    supports[nodeIndex] = supports[lastNodeIndex];
    supports.pop_back();
    adj[nodeIndex] = adj[lastNodeIndex];
    adj.pop_back();
    i2node[nodeIndex] = i2node[lastNodeIndex];
    i2node.pop_back();
    node2i[i2node[nodeIndex]] = nodeIndex;
}

void ExplicitCouplingGraph::cutoffEdges(double minimumWeight) {
    for (unordered_map<uint, double>& edgeList : adj) {
        // https://stackoverflow.com/a/9515446/4354423 remove from map by condition
        for(auto it = edgeList.begin(), ite = edgeList.end(); it != ite;) {
            if(it->second < minimumWeight) {
                it = edgeList.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void ExplicitCouplingGraph::removeSmallComponents(int minimumComponentSize) {
    auto cc = connectedComponents();
    vector<string> nodesToRemove;
    for (const auto& component: cc) {
        if (component.size() < minimumComponentSize) {
            for (uint node: component) {
                nodesToRemove.push_back(i2node[node]);
            }
        }
    }
    for (const auto& node: nodesToRemove) {
        removeNode(node);
    }
}

void ExplicitCouplingGraph::propagateDown(int layers, double weightFactor) {
    unordered_map<uint, vector<uint>> childrenDict = getChildrenDict();
    vector<uint> childHavingNodes;
    for (const auto& entry: childrenDict) {
        childHavingNodes.push_back(entry.first);
    }
    sort(all(childHavingNodes), [&](uint a, uint b) {
        return count(all(i2node[a]), '/') > count(all(i2node[b]), '/');
    });

    rep(iteration, layers) {
        vector<tuple<uint, uint, double>> changesToApply;
        for (const uint& node: childHavingNodes) {
            vector<pair<uint, double>> connectionsAndWeights;
            for (const auto& [n2, w]: adj[node]) {
                if (!startsWith(i2node[n2], i2node[node] + "/")) {
                    connectionsAndWeights.emplace_back(n2, w * weightFactor);
                }
            }

            for (const uint& child: childrenDict[node]) {
                for (const auto& [conn, val]: connectionsAndWeights) {
                    auto foundConnChildren = childrenDict.find(conn);
                    if (foundConnChildren != childrenDict.end()) {
                        for (const auto& connChild: foundConnChildren->second) {
                            changesToApply.emplace_back(child, connChild, val);
                        }
                    }
                }
            }
        }
        for (const auto& [a, b, delta]: changesToApply) {
            addI(a, b, delta);
        }
    }
}

void ExplicitCouplingGraph::dilate(int iterations, double weightFactor) {

    rep(iteration, iterations) {
        vector<unordered_map<uint, double>> changesToApply;
        changesToApply.resize(adj.size());

        ProgressDisplay::init("Collecting dilation info", node2i.size());
        rep(node, node2i.size()) {
            ProgressDisplay::update();
            vector<pair<uint, double>> connectionsAndWeights;
            for (const auto& [n2, w]: adj[node]) {
                if (!startsWith(i2node[n2], i2node[node] + "/") && !startsWith(i2node[node], i2node[n2] + "/")) {
                    connectionsAndWeights.emplace_back(n2, w * weightFactor);
                }
            }
            if (connectionsAndWeights.empty()) continue;
            rep_all_pairs(i, j, connectionsAndWeights.size()) {
                const auto& [c1, w1] = connectionsAndWeights[i];
                const auto& [c2, w2] = connectionsAndWeights[j];
                double w = min(w1, w2);
                uint a = c1;
                uint b = c2;
                if (a > b) swap(a, b);
                auto existing = changesToApply[a].find(b);
                if (existing != changesToApply[a].end()) {
                    existing->second += w;
                } else {
                    changesToApply[a][b] = w;
                }
            }
        }
        ProgressDisplay::close();

        ProgressDisplay::init("Applying changes", adj.size());
        rep(n1, adj.size()) {
            ProgressDisplay::update();
            for (const auto& [n2, w]: changesToApply[n1]) {
                addI(n1, n2, w);
            }
        }
        ProgressDisplay::close();
    }
}

uint ExplicitCouplingGraph::getNodeIndex(const string& node) {
    auto found = node2i.find(node);
    if (found != node2i.end()) {
        return found->second;
    }
    uint result = node2i.size();
    supports.push_back(0);
    adj.emplace_back();
    i2node.push_back(node);
    node2i[node] = result;
    return result;
}

unordered_map<uint, vector<uint>> ExplicitCouplingGraph::getChildrenDict() {
    unordered_map<uint, vector<uint>> result;
    rep(node, node2i.size()) {
        string& nodeStr = i2node[node];
        if (nodeStr.find('/') != std::string::npos) {
            string parentStr = getParent(nodeStr);
            auto foundParent = node2i.find(parentStr);
            if (foundParent != node2i.end()) {
                result[foundParent->second].push_back(node);
            }
        }
    }
    return result;
}

void ExplicitCouplingGraph::plaintextSave(ostream& out) {
    out << "Explicit" << endl;
    // first line: all node strings
    // second line: support values for each node
    // third line: all edges, by node index

    rep(i, node2i.size()) {
        if (i != 0) out << ";";
        out << i2node[i];
    }
    out << endl;

    rep(i, node2i.size()) {
        if (i != 0) out << ";";
        out << supports[i];
    }
    out << endl;

    bool first = true;
    rep(n1, node2i.size()) {
        for (const auto& [n2, weight]: adj[n1]) {
            if (n2 < n1) continue;
            if (first) {
                first = false;
            } else {
                out << ";";
            }
            out << n1 << "," << n2 << "," << weight;
        }
    }
    out << endl;
}

void ExplicitCouplingGraph::plaintextLoad(istream& in) {
    string line;
    getline(in, line);
    i2node.clear();
    split(line, ';', i2node);
    auto n = i2node.size();

    rep(i, n) {
        node2i[i2node[i]] = i;
    }

    supports.resize(n);
    getline(in, line);
    auto supportStrings = split(line, ';');
    rep(i, n) {
        supports[i] = stof(supportStrings[i]);
    }

    adj.resize(n);
    getline(in, line);
    for (const auto& connectionString: split(line, ';')) {
        auto parts = split(connectionString, ',');
        ensure(parts.size() == 3, "connection needs to consist of 3 parts: a, b, weight!");
        int n1 = stoi(parts[0]);
        int n2 = stoi(parts[1]);
        double w = stof(parts[2]);
        adj[n1][n2] = w;
        adj[n2][n1] = w;
    }
}

void ExplicitCouplingGraph::printStatistics() {
    auto nodeCount = adj.size();
    auto edgeCount = accumulate(all(adj), 0, [](int acc, const unordered_map<uint, double>& edgeList){return acc + edgeList.size();}) / 2;
    auto cc = connectedComponents();
    sort(all(cc), [](const unordered_set<uint>& a, const unordered_set<uint>& b){return a.size() < b.size();});
    vector<int> ccSizes(cc.size());
    rep(i, cc.size()) {
        ccSizes[i] = cc[i].size();
    }
    cout << "ExplicitCouplingGraph statistics: " <<
         nodeCount << " nodes, " <<
         edgeCount << " edges, " <<
         cc.size() << " connected components, with sizes: ";
    printAbbrev(ccSizes);
    cout << endl;
    vector<double> edgeWeights;
    edgeWeights.reserve(edgeCount);
    rep(n1, adj.size()) {
        for (const auto& [n2, w]: adj[n1]) {
            if (n2 > n1) edgeWeights.push_back(w);
        }
    }
    sort(all(edgeWeights));
    vector<double> nodeSupports(supports);
    sort(all(nodeSupports));
    cout << "Edge weights: ";
    printAbbrev(edgeWeights);
    cout << ", mean: " << mean(edgeWeights) << endl;
    cout << "Node support values: ";
    printAbbrev(nodeSupports);
    cout << ", mean: " << mean(nodeSupports) << endl;
}

const unordered_set<string>& ExplicitCouplingGraph::getChildren(const string& node) {
    return NodeSetCouplingGraph::getChildren(node);
}

double ExplicitCouplingGraph::getNormalizedSupport(const string& node) {
    return NormalizeSupport::getNormalizedSupport(node);
}

double ExplicitCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    return NormalizeCouplingWithChildren::getNormalizedCoupling(a, b);
}

unordered_set<uint> ExplicitCouplingGraph::plainBfs(uint source) {
    unordered_set<uint> seen;
    unordered_set<uint> nextLevel;
    nextLevel.insert(source);
    while (!nextLevel.empty()) {
        auto thisLevel = nextLevel;
        nextLevel.clear();
        for (const auto& v: thisLevel) {
            if (seen.find(v) == seen.end()) {
                seen.insert(v);
                for (const auto& [n, w]: adj[v]) {
                    nextLevel.insert(n);
                }
            }
        }
    }
    return seen;
}

vector<unordered_set<uint>> ExplicitCouplingGraph::connectedComponents() {
    vector<unordered_set<uint>> result;
    vector<bool> seen(adj.size(), false);
    rep(v, adj.size()) {
        if (!seen[v]) {
            result.push_back(plainBfs(v));
            for (uint s: result.back()) {
                seen[s] = true;
            }
        }
    }
    return result;
}
