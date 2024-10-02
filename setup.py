import os
import sys
import setuptools
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import pybind11

# Define the C++ extension
ext_modules = [
    Extension(
        'pyvst3_host',  # Name of the generated Python module
        sources=[
            'src/cpp_host/PluginHost.cpp',
            'src/cpp_host/includes.cpp',
            'src/cpp_host/usediids.cpp',
            'src/Binding.cpp'
        ],
        include_dirs=[
            pybind11.get_include(),  # Pybind11 include directory
            "src/cpp_host",  # Directory with PluginHost.hpp and other headers
            "./public.sdk/source",
            "./"
        ],
        language='c++',
        extra_compile_args=[
            '-std=c++17',              # Use C++17 standard
            '-D_DEBUG',                # Define debug flag
            '-fdiagnostics-color=always',  # Enable colored diagnostics
            '-g',                       # Include debug symbols
            "-DPYBIND11_DETAILED_ERROR_MESSAGES"
        ],
        extra_link_args=[
            '-ldl',                    # Link against dynamic linking library
            '-lstdc++fs'               # Link against the filesystem library
        ],
    ),
]

class CustomBuildExt(build_ext):
    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = []

        # Platform-specific compiler options
        if sys.platform == 'darwin':
            opts.append('-stdlib=libc++')
        elif sys.platform.startswith('win'):
            opts.append('/DVERSION_INFO=\\"{}\\"'.format(self.distribution.get_version()))
        else:
            opts.append('-DVERSION_INFO="{}"'.format(self.distribution.get_version()))
            opts.append('-fvisibility=hidden')

        # Add platform-specific options to each extension's compile arguments
        for ext in self.extensions:
            ext.extra_compile_args = opts + ext.extra_compile_args

        # Call the original build_ext method to compile the extensions
        build_ext.build_extensions(self)

        # Move the built .so files to the target directory
        target_dir = os.path.abspath("src/pyvst3_host")  # Custom output directory
        os.makedirs(target_dir, exist_ok=True)

        # Move each extension to the custom output directory
        for ext in self.extensions:
            # Get the filename of the built extension
            filename = self.get_ext_filename(ext.name)
            build_lib = os.path.abspath(self.build_lib)
            original_path = os.path.join(build_lib, filename)

            # Define the target path for the .so file
            target_path = os.path.join(target_dir, filename)

            # Move the .so file to the target directory
            self.copy_file(original_path, target_path)

if __name__ == "__main__":
    # run code auto gen
    print("Run auto gen")
    subprocess.run([
        sys.executable, "auto_generate.py"
    ])

    # Define the setup configuration
    setup(
        name='pyvst3_host',
        version='0.1.0',
        author='Sacha',
        author_email='sacha@example.com',
        description='Python bindings for VST host plugin interface using pybind11',
        long_description=open('readme.md').read(),
        long_description_content_type='text/markdown',
        ext_modules=ext_modules,
        cmdclass={'build_ext': CustomBuildExt},
        zip_safe=False,
        setup_requires=['pybind11>=2.5.0'],
        install_requires=['pybind11>=2.5.0'],
    )
