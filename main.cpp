#include "src/cpp_host/PluginHost.hpp"
#include <iostream>
#include <sndfile.h>  // Include the libsndfile header

int main() {
    auto host = Steinberg::Vst::VstHost(); // vst3
    host.init("VitalInstaller/lib/vst3/Vital.vst3");
    
    // Sample rate, number of channels, and duration (in seconds)
    const int sampleRate = 44100;
    const int numChannels = 2;
    const int durationInSeconds = 1;
    const int numSamples = sampleRate * durationInSeconds;

    // Prepare output buffers (stereo)
    float* outputBuffers[2];
    outputBuffers[0] = new float[numSamples];
    outputBuffers[1] = new float[numSamples];

    // Zero initialize the output buffers
    std::fill(outputBuffers[0], outputBuffers[0] + numSamples, 0.0f);
    std::fill(outputBuffers[1], outputBuffers[1] + numSamples, 0.0f);

    // Prepare MIDI note-on event for C5
    Steinberg::Vst::EventList eventList;
    Steinberg::Vst::Event noteOnEvent = {};
    noteOnEvent.type = Steinberg::Vst::Event::kNoteOnEvent;
    noteOnEvent.noteOn.channel = 0;    // MIDI channel 0
    noteOnEvent.noteOn.pitch = 72;     // MIDI note number for C5
    noteOnEvent.noteOn.velocity = 1.0f; // Full velocity
    noteOnEvent.noteOn.length = 0;      // Note length (if needed)
    eventList.addEvent(noteOnEvent);

    // Prepare the process for 1 second of audio
    host.process(eventList, outputBuffers, numChannels, numSamples);

    // Here, you could save the audio to a file, play it, or perform further processing
    // For demonstration purposes, we'll just print a confirmation
    std::cout << "Processed 1 second of audio with a C5 note-on event." << std::endl;

    // Combine the output buffers into an interleaved array for stereo WAV output
    std::vector<float> interleavedOutput(numSamples * numChannels);
    for (int i = 0; i < numSamples; ++i) {
        interleavedOutput[i * numChannels] = outputBuffers[0][i];         // Left channel
        interleavedOutput[i * numChannels + 1] = outputBuffers[1][i];     // Right channel
    }

    // Set up SF_INFO for WAV file
    SF_INFO sfinfo;
    sfinfo.frames = numSamples;
    sfinfo.samplerate = sampleRate;
    sfinfo.channels = numChannels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // WAV format with 16-bit PCM

    // Open the output file
    SNDFILE* outFile = sf_open("output.wav", SFM_WRITE, &sfinfo);
    if (!outFile) {
        std::cerr << "Error opening output file: " << sf_strerror(outFile) << std::endl;
        delete[] outputBuffers[0];
        delete[] outputBuffers[1];
        return -1;
    }

    // Write interleaved audio data to the file
    sf_write_float(outFile, interleavedOutput.data(), interleavedOutput.size());

    // Close the output file
    sf_close(outFile);

    // Cleanup output buffers
    delete[] outputBuffers[0];
    delete[] outputBuffers[1];

    std::cout << "Processed and saved 1 second of audio with a C5 note-on event to output.wav." << std::endl;

    return 0;
}
