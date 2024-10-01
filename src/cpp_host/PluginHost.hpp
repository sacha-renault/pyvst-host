#pragma once

#include "vst/hosting/module.h"
#include "vst/hosting/plugprovider.h"
#include "vst/utility/optional.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "vst/hosting/parameterchanges.h"
#include "vst/utility/stringconvert.h"
#include <string>
#include <vector>
#include <unordered_map>

#include "VstParameter.h"

namespace Steinberg {
namespace Vst {

class VstHost {
public:
    VstHost(
        double samplerate = 44100.0,
        int maxSamplesPerBlock = 512,
        int processMode = Steinberg::Vst::ProcessModes::kOffline,
        int symbolicSampleSize = Steinberg::Vst::SymbolicSampleSizes::kSample32);
    ~VstHost();

    bool init(const std::string& path, VST3::Optional<VST3::UID> effectID = {});
    void terminate();
    void process(std::vector<float>& inputBuffer, std::vector<float>& outputBuffer);

    std::vector<VstParameter> getParameters();
    void setParameter(unsigned int id, double value);

private:
    VST3::Hosting::Module::Ptr module {nullptr};
    IPtr<PlugProvider> plugProvider {nullptr};
    IPtr<IComponent> component {nullptr};
    IPtr<IEditController> controller {nullptr};
    std::unordered_map<unsigned int, double> parametersChangeMap;
    Steinberg::Vst::ProcessSetup setup_;
};

} // namespace Vst
} // namespace Steinberg
