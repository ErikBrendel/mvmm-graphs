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
    auto existingConnection = find_if(all(edgeList), [=](const pair<uint, double>& e){return e.first == b;});
    if (existingConnection == edgeList.end()) {
        edgeList.emplace_back(b, delta);
    } else {
        existingConnection->second += delta;
    }
    auto& edgeList2 = adj[b];
    auto existingConnection2 = find_if(all(edgeList2), [=](const pair<uint, double>& e){return e.first == a;});
    if (existingConnection2 == edgeList2.end()) {
        edgeList2.emplace_back(a, delta);
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
    auto conn = find_if(all(edgeList), [=](const pair<uint, double>& e){return e.first == bi->second;});
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
    vector<string> result(edgeList.size());
    rep(n, edgeList.size()) {
        result[n] = i2node[edgeList[n].first];
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

void ExplicitCouplingGraph::cutoffEdges(double minimumWeight) {
    for (vector<pair<uint, double>>& edgeList : adj) {
        edgeList.erase(
                remove_if(all(edgeList), [=](const pair<uint, double>& conn) { return conn.second < minimumWeight; }),
                edgeList.end()
        );
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
    }); // TODO is this the right order?

    rep(iteration, layers) {
        vector<tuple<uint, uint, double>> changesToApply;
        for (const uint& node: childHavingNodes) {
            vector<pair<uint, double>> connectionsAndWeights;
            for (const pair<uint, double>& conn: adj[node]) {
                if (!startsWith(i2node[conn.first], i2node[node] + "/")) {
                    connectionsAndWeights.emplace_back(conn.first, conn.second * weightFactor);
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
        vector<tuple<uint, uint, double>> changesToApply;

        ProgressDisplay::init("Collecting dilation info", adj.size());
        rep(node, adj.size()) {
            ProgressDisplay::update();
            vector<pair<uint, double>> connectionsAndWeights;
            for (const pair<uint, double>& conn: adj[node]) {
                if (!startsWith(i2node[conn.first], i2node[node] + "/") && !startsWith(i2node[node], i2node[conn.first] + "/")) {
                    connectionsAndWeights.emplace_back(conn.first, conn.second * weightFactor);
                }
            }
            if (connectionsAndWeights.empty()) continue;
            rep_all_pairs(i, j, connectionsAndWeights.size()) {
                const auto& [c1, w1] = connectionsAndWeights[i];
                const auto& [c2, w2] = connectionsAndWeights[j];
                changesToApply.emplace_back(c1, c2, min(w1, w2));
            }
        }
        ProgressDisplay::close();

        ProgressDisplay::init("Applying changes", changesToApply.size());
        for (const auto& [a, b, delta]: changesToApply) {
            ProgressDisplay::update();
            addI(a, b, delta);
        }
        ProgressDisplay::close();
    }
}

uint ExplicitCouplingGraph::getNodeIndex(const string& node) {
    auto found = node2i.find(node);
    if (found != node2i.end()) {
        return found->second;
    }
    uint result = adj.size();
    supports.push_back(0);
    adj.emplace_back();
    i2node.push_back(node);
    node2i[node] = result;
    return result;
}

unordered_map<uint, vector<uint>> ExplicitCouplingGraph::getChildrenDict() {
    unordered_map<uint, vector<uint>> result;
    rep(node, adj.size()) {
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

    rep(i, adj.size()) {
        if (i != 0) out << ";";
        out << i2node[i];
    }
    out << endl;

    rep(i, adj.size()) {
        if (i != 0) out << ";";
        out << supports[i];
    }
    out << endl;

    bool first = true;
    rep(n1, adj.size()) {
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
        adj[n1].emplace_back(n2, w);
        adj[n2].emplace_back(n1, w);
    }
}

void ExplicitCouplingGraph::printStatistics() {
    plaintextSave(cout);
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
