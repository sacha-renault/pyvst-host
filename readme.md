# pyvst3host

pyvst3host is a Python module that allows hosting VST3 plugins and playing notes. The project leverages VST3 SDK capabilities to work with audio plugins, providing control over plugin parameters and managing audio processing states.

## Features

- Host and manage VST3 plugins in Python.
- Control plugin parameters programmatically.
- Primary made for offline processing.
- Save and load plugin presets.

## Installation

1 - Clone the repository with submodules

```sh
git clone --recurse-submodules https://github.com/sacha-renault/pyvst-host
```

2 - Build the Python module

```sh
python3 setup.py build_ext
```

## Usage

### Example

You can use VstHost to load, configure, and control a VST3 plugin.

```py
from src.pyvst3_host import VstHost, note

notes = [
    note(0,     # Channel
        "c4",   # Pict
        0.8,    # Velocity
        0,      # start (can be -1 for no start)
        20_000) # end (can be -1 for no end)
]

host = VstHost(samplerate=44_100, max_samples_per_block=512)
host.init("path/to/plugin.vst3")

n_channel = 2
n_samples = 44_100 # 1 second processing
output = host.process_midi(notes, n_channel, n_samples)
```

## Known Issues

Some VST plugins don't reset their internal buffers when processing goes from false to true, even though this is specified in the VST3 SDK documentation. According to the VST3 SDK [documentation](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical+Documentation/Workflow+Diagrams/Audio+Processor+Call+Sequence.html):

> The plug-in has to reset its inner processing state (for example, to clean its delay buffers in order to have a defined state when the processing starts again).

Unfortunately, this is a behavior issue with certain plugins, and it cannot be addressed within `pyvst3host` itself.
