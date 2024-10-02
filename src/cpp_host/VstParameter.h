#pragma once
#include "vst/utility/stringconvert.h"
#include "vst/hosting/plugprovider.h"
#include "vst/utility/optional.h"

struct VstParameter {
    std::string title;
    std::string shortTitle;
    double defaultNormalizedValue;
    double value;
    unsigned int id;
    int flags;
    int stepCount;
    int unitId;
    std::string units;

    // Helper functions to interpret flags
    bool canAutomate() const { return flags & Steinberg::Vst::ParameterInfo::kCanAutomate; }
    bool isBypass() const { return flags & Steinberg::Vst::ParameterInfo::kIsBypass; }
    bool isHidden() const { return flags & Steinberg::Vst::ParameterInfo::kIsHidden; }
    bool isList() const { return flags & Steinberg::Vst::ParameterInfo::kIsList; }
    bool isProgramChange() const { return flags & Steinberg::Vst::ParameterInfo::kIsProgramChange; }
    bool isReadOnly() const { return flags & Steinberg::Vst::ParameterInfo::kIsReadOnly; }
    bool isWrapAround() const { return flags & Steinberg::Vst::ParameterInfo::kIsWrapAround; }
};


inline VstParameter convertInfoToParam(const Steinberg::Vst::ParameterInfo& info, double value) {
    VstParameter param;

    try {
        // Convert UTF-16 strings to UTF-8
        if (info.title) {
            param.title = Steinberg::Vst::StringConvert::convert(info.title);
        }
        if (info.shortTitle) {
            param.shortTitle = Steinberg::Vst::StringConvert::convert(info.shortTitle);
        }
        if (info.units) {
            param.units = Steinberg::Vst::StringConvert::convert(info.units);
        }
    } catch (const std::exception& e) {
        // Handle the error appropriately, e.g., by assigning default strings
        param.title = "[Unknown Title]";
        param.shortTitle = "[Unknown Short Title]";
        param.units = "[Unknown Units]";
    }

    // Assign the rest of the ParameterInfo fields
    param.value = value;
    param.defaultNormalizedValue = info.defaultNormalizedValue;
    param.id = info.id;
    param.flags = info.flags;
    param.stepCount = info.stepCount;
    param.unitId = info.unitId;

    return param;
}

