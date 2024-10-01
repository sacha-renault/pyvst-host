#include "PluginHost.hpp"
#include <iostream>

namespace Steinberg {
namespace Vst {

VstHost::VstHost() = default;

VstHost::~VstHost() {
    terminate();
}

bool VstHost::init(const std::string& path, VST3::Optional<VST3::UID> effectID) {
    std::string error;
	module = VST3::Hosting::Module::create (path, error);
	if (!module)
	{
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
		std::cerr << reason << std::endl;
        return false;
	}

    auto factory = module->getFactory ();
	for (auto& classInfo : factory.classInfos ())
	{
		if (classInfo.category () == kVstAudioEffectClass)
		{
			if (effectID)
			{
				if (*effectID != classInfo.ID ())
					continue;
			}
			plugProvider = owned (new PlugProvider (factory, classInfo, true));
			break;
		}
	}

    if (!plugProvider)
	{
		std::string error;
		if (effectID)
			error =
			    "No VST3 Audio Module Class with UID " + effectID->toString () + " found in file ";
		else
			error = "No VST3 Audio Module Class found in file ";
		error += path;
		std::cerr << error << std::endl;
        return false;
	}

    OPtr<IComponent> component = plugProvider->getComponent ();
	OPtr<IEditController> controller = plugProvider->getController ();

    if (!component || !controller) {
        std::cerr << "Failed to get component or controller from plugin." << std::endl;
        return false;
    }

    // Initialize component
    if (component->initialize(nullptr) != Steinberg::kResultOk) {
        std::cerr << "Failed to initialize component." << std::endl;
        return false;
    }

    if (controller->initialize(nullptr) != Steinberg::kResultOk) {
        std::cerr << "Failed to initialize controller." << std::endl;
        return false;
    }

    // Connect component and controller (optional, but good for parameter updates)
    Steinberg::FUnknownPtr<Steinberg::Vst::IConnectionPoint> componentConnection(component);
    Steinberg::FUnknownPtr<Steinberg::Vst::IConnectionPoint> controllerConnection(controller);
    if (componentConnection && controllerConnection) {
        componentConnection->connect(controllerConnection);
        controllerConnection->connect(componentConnection);
    }

    return true;
}

void VstHost::terminate() {
    plugProvider = nullptr;
    module = nullptr;
    processor = nullptr;
}

void VstHost::processAudio(const std::string& inputFile, const std::string& outputFile) {
    // Placeholder: implement audio file reading, plugin processing, and file writing.
    std::cout << "Processing audio from: " << inputFile << " to: " << outputFile << std::endl;

    // Example workflow:
    // 1. Load input audio data into a buffer (e.g., using an audio library).
    // 2. Prepare the buffer to be processed by the plugin.
    // 3. Call the plugin's `process()` method to modify the audio data.
    // 4. Write the output buffer to the output file.

    // Note: Ensure correct format conversions (e.g., sample rate, bit depth) if necessary.
}

std::vector<VstParameter> VstHost::getParameter() {
    auto controller = plugProvider->getController();
    if (controller) {
        // init number parameters
        int numParameters = controller->getParameterCount();
        std::vector<VstParameter> parameters;
        parameters.reserve(numParameters);  // Optional: Reserve to avoid reallocations


        // get
        for (int i = 0 ; i < numParameters; ++i ) {
            Steinberg::Vst::ParameterInfo infos;
            if (controller->getParameterInfo(i, infos) == Steinberg::kResultOk) {
                parameters.emplace_back(convertInfoToParam(infos));
            } else {
                std::cerr << "Error fetching param at index : " << i << "." << std::endl;
            }
        }

        return parameters;
    } else {
        throw std::runtime_error("Controller wasn't initialized");
    }
}

} // namespace Vst
} // namespace Steinberg
