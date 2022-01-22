// PluginGravityN.hpp
// Aleksandar Koruga (aleksandar.koruga@gmail.com)

#pragma once

#include "SC_PlugIn.hpp"

namespace GravityN {

class GravityN : public SCUnit {
public:
    GravityN();

    // Destructor
    // ~GravityN();

private:
    // Calc function
    void next(int nSamples);

    // Member variables
};

} // namespace GravityN
