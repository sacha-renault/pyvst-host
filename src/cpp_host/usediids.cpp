#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/base/ibstream.h"
#include "public.sdk/source/vst/vstinitiids.cpp"

// pluginterfaces/vst/ivsttestplugprovider.h

// DEF_CLASS_IID (Steinberg::Vst::IComponent)
// DEF_CLASS_IID (Steinberg::Vst::IEditController)
DEF_CLASS_IID (Steinberg::FUnknown)
DEF_CLASS_IID (Steinberg::IPluginBase)
DEF_CLASS_IID (Steinberg::IPluginFactory)
DEF_CLASS_IID (Steinberg::IPluginFactory2)
DEF_CLASS_IID (Steinberg::IPluginFactory3)
DEF_CLASS_IID (Steinberg::IBStream)
// DEF_CLASS_IID (Steinberg::Vst::IConnectionPoint)