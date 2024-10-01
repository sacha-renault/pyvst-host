#include "PluginHost.hpp"
#include "MemoryStream.h"

PluginHost::PluginHost(const std::string& pluginPath) {
    if (!loadPlugin(pluginPath)) {
        // throw std::runtime_error("Coudln't load vst");
        std::cerr << "Failed to load plugin from: " << pluginPath << std::endl;
    } else {
        component->setState(nullptr);
        if (component->setActive(true) != Steinberg::kResultOk) {
            std::cerr << "Failed to activate the component." << std::endl;
            return;
        }

        // Get the component state and apply it to the controller
        MemoryStream componentStateStream;
        if (component->getState(&componentStateStream) == Steinberg::kResultOk) {
            if (controller->setComponentState(&componentStateStream) != Steinberg::kResultOk) {
                std::cerr << "Failed to set component state for controller." << std::endl;
                return;
            }
        } else {
            std::cerr << "Failed to get component state from component." << std::endl;
            return;
        }
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
#ifdef _WIN32
    libraryHandle = LoadLibraryA(path.c_str());
    if (!libraryHandle) {
        std::cerr << "Failed to load library: " << path << std::endl;
        return false;
    }

    using GetFactoryProc = Steinberg::IPluginFactory* (*)();
    GetFactoryProc getFactory = reinterpret_cast<GetFactoryProc>(GetProcAddress(libraryHandle, "GetPluginFactory"));
    if (!getFactory) {
        std::cerr << "Failed to get factory function from library: " << path << std::endl;
        return false;
    }
#else
    libraryHandle = dlopen(path.c_str(), RTLD_LAZY);
    if (!libraryHandle) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return false;
    }

    using GetFactoryProc = Steinberg::IPluginFactory* (*)();
    GetFactoryProc getFactory = reinterpret_cast<GetFactoryProc>(dlsym(libraryHandle, "GetPluginFactory"));
    if (!getFactory) {
        std::cerr << "Failed to get factory function: " << dlerror() << std::endl;
        return false;
    }
#endif

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