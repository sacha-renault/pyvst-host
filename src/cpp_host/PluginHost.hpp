#pragma once

#include "vst/hosting/module.h"
#include "vst/hosting/plugprovider.h"
#include "vst/utility/optional.h"
#include "vst/hosting/eventlist.h"
#include "vst/hosting/parameterchanges.h"
#include "vst/utility/stringconvert.h"
#include "common/memorystream.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivsthostapplication.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

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

    bool init(const std::string& path /*, const VST3::Optional<VST3::UID>& effectID = {}*/);
    void terminate();

    // Function for effet (process audio input)
    void process(float** inputBuffers, float** outputBuffers, int numChannels, int numSamples);

    // Function for synth (process a serie of event)
    void process(Steinberg::Vst::EventList& eventList, float** outputBuffers, int numChannels, int numSamples);

    // param & states
    std::vector<VstParameter> getParameters();
    void setParameter(unsigned int id, double value);
    void setParameter(std::string title, double value);
    void savePreset(std::string& path);
    void loadPreset(const std::string& path);

private:
    VST3::Hosting::Module::Ptr module {nullptr};
    IPtr<PlugProvider> plugProvider {nullptr};
    IPtr<IComponent> component {nullptr};
    IPtr<IEditController> controller {nullptr};
    std::unordered_map<unsigned int, double> parametersChangeMap;
    std::unordered_map<std::string, unsigned int> parametersTitleIdMap;
    Steinberg::Vst::ProcessSetup setup_;

    void prepareParametersChange(Steinberg::Vst::ParameterChanges& parameterChanges);
};

} // namespace Vst
} // namespace Steinberg
