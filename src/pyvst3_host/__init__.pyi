# type: ignore
import sys
from typing import Literal, List, Any, Optional, Tuple, Dict
import numpy as np
import enum
import numpy

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# <litgen_stub> // Autogenerated code below! Do not edit!
####################    <generated_from:PluginHost.hpp>    ####################




class VstHost:
    def __init__(self) -> None:
        pass

    def init(
        self,
        path: str,
        effect_id: VST3.Optional<VST3.UID> = VST3.Optional<VST3.UID>()
        ) -> bool:
        pass
    def terminate(self) -> None:
        pass
    def process_audio(self, input_file: str, output_file: str) -> None:
        pass

    def get_parameters(self) -> List[VstParameter]:
        pass




####################    </generated_from:PluginHost.hpp>    ####################


####################    <generated_from:VstParameter.h>    ####################

class VstParameter:
    title: str
    short_title: str
    default_normalized_value: float
    id: int
    flags: int
    step_count: int
    unit_id: int
    units: str

    # Helper functions to interpret flags
    def can_automate(self) -> bool:
        pass
    def is_bypass(self) -> bool:
        pass
    def is_hidden(self) -> bool:
        pass
    def is_list(self) -> bool:
        pass
    def is_program_change(self) -> bool:
        pass
    def is_read_only(self) -> bool:
        pass
    def is_wrap_around(self) -> bool:
        pass
    def __init__(
        self,
        title: str = "",
        short_title: str = "",
        default_normalized_value: float = float(),
        flags: int = int(),
        step_count: int = int(),
        unit_id: int = int(),
        units: str = ""
        ) -> None:
        """Auto-generated default constructor with named params"""
        pass


def convert_info_to_param(info: Steinberg.Vst.ParameterInfo) -> VstParameter:
    pass

####################    </generated_from:VstParameter.h>    ####################

# </litgen_stub> // Autogenerated code end!