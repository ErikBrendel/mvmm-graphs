#include "ExplicitCouplingGraph.h"
#include "../util.h"

#include <iostream>
#include <algorithm>

ExplicitCouplingGraph::ExplicitCouplingGraph(const string& name)
    : NodeSetCouplingGraph(name) {}

const vector<string>& ExplicitCouplingGraph::getNodeSet() {
    return i2node;
}

void ExplicitCouplingGraph::add(const string& a, const string& b, float delta) {
    if (a == b) return;
    addI(getNodeIndex(a), getNodeIndex(b), delta);
}

void ExplicitCouplingGraph::addI(uint a, uint b, float delta) {
    if (a == b) return;
    auto& edgeList = adj[a];
    auto existingConnection = find_if(all(edgeList), [=](const pair<uint, float>& e){return e.first == b;});
    if (existingConnection == edgeList.end()) {
        edgeList.emplace_back(b, delta);
    } else {
        existingConnection->second += delta;
    }
}

float ExplicitCouplingGraph::get(const string& a, const string& b) {
    auto ai = node2i.find(a);
    if (ai == node2i.end()) return 0;
    auto bi = node2i.find(b);
    if (bi == node2i.end()) return 0;
    auto& edgeList = adj[ai->second];
    auto conn = find_if(all(edgeList), [=](const pair<uint, float>& e){return e.first == bi->second;});
    if (conn == edgeList.end()) {
        return 0;
    } else {
        return conn->second;
    }
}

float ExplicitCouplingGraph::getDirectCoupling(const string& a, const string& b) {
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

void ExplicitCouplingGraph::addSupport(const string& node, float delta) {
    supports[getNodeIndex(node)] += delta;
}

float ExplicitCouplingGraph::getSupport(const string& node) {
    auto found = node2i.find(node);
    if (found == node2i.end()) return 0;
    return supports[found->second];
}

float ExplicitCouplingGraph::getAbsoluteSelfSupport(const string& node) {
    return getSupport(node);
}

void ExplicitCouplingGraph::addAndSupport(const string& a, const string& b, float delta) {
    add(a, b, delta);
    addSupport(a, delta);
    addSupport(b, delta);
}

void ExplicitCouplingGraph::cutoffEdges(float minimumWeight) {
    for (vector<pair<uint, float>>& edgeList : adj) {
        edgeList.erase(
                remove_if(all(edgeList), [=](const pair<uint, float>& conn) { return conn.second < minimumWeight; }),
                edgeList.end()
        );
    }
}

void ExplicitCouplingGraph::propagateDown(int layers, float weightFactor) {
    unordered_map<uint, vector<uint>> childrenDict = getChildrenDict();
    vector<uint> childHavingNodes;
    for (const auto& entry: childrenDict) {
        childHavingNodes.push_back(entry.first);
    }
    sort(all(childHavingNodes), [&](uint a, uint b) {
        return count(all(i2node[a]), '/') > count(all(i2node[b]), '/');
    }); // TODO is this the right order?

    rep(iteration, layers) {
        vector<tuple<uint, uint, float>> changesToApply;
        for (const uint& node: childHavingNodes) {
            vector<pair<uint, float>> connectionsAndWeights;
            for (const pair<uint, float>& conn: adj[node]) {
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

void ExplicitCouplingGraph::dilate(int iterations, float weightFactor) {

    rep(iteration, iterations) {
        vector<tuple<uint, uint, float>> changesToApply;

        rep(node, adj.size()) {
            vector<pair<uint, float>> connectionsAndWeights;
            for (const pair<uint, float>& conn: adj[node]) {
                if (!startsWith(i2node[conn.first], i2node[node] + "/") && !startsWith(i2node[node], i2node[conn.first] + "/")) {
                    connectionsAndWeights.emplace_back(conn.first, conn.second * weightFactor);
                }
            }
            rep_all_pairs(i, j, connectionsAndWeights.size()) {
                const auto& [c1, w1] = connectionsAndWeights[i];
                const auto& [c2, w2] = connectionsAndWeights[j];
                changesToApply.emplace_back(c1, c2, min(w1, w2));
            }
        }

        for (const auto& [a, b, delta]: changesToApply) {
            addI(a, b, delta);
        }
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

void ExplicitCouplingGraph::plaintextContent(ostream& out) {
    // first line: all node strings
    // second line: all edges, by node index
    rep(i, adj.size()) {
        if (i != 0) out << ";";
        out << i2node[i];
    }
    out << endl;
    bool first = true;
    rep(n1, adj.size()) {
        for (const auto& [n2, weight]: adj[n1]) {
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

void ExplicitCouplingGraph::printStatistics() {
    plaintextContent(cout);
}

vector<string> ExplicitCouplingGraph::getChildren(const string& node) {
    return NodeSetCouplingGraph::getChildren(node);
}

float ExplicitCouplingGraph::getNormalizedSupport(const string& node) {
    return NormalizeSupport::getNormalizedSupport(node);
}

float ExplicitCouplingGraph::getNormalizedCoupling(const string& a, const string& b) {
    return NormalizeCouplingWithChildren::getNormalizedCoupling(a, b);
}
