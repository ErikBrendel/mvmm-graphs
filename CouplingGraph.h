#pragma once

#include <string>
#include <vector>
#include <memory>

using namespace std;

class CouplingGraph {
private:
    string name;

public:
    explicit CouplingGraph(string name);

    [[nodiscard]] const string& getName() const;

    virtual const vector<string>& getNodeSet();
    virtual float getNormalizedSupport(const string& node) = 0;
    virtual float getNormalizedCoupling(const string& a, const string& b) = 0;

    void save(const string& repoName);
    static shared_ptr<CouplingGraph> load(const string& repoName, const string& name);
    static string picklePath(const string& repoName, const string& name);

    void plaintextSave(const string& repoName);

    virtual string plaintextContent() = 0;
    virtual void printStatistics() = 0;

    float howWellPredictsMissingNode(const vector<string>& nodeSet, const string& nodeMissingFromSet, const vector<string>& allNodes);

public:
    static string getParent(const string& node);
};


