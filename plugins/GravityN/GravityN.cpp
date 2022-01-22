// PluginGravityN.cpp
// Aleksandar Koruga (aleksandar.koruga@gmail.com)

#include "SC_PlugIn.hpp"
#include "GravityN.hpp"

static InterfaceTable* ft;

namespace GravityN {

GravityN::GravityN() {
    mCalcFunc = make_calc_function<GravityN, &GravityN::next>();
    next(1);
}

void GravityN::next(int nSamples) {
    const float* input = in(0);
    const float* gain = in(1);
    float* outbuf = out(0);

    // simple gain function
    for (int i = 0; i < nSamples; ++i) {
        outbuf[i] = input[i] * gain[i];
    }
}

} // namespace GravityN

PluginLoad(GravityNUGens) {
    // Plugin magic
    ft = inTable;
    registerUnit<GravityN::GravityN>(ft, "GravityN", false);
}
