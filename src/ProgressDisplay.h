#pragma once

#include <chrono>

#include "util.h"

using namespace std;

class ProgressDisplay {
private:
    inline static string description;
    inline static int maximum;
    inline static int current;
    inline static bool hasPrintedOnce;
    inline static chrono::time_point<chrono::steady_clock> lastShownUpdate;

    constexpr static const chrono::duration displayDelay = 1000ms;
    constexpr static const chrono::duration updateInterval = 200ms;
public:
    static void init(const string& description, int maximum = 100);
    static void showImmediately();
    static void update(int current, int maximum);
    static void update(int current);
    static void update();
    static void close();
private:
    static void _printUpdateMaybe(bool forceImmediateUpdate = false);
};

