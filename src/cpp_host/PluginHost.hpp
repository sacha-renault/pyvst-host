#pragma once

#ifdef _WIN32
#include <windows.h>  // For LoadLibrary, GetProcAddress
#else
#include <dlfcn.h>    // For dlopen, dlsym (Linux/macOS)
#endif

#include <iostream>
#include <string>
#include <vector>
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "main/pluginfactory.h"

class PluginHost {
public:
    PluginHost(const std::string& pluginPath);
    ~PluginHost();

    // audio processing
    std::vector<float> processMidiInput(const std::vector<uint8_t>& midiInput, int numSamples);

private:
    void* libraryHandle = nullptr;
    Steinberg::Vst::IComponent* component = nullptr;
    Steinberg::Vst::IEditController* controller = nullptr;
    bool loadPlugin(const std::string& path);
};
