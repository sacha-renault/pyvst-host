#pragma once
#include <vector>
#include "PluginHost.hpp"

inline Steinberg::Vst::EventList create_midi_events(const std::vector<std::tuple<int, int, float, int, int>>& notes) {
    // Create an EventList instance
    Steinberg::Vst::EventList eventList;

    for (const auto& note : notes) {
        int channel = std::get<0>(note);
        int pitch = std::get<1>(note);
        float velocity = std::get<2>(note);
        int on_offset = std::get<3>(note);  // Note On sample offset
        int off_offset = std::get<4>(note); // Note Off sample offset

        // Add Note On event if on_offset is specified
        if (on_offset != -1) {
            Steinberg::Vst::Event event = {};
            event.type = Steinberg::Vst::Event::kNoteOnEvent;
            event.noteOn.channel = channel;
            event.noteOn.pitch = pitch;
            event.noteOn.velocity = velocity;
            event.sampleOffset = on_offset; // Set the sample offset for Note On
            eventList.addEvent(event);
        }

        // Add Note Off event if off_offset is specified
        if (off_offset != -1) {
            Steinberg::Vst::Event event = {};
            event.type = Steinberg::Vst::Event::kNoteOffEvent;
            event.noteOff.channel = channel;
            event.noteOff.pitch = pitch;
            event.noteOff.velocity = velocity;
            event.sampleOffset = off_offset; // Set the sample offset for Note Off
            eventList.addEvent(event);
        }
    }
    return eventList;
}
