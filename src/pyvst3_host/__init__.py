from typing import NamedTuple
import string
from .pyvst3_host import *

__notes = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
__midi_note_mapping = {k: i for i, k in enumerate(__notes)}

def _string_note_to_pitch(note: str) -> int:
    if len(note) < 2 or len(note) > 3:
        raise ValueError("Note must be of format like 'C4' or 'C#4'.")

    # Extract relative pitch and octave level
    if len(note) == 3:
        rpitch = note[:2]  # Handles notes like "C#"
        octave = note[2]
    else:
        rpitch = note[0]   # Handles notes like "C"
        octave = note[1]

    if rpitch not in __notes:
        raise ValueError(f"{rpitch} is not in {__notes}")

    if not octave.isdigit():
        raise ValueError("second element of note must be an integer")

    pitch = (int(octave) + 1) * 12 + __midi_note_mapping[rpitch]
    return pitch

def note(channel: int, pitch_or_note: int | str, velocity: float = 0.8, on_offset: int = -1, off_offset: int = -1) -> tuple:
    if isinstance(pitch_or_note, str):
        pitch = _string_note_to_pitch(pitch_or_note)
    elif isinstance(pitch_or_note, int):
        pitch = pitch_or_note
    else:
        raise TypeError("pitch_or_note must be int | str")

    return channel, pitch, velocity, on_offset, off_offset