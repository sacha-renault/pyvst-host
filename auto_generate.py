import litgen
import glob
import os


def my_litgen_options() -> litgen.LitgenOptions:
    # configure your options here
    options = litgen.LitgenOptions()

    # //////////////////////////////////////////////////////////////////
    # Basic functions bindings
    # ////////////////////////////////////////////////////////////////////
    # No specific option is needed for these basic bindings
    # litgen will add the docstrings automatically in the python bindings

    # //////////////////////////////////////////////////////////////////
    # Classes and structs bindings
    # //////////////////////////////////////////////////////////////////
    # No specific option is needed for these bindings.
    # - Litgen will automatically add a default constructor with named parameters
    #   for structs that have no constructor defined in C++.
    #  - A class will publish only its public methods and members

    # ///////////////////////////////////////////////////////////////////
    #  Exclude functions and/or parameters from the bindings
    # ///////////////////////////////////////////////////////////////////
    # We want to exclude `inline void priv_SetOptions(bool v) {}` from the bindings
    # priv_ is a prefix for private functions that we don't want to expose
    options.fn_exclude_by_name__regex = "^priv_"

    # Inside `inline void SetOptions(bool v, bool priv_param = false) {}`,
    # we don't want to expose the private parameter priv_param
    # (it is possible since it has a default value)
    options.fn_params_exclude_names__regex = "^priv_"

    # ////////////////////////////////////////////////////////////////////
    # Override virtual methods in python
    # ////////////////////////////////////////////////////////////////////
    # The virtual methods of this class can be overriden in python
    options.class_override_virtual_methods_in_python__regex = "^Animal$"

    # ////////////////////////////////////////////////////////////////////
    #  Published vectorized math functions and namespaces
    # ////////////////////////////////////////////////////////////////////
    # The functions in the MathFunctions namespace will be also published as vectorized functions
    # options.fn_namespace_vectorize__regex = r"^DaftLib::MathFunctions$"  # Do it in this namespace only
    # options.fn_vectorize__regex = r".*"  # For all functions

    # ////////////////////////////////////////////////////////////////////
    # Format the python stubs with black
    # ////////////////////////////////////////////////////////////////////
    # Set to True if you want the stub file to be formatted with black
    # options.python_run_black_formatter = True

    # To bypass namespaces
    options.namespaces_root = ["Steinberg", "Vst"]

    return options


def autogenerate() -> None:
    repository_dir = os.path.realpath(os.path.dirname(__file__))
    output_dir = repository_dir + "/src"

    include_dir = repository_dir + "/src/cpp_host/"

    header_files = []
    for root, _, files in os.walk(include_dir):
        for file in files:
            if file.endswith(".hpp") or file.endswith(".h"):
                header_files.append(os.path.join(root, file))


    litgen.write_generated_code_for_files(
        options=my_litgen_options(),
        input_cpp_header_files=header_files,
        output_cpp_pydef_file=output_dir + "/Binding.cpp",
        output_stub_pyi_file=output_dir + "/pyvst3_host/__init__.pyi",
    )


if __name__ == "__main__":
    autogenerate()