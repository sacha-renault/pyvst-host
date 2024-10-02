#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>

#include "cpp_host/PluginHost.hpp"  // Change this include to the library you are binding
#include "cpp_host/Utils.h"

namespace py = pybind11;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// <litgen_glue_code>  // Autogenerated code below! Do not edit!

// </litgen_glue_code> // Autogenerated code end
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


class AudioBufferWrapper {
public:
    // Constructor to create an empty buffer
    AudioBufferWrapper(int numChannels, int numSamples)
        : numChannels_(numChannels), numSamples_(numSamples) {
        // Allocate memory for channels and data
        buffer_ = new float*[numChannels_];
        data_ = new float[numChannels_ * numSamples_];

        // Assign pointers for each channel
        for (int channel = 0; channel < numChannels_; ++channel) {
            buffer_[channel] = data_ + (channel * numSamples_);
        }
    }

    // Constructor to create from a NumPy array
    AudioBufferWrapper(py::array_t<float> numpyArray) {
        // Ensure the NumPy array is 2D and has compatible dimensions
        if (numpyArray.ndim() != 2) {
            throw std::runtime_error("NumPy array must be 2-dimensional (samples x channels).");
        }

        numSamples_ = numpyArray.shape(0);
        numChannels_ = numpyArray.shape(1);

        // Get the pointer to the NumPy data (no copy)
        float* dataPtr = static_cast<float*>(numpyArray.request().ptr);

        // Allocate memory for channel pointers
        buffer_ = new float*[numChannels_];

        // Point each channel pointer to the corresponding offset in the NumPy data
        for (int channel = 0; channel < numChannels_; ++channel) {
            buffer_[channel] = dataPtr + (channel * numSamples_);
        }

        // We do not allocate `data_` in this case as we are referencing external data.
        data_ = nullptr; // Data managed by NumPy, no need to delete
    }

    // Destructor
    ~AudioBufferWrapper() {
        if (data_) {
            delete[] data_; // Only delete if we allocated data ourselves
        }
        delete[] buffer_;
    }

    // Get the float** buffer (for C++ use)
    float** getBuffer() {
        return buffer_;
    }

    int getNumChannels() {
        return numChannels_;
    }

    int getNumSamples() {
        return numSamples_;
    }

    // Expose a method to get the data as a NumPy array
    py::array_t<float> to_numpy() {
        if (!data_) {
            throw std::runtime_error("Cannot convert to NumPy array as the buffer is not owned.");
        }

        // Create a NumPy array from the existing data
        return py::array_t<float>(
            {numSamples_, numChannels_},              // Shape: samples x channels
            {sizeof(float) * numChannels_, sizeof(float)}, // Strides
            data_                                     // Data pointer
        );
    }

private:
    int numChannels_;
    int numSamples_;
    float** buffer_;
    float* data_;
};


void py_init_module_pyvst3_host(py::module& m)      //  rename this function name!!!
{
    // You can add any code here


    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // <litgen_pydef> // Autogenerated code below! Do not edit!
    ////////////////////    <generated_from:PluginHost.hpp>    ////////////////////
    auto pyClassVstHost =
        py::class_<Steinberg::Vst::VstHost>
            (m, "VstHost", "")
        .def(py::init<double, int, int, int>(),
            py::arg("samplerate") = 44100.0, py::arg("max_samples_per_block") = 512, py::arg("process_mode") = Steinberg::Vst::ProcessModes::kOffline, py::arg("symbolic_sample_size") = Steinberg::Vst::SymbolicSampleSizes::kSample32)
        .def("init",
            &Steinberg::Vst::VstHost::init, py::arg("path"), py::arg("effect_id") = VST3::Optional<VST3::UID>{})
        .def("terminate",
            &Steinberg::Vst::VstHost::terminate)
        .def("get_parameters",
            &Steinberg::Vst::VstHost::getParameters)
        .def("set_parameter",
            &Steinberg::Vst::VstHost::setParameter, py::arg("id"), py::arg("value"))
        ;
    ////////////////////    </generated_from:PluginHost.hpp>    ////////////////////


    ////////////////////    <generated_from:Utils.h>    ////////////////////
    ////////////////////    </generated_from:Utils.h>    ////////////////////


    ////////////////////    <generated_from:VstParameter.h>    ////////////////////
    auto pyClassVstParameter =
        py::class_<VstParameter>
            (m, "VstParameter", "")
        .def(py::init<>([](
        std::string title = std::string(), std::string shortTitle = std::string(), double defaultNormalizedValue = double(), int flags = int(), int stepCount = int(), int unitId = int(), std::string units = std::string())
        {
            auto r = std::make_unique<VstParameter>();
            r->title = title;
            r->shortTitle = shortTitle;
            r->defaultNormalizedValue = defaultNormalizedValue;
            r->flags = flags;
            r->stepCount = stepCount;
            r->unitId = unitId;
            r->units = units;
            return r;
        })
        , py::arg("title") = std::string(), py::arg("short_title") = std::string(), py::arg("default_normalized_value") = double(), py::arg("flags") = int(), py::arg("step_count") = int(), py::arg("unit_id") = int(), py::arg("units") = std::string()
        )
        .def_readwrite("title", &VstParameter::title, "")
        .def_readwrite("short_title", &VstParameter::shortTitle, "")
        .def_readwrite("default_normalized_value", &VstParameter::defaultNormalizedValue, "")
        .def_readwrite("id", &VstParameter::id, "")
        .def_readwrite("flags", &VstParameter::flags, "")
        .def_readwrite("step_count", &VstParameter::stepCount, "")
        .def_readwrite("unit_id", &VstParameter::unitId, "")
        .def_readwrite("units", &VstParameter::units, "")
        .def("can_automate",
            &VstParameter::canAutomate)
        .def("is_bypass",
            &VstParameter::isBypass)
        .def("is_hidden",
            &VstParameter::isHidden)
        .def("is_list",
            &VstParameter::isList)
        .def("is_program_change",
            &VstParameter::isProgramChange)
        .def("is_read_only",
            &VstParameter::isReadOnly)
        .def("is_wrap_around",
            &VstParameter::isWrapAround)
        ;
    ////////////////////    </generated_from:VstParameter.h>    ////////////////////

    // </litgen_pydef> // Autogenerated code end
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // to byass autogen
    pyClassVstHost.def("process_midi", [](Steinberg::Vst::VstHost& self, const std::vector<std::tuple<int, int, float, bool>> &notes, int numChannels, int numSamples) {
        // Create an AudioBufferWrapper for the output
        AudioBufferWrapper output(numChannels, numSamples);

        // Call the process function on the provided VstHost instance
        auto events = create_midi_events(notes);
        self.process(events, output.getBuffer(), numChannels, numSamples);

        // Return the output as a NumPy array
        return output.to_numpy();
    }, py::arg("events"), py::arg("numChannels"), py::arg("numSamples"));

    pyClassVstHost.def("process_audio", [](Steinberg::Vst::VstHost& self, py::array_t<float> input_array) {
        // Create an AudioBufferWrapper from the input NumPy array
        AudioBufferWrapper input_buffer(input_array);

        // Create an AudioBufferWrapper for the output
        AudioBufferWrapper output_buffer(input_buffer.getNumChannels(), input_buffer.getNumSamples());

        // Call the process function on the provided VstHost instance
        self.process(input_buffer.getBuffer(), output_buffer.getBuffer(), input_buffer.getNumChannels(), input_buffer.getNumSamples());

        // Return the output as a NumPy array
        return output_buffer.to_numpy();
    }, py::arg("input_array"));
}