#include "src/cpp_host/PluginHost.hpp"
#include <iostream>


int main() {
    auto host = Steinberg::Vst::VstHost(); // vst3
    host.init("VitalInstaller/lib/vst3/Vital.vst3");
    auto params = host.getParameter();

    for (const auto& param : params) {
        std::cout << "Parameter: " << param.title << "\n";
        std::cout << "  Default Normalized Value: " << param.defaultNormalizedValue << "\n";
        std::cout << "  Can Automate: " << (param.canAutomate() ? "Yes" : "No") << "\n";
    }
    return 0;
}
