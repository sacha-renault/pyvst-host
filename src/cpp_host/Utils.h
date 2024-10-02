#pragma once
#include <vector>
#include "PluginHost.hpp"

inline Steinberg::Vst::EventList create_midi_events(const std::vector<std::tuple<int, int, float, bool>>& notes) {
    // Create an EventList instance
    Steinberg::Vst::EventList eventList;

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

        eventList.addEvent(event);
    }

    return eventList;
}