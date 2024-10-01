#include "src/cpp_host/PluginHost.hpp"


int main() {
    auto hosted = PluginHost("VitalInstaller/lib/vst3/Vital.vst3/Contents/x86_64-linux/Vital.so");
    return 0;
}