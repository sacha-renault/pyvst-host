#include "src/cpp_host/PluginHost.hpp"
#include <iostream>
#include "public.sdk/source/common/memorystream.cpp"
#include <fstream>

int main() {
    auto host = Steinberg::Vst::VstHost(); // vst3
    host.init("VitalInstaller/lib/vst3/Vital.vst3");
        
        
}
