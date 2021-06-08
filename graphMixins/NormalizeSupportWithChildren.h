#pragma once

#include "NormalizeSupport.h"

template <typename BASE>
class NormalizeSupportWithChildren : public NormalizeSupport<BASE> {
    static_assert(std::is_base_of<CouplingGraph, BASE>::value, "can only apply mixin to coupling graphs");
public:
    virtual vector<string> getChildren(const string& node) = 0;
    virtual float getAbsoluteSelfSupport(const string& node) = 0;

    float getAbsoluteSupport(const string& node) override;
};


#include "NormalizeSupportWithChildren.inl"
