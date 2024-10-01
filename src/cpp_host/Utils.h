#pragma once
#include <vector>
#include "PluginHost.hpp"

// A utility function to create MIDI note events
inline std::vector<Steinberg::Vst::Event> create_midi_events(const std::vector<std::tuple<int, int, float, bool>>& notes) {
    // Vector to store VST events
    std::vector<Steinberg::Vst::Event> events;

    for (const auto& note : notes) {
        int channel = std::get<0>(note);
        int pitch = std::get<1>(note);
        float velocity = std::get<2>(note);
        bool is_note_on = std::get<3>(note);

        Steinberg::Vst::Event event = {};
        if (is_note_on) {
            event.type = Steinberg::Vst::Event::kNoteOnEvent;
            event.noteOn.channel = channel;
            event.noteOn.pitch = pitch;
            event.noteOn.velocity = velocity;
        } else {
            event.type = Steinberg::Vst::Event::kNoteOffEvent;
            event.noteOff.channel = channel;
            event.noteOff.pitch = pitch;
            event.noteOff.velocity = velocity;
        }

        events.push_back(event);
    }

    return events;
}