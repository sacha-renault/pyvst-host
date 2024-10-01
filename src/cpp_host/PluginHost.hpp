#pragma once
#include <iostream>
#include <string>
#include <dlfcn.h>  // For Linux/macOS
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "main/pluginfactory.h"

class PluginHost {
public:
    PluginHost(const std::string& pluginPath);
    ~PluginHost();

private:
    void* libraryHandle = nullptr;
    Steinberg::Vst::IComponent* component = nullptr;
    Steinberg::Vst::IEditController* controller = nullptr;
    bool loadPlugin(const std::string& path);
};
