#include "ProgressDisplay.h"

#include <iostream>

void ProgressDisplay::init(const string& description, int maximum) {
    ProgressDisplay::description = description;
    ProgressDisplay::maximum = maximum;
    ProgressDisplay::current = 0;
    ProgressDisplay::hasPrintedOnce = false;
    ProgressDisplay::lastShownUpdate = chrono::steady_clock::now();
}

void ProgressDisplay::showImmediately() {
    hasPrintedOnce = true;
    _printUpdateMaybe(true);
}

void ProgressDisplay::update(int current, int maximum) {
    ProgressDisplay::current = current;
    ProgressDisplay::maximum = maximum;
    _printUpdateMaybe();
}

void ProgressDisplay::update(int current) {
    ProgressDisplay::current = current;
    _printUpdateMaybe();
}

void ProgressDisplay::update() {
    if (current >= maximum) return;
    ProgressDisplay::current++;
    _printUpdateMaybe();
}

void ProgressDisplay::close() {
    current = maximum;
    _printUpdateMaybe(true);
}

void ProgressDisplay::_printUpdateMaybe(bool forceImmediateUpdate) {
    auto& requiredDeltaTime = hasPrintedOnce ? updateInterval : displayDelay;
    auto now = chrono::steady_clock::now();
    if (((now - lastShownUpdate) >= requiredDeltaTime) || (forceImmediateUpdate && hasPrintedOnce)) [[unlikely]] {
        cout << "#progress " << current << " " << maximum << " " << description << endl;
        cout.flush();
        lastShownUpdate = now;
        hasPrintedOnce = true;
    }
}
