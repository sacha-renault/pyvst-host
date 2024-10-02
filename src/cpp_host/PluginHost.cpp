#include "PluginHost.hpp"
#include <iostream>

namespace Steinberg {
namespace Vst {

VstHost::VstHost(
    double samplerate,
    int maxSamplesPerBlock,
    int processMode,
    int symbolicSampleSize)
{
    setup_.maxSamplesPerBlock = maxSamplesPerBlock;
    setup_.sampleRate = samplerate;
    setup_.symbolicSampleSize = symbolicSampleSize;
    setup_.processMode = processMode;
};

VstHost::~VstHost() {
    controller->terminate();
    component->terminate();
    component = nullptr;
    controller = nullptr;
    terminate();
}

bool VstHost::init(const std::string& path /*, const VST3::Optional<VST3::UID>& effectID*/) {
    std::string error;
	module = VST3::Hosting::Module::create (path, error);
	if (!module)
	{
		std::string reason = "Could not create Module for file:";
		reason += path;
		reason += "\nError: ";
		reason += error;
		throw std::runtime_error(reason);
	}

    auto factory = module->getFactory ();
	for (auto& classInfo : factory.classInfos ())
	{
		if (classInfo.category () == kVstAudioEffectClass)
		{
			// If effectID filtering is needed, uncomment this block
            /*
            if (effectID)
            {
                if (*effectID != classInfo.ID())
                    continue;
            }
            */
			plugProvider = owned (new PlugProvider (factory, classInfo, true));
			break;
		}
	}

    if (!plugProvider)
	{
		// If effectID filtering is needed, uncomment this block
        /*
        std::string error;
        if (effectID)
            error = "No VST3 Audio Module Class with UID " + effectID->toString() + " found in file ";
        else
            error = "No VST3 Audio Module Class found in file ";
        */
        std::string error = "No VST3 Audio Module Class found in file ";
		error += path;
		throw std::runtime_error(error);
	}

    component = plugProvider->getComponent ();
	controller = plugProvider->getController ();

    if (!component || !controller) {
        std::cerr << "Failed to get component or controller from plugin." << std::endl;
        return false;
    }

    // Initialize component
    if (component->initialize(nullptr) != Steinberg::kResultOk || component->setActive(true) != Steinberg::kResultOk) {
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

    // get the processor object
    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> audioProcessor;
    if (component->queryInterface(Steinberg::Vst::IAudioProcessor::iid, (void**)&audioProcessor) == Steinberg::kResultOk && audioProcessor) {
        audioProcessor->setupProcessing(setup_);
    } else {
        throw std::runtime_error("Couldn't access audio interface");
    }

    // map all parameters into the unordered map
    // init number parameters
    int numParameters = controller->getParameterCount();
    for (int i = 0 ; i < numParameters ; ++i) {
        Steinberg::Vst::ParameterInfo infos;
        if (controller->getParameterInfo(i, infos) == Steinberg::kResultOk) {
            parametersTitleIdMap[Steinberg::Vst::StringConvert::convert(infos.title)] = infos.id;
        } else {
            std::cerr << "Error fetching param at index : " << i << "." << std::endl;
        }
    }

    return true;
}

void VstHost::terminate() {
    plugProvider = nullptr;
    module = nullptr;
    component = nullptr;
}

void VstHost::process(float** inputBuffers, float** outputBuffers, int numChannels, int numSamples) {
    // Ensure input and output buffers are valid
    if (numChannels <= 0) {
        throw std::runtime_error("Number of channels must be greater than zero.");
    }

    if (!inputBuffers || !outputBuffers) {
        throw std::runtime_error("Input and output buffers must be valid.");
    }

    // Prepare the process data object
    Steinberg::Vst::ProcessData processData = {};
    processData.numSamples = numSamples;

    // Set up input and output audio bus buffers
    Steinberg::Vst::AudioBusBuffers inputBus;
    inputBus.numChannels = numChannels; // Stereo
    inputBus.channelBuffers32 = inputBuffers;

    Steinberg::Vst::AudioBusBuffers outputBus;
    outputBus.numChannels = numChannels; // Stereo
    outputBus.channelBuffers32 = outputBuffers;

    // Assign buses to process data
    processData.inputs = &inputBus;
    processData.outputs = &outputBus;
    processData.numInputs = 1;
    processData.numOutputs = 1;

    // For parameter change
    Steinberg::Vst::ParameterChanges parameterChanges;
    prepareParametersChange(parameterChanges);
    processData.inputParameterChanges = &parameterChanges;

    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> audioProcessor;
    if (component->queryInterface(Steinberg::Vst::IAudioProcessor::iid, (void**)&audioProcessor) == Steinberg::kResultOk && audioProcessor) {
        audioProcessor->process(processData);
    } else {
        throw std::runtime_error("Couldn't access audio interface");
    }

    // Note: Ensure correct format conversions (e.g., sample rate, bit depth) if necessary.
}

void VstHost::process(Steinberg::Vst::EventList& eventList, float** outputBuffers, int numChannels, int numSamples) {
    // Ensure output buffers are valid
    if (numChannels <= 0) {
        std::cerr << "Number of channels must be greater than zero." << std::endl;
        return;
    }

    if (!outputBuffers) {
        std::cerr << "Output buffers must be valid." << std::endl;
        return;
    }

    // Prepare the process data object
    Steinberg::Vst::ProcessData processData = {};
    processData.numSamples = numSamples;

    // Set up output audio bus buffers
    Steinberg::Vst::AudioBusBuffers outputBus;
    outputBus.numChannels = numChannels;
    outputBus.channelBuffers32 = outputBuffers;

    // Assign buses to process data
    processData.outputs = &outputBus;
    processData.numOutputs = 1;

    // Assign event list to process data
    processData.inputEvents = &eventList;

    // For parameter change
    Steinberg::Vst::ParameterChanges parameterChanges;
    prepareParametersChange(parameterChanges);
    processData.inputParameterChanges = &parameterChanges;

    // Access the audio processor
    Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor> audioProcessor;
    if (component->queryInterface(Steinberg::Vst::IAudioProcessor::iid, (void**)&audioProcessor) == Steinberg::kResultOk && audioProcessor) {
        audioProcessor->process(processData);
    } else {
        throw std::runtime_error("Couldn't access audio interface");
    }

    // Note: Ensure correct format conversions (e.g., sample rate, bit depth) if necessary.
}

void VstHost::prepareParametersChange(Steinberg::Vst::ParameterChanges& parameterChanges) {
    for (const auto& entry : parametersChangeMap) {
        const unsigned int& paramID = entry.first;
        const double& value = entry.second;
        Steinberg::int32 queueIndex = -1;
        Steinberg::Vst::IParamValueQueue* queue = parameterChanges.addParameterData(paramID, queueIndex);
        if (queue) {
            Steinberg::int32 pointIndex = -1;
            queue->addPoint(0, value, pointIndex);
        }
    }

    // the parameter change wil be pased to audio processor,
    // We have to warn controller
    for (const auto& entry : parametersChangeMap) {
        const unsigned int& paramID = entry.first;
        const double& value = entry.second;
        controller->setParamNormalized(paramID, value);
    }

    // clear parametersChangeMap
    parametersChangeMap.clear();
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
                double value = controller->getParamNormalized(infos.id);
                parameters.emplace_back(convertInfoToParam(infos, value));
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
    parametersChangeMap[id] = value; // change value here.
}

void VstHost::setParameter(std::string title, double value) {
    setParameter(parametersTitleIdMap[title], value);
}

void VstHost::savePreset(std::string& path) {
    auto* state = new Steinberg::MemoryStream();
    if (component->getState(state) == Steinberg::kResultOk) {
        std::ofstream outFile("plugin_state.vstpreset", std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(reinterpret_cast<const char*>(state->getData()), state->getSize());
            outFile.close();
        }
    } else {
        throw std::runtime_error("Coudln't access processor state.");
    }
}

void VstHost::loadPreset(const std::string& path) {
    // Open the file in binary mode
    std::ifstream inFile(path, std::ios::binary);
    if (inFile.is_open()) {
        // Get the file size
        inFile.seekg(0, std::ios::end);
        std::streamsize size = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        // Read the file data into a buffer
        std::vector<char> buffer(size);
        if (inFile.read(buffer.data(), size)) {
            // Create a MemoryStream from the buffer
            auto* state = new Steinberg::MemoryStream(buffer.data(), size);
            
            // Set the state in the plugin component
            if (component->setState(state) != Steinberg::kResultOk) {
                delete state;
                throw std::runtime_error("Failed to set processor state.");
            }            

            // Clean up
            delete state;
        } else {
            throw std::runtime_error("Failed to read preset file.");
        }

        inFile.close();
    } else {
        throw std::runtime_error("Couldn't open preset file.");
    }
}

} // namespace Vst
} // namespace Steinberg
