#include "PluginHost.hpp"

void PluginHost::setParameter(Steinberg::Vst::ParamID paramID, float normalizedValue) {
    if (controller) {
        if (normalizedValue < 0.0f || normalizedValue > 1.0f) {
            std::cerr << "Normalized value must be between 0.0 and 1.0." << std::endl;
            return;
        }

        controller->setParamNormalized(paramID, normalizedValue);
        std::cout << "Parameter with ID " << paramID << " set to normalized value " << normalizedValue << std::endl;

        // Optionally inform the component of the change if it is necessary
        if (component) {
            Steinberg::Vst::ParamValue actualValue = controller->getParamNormalized(paramID);
            component->setState(nullptr);  // Sometimes you need to sync states explicitly
            std::cout << "Component updated with new parameter value." << std::endl;
        }
    } else {
        std::cerr << "Controller is not initialized." << std::endl;
    }
}

void PluginHost::listParameters() {
    if (!controller) {
        std::cerr << "Controller is not initialized." << std::endl;
        return;
    }

    // Get the number of parameters from the controller
    Steinberg::int32 paramCount = controller->getParameterCount();
    if (paramCount == 0) {
        // std::cout << "No parameters found for the plugin." << std::endl;
        // return;
        paramCount = 10;
    }

    std::cout << "Parameters of the plugin:" << std::endl;

    // Iterate through all parameters
    for (Steinberg::int32 i = 0; i < paramCount; ++i) {
        Steinberg::Vst::ParameterInfo paramInfo = {};
        if (controller->getParameterInfo(i, paramInfo) == Steinberg::kResultOk) {
            std::cout << "Parameter " << i + 1 << ": " << paramInfo.title
                      << ", ID: " << paramInfo.id
                      << ", Default Value: " << paramInfo.defaultNormalizedValue
                      << ", Unit: " << paramInfo.units << std::endl;
        } else {
            std::cerr << "Failed to get info for parameter " << i << std::endl;
        }
    }
}
