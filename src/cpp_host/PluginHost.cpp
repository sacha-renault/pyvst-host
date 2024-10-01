#include "PluginHost.hpp"

PluginHost::PluginHost(const std::string& pluginPath) {
    if (!loadPlugin(pluginPath)) {
        // throw std::runtime_error("Coudln't load vst");
        std::cerr << "Failed to load plugin from: " << pluginPath << std::endl;
    }
}

PluginHost::~PluginHost() {
    if (component) {
        component->terminate();
        component->release();
    }
    if (controller) {
        controller->terminate();
        controller->release();
    }
    if (libraryHandle) {
        dlclose(libraryHandle);
    }
}

bool PluginHost::loadPlugin(const std::string& path) {
    // Step 1: Load the dynamic library
    libraryHandle = dlopen(path.c_str(), RTLD_LAZY);
    if (!libraryHandle) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return false;
    }

    // Step 2: Get the GetPluginFactory function
    using GetFactoryProc = Steinberg::IPluginFactory* (*)();
    auto getFactory = (GetFactoryProc)dlsym(libraryHandle, "GetPluginFactory");
    if (!getFactory) {
        std::cerr << "Failed to get factory function: " << dlerror() << std::endl;
        return false;
    }

    // Step 3: Get the plugin factory
    Steinberg::IPluginFactory* factory = getFactory();
    if (!factory) {
        std::cerr << "Failed to get plugin factory." << std::endl;
        return false;
    }

    // Step 4: Iterate over available classes and create an instance
    Steinberg::int32 count = factory->countClasses();
    for (Steinberg::int32 i = 0; i < count; ++i) {
        Steinberg::PClassInfo classInfo = {};
        if (factory->getClassInfo(i, &classInfo) == Steinberg::kResultOk) {
            std::cout << "Found plugin class: " << classInfo.name << ", Category: " << classInfo.category << std::endl;

            if (strcmp(classInfo.category, kVstAudioEffectClass) == 0) {
                // Create the audio component
                if (factory->createInstance(classInfo.cid, Steinberg::Vst::IComponent::iid, (void**)&component) == Steinberg::kResultOk) {
                    if (component->initialize(nullptr) == Steinberg::kResultOk) {
                        std::cout << "Successfully created IComponent instance: " << classInfo.name << std::endl;
                    } else {
                        std::cerr << "Failed to initialize IComponent." << std::endl;
                        return false;
                    }
                } else {
                    std::cerr << "kVstAudioEffectClass wasn't loaded" << std::endl;
                }
            } else if (strcmp(classInfo.category, kVstComponentControllerClass) == 0) {
                // Create the controller
                if (factory->createInstance(classInfo.cid, Steinberg::Vst::IEditController::iid, (void**)&controller) == Steinberg::kResultOk) {
                    if (controller->initialize(nullptr) == Steinberg::kResultOk) {
                        std::cout << "Successfully created IEditController instance: " << classInfo.name << std::endl;

                        // Link the controller to the component if both are available
                        if (component) {
                            controller->setComponentState(nullptr); // Typically, you'd provide state data here
                        }
                    } else {
                        std::cerr << "Failed to initialize IEditController." << std::endl;
                        return false;
                    }
                } else {
                    std::cerr << "kVstComponentControllerClass wasn't loaded" << std::endl;
                }
            } else {
                throw std::runtime_error("Unknown vst component category");
            }
        }
    }

    if (component && controller) {
        return true; // Both component and controller are loaded successfully
    } else {
        std::cerr << "No suitable component or controller found in the plugin." << std::endl;
        return false;
    }
}