#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

class CouplingGraph {
private:
    string name;

public:
    explicit CouplingGraph(string name);

    [[nodiscard]] const string& getName() const;

    virtual const vector<string>& getNodeSet();
    virtual double getNormalizedSupport(const string& node) = 0;
    virtual double getNormalizedCoupling(const string& a, const string& b) = 0;

    static string saveLocation(const string& repoName, const string& name, const string& storageDir);
    void save(const string& repoName, const string& storageDir);
    static shared_ptr<CouplingGraph> load(const string& repoName, const string& name, const string& storageDir);
    virtual void plaintextSave(ostream& out);
    virtual void plaintextLoad(istream& in);

    virtual void printStatistics() = 0;

    double howWellPredictsMissingNode(const vector<string>& nodeSet, const string& nodeMissingFromSet, const vector<string>& allNodes);

public:
    static string getParent(const string& node);
};


