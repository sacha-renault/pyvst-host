#include "src/cpp_host/PluginHost.hpp"


int main() {
    auto host = Steinberg::Vst::VstHost(); // vst3
    host.init("VitalInstaller/lib/vst3/Vital.vst3");
    return 0;
}
