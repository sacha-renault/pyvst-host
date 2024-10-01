#include "src/cpp_host/PluginHost.hpp"
#include <iostream>


int main() {
    auto host = Steinberg::Vst::VstHost(); // vst3
    host.init("VitalInstaller/lib/vst3/Vital.vst3");
    auto params = host.getParameters();
    host.setParameter(10, 0.5);
    return 0;
}
