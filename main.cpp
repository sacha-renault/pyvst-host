#include "src/cpp_host/PluginHost.hpp"


int main() {
    auto host = PluginHost("VitalInstaller/lib/vst3/Vital.vst3/Contents/x86_64-linux/Vital.so");
    host.listParameters();
    host.setParameter(0, 0.5);
    host.listParameters();

    return 0;
}