#include "PluginHost.hpp"
#include <iostream>

namespace Steinberg {
namespace Vst {

VstHost::VstHost() = default;

VstHost::~VstHost() {
    controller->terminate();
    processor->terminate();
    processor = nullptr;
    controller = nullptr;
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

    processor = plugProvider->getComponent ();
	controller = plugProvider->getController ();

    if (!processor || !controller) {
        std::cerr << "Failed to get component or controller from plugin." << std::endl;
        return false;
    }

    // Initialize component
    if (processor->initialize(nullptr) != Steinberg::kResultOk || processor->setActive(true) != Steinberg::kResultOk) {
        std::cerr << "Failed to initialize component." << std::endl;
        return false;
    }

    if (controller->initialize(nullptr) != Steinberg::kResultOk) {
        std::cerr << "Failed to initialize controller." << std::endl;
        return false;
    }

    // Connect component and controller (optional, but good for parameter updates)
    Steinberg::FUnknownPtr<Steinberg::Vst::IConnectionPoint> componentConnection(processor);
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

    // prepare the process data object
    Steinberg::Vst::ProcessData processData = {};

    // Example workflow:
    Steinberg::Vst::ParameterChanges parameterChanges;
    for (const auto& [paramID, value] : parametersChangeMap) {
        Steinberg::int32 queueIndex = -1;
        Steinberg::Vst::IParamValueQueue* queue = parameterChanges.addParameterData(paramID, queueIndex);
        if (queue) {
            Steinberg::int32 pointIndex = -1;
            queue->addPoint(0, value, pointIndex);
        }
    }
    processData.inputParameterChanges = &parameterChanges;

    // 1. Load input audio data into a buffer (e.g., using an audio library).
    // 2. Prepare the buffer to be processed by the plugin.
    // 3. Call the plugin's `process()` method to modify the audio data.
    // 4. Write the output buffer to the output file.

    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> audioProcessor;
    if (processor->queryInterface(Steinberg::Vst::IAudioProcessor::iid, (void**)&audioProcessor) == Steinberg::kResultOk && audioProcessor) {
        // audioProcessor->process();
    } else {
        throw std::runtime_error("Couldn't access audio interface");
    }

    // clear parametersChangeMap
    parametersChangeMap.clear();

    // Note: Ensure correct format conversions (e.g., sample rate, bit depth) if necessary.
}

std::vector<VstParameter> VstHost::getParameters() {
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

void VstHost::setParameter(unsigned int id, double value) {
    // Ensure value is normalized to the range [0.0, 1.0]
    if (value > 1.0) value = 1.0;
    if (value < 0.0) value = 0.0;
}

} // namespace Vst
} // namespace Steinberg
