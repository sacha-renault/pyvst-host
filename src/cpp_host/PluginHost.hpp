#pragma once

#include "vst/hosting/module.h"
#include "vst/hosting/plugprovider.h"
#include "vst/utility/optional.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include <string>

namespace Steinberg {
namespace Vst {

class VstHost {
public:
    VstHost();
    ~VstHost();

    bool init(const std::string& path, VST3::Optional<VST3::UID> effectID = {});
    void terminate();
    void processAudio(const std::string& inputFile, const std::string& outputFile);

    void getParameter();

private:
    VST3::Hosting::Module::Ptr module {nullptr};
    IPtr<PlugProvider> plugProvider {nullptr};
    IPtr<IComponent> processor {nullptr};
};

} // namespace Vst
} // namespace Steinberg
