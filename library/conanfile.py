from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps


class IpfsChromium(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    _PB = 'protobuf/3.21.9'
    requires = [
        'abseil/20230125.3',
        'boost/1.81.0',
        'gtest/1.13.0',
        'openssl/1.1.1t',
        _PB,
    ]
    default_options = {"boost/*:header_only": True}
    tool_requires = [
        'cmake/3.22.6',
        'doxygen/1.9.4',
        'ninja/1.11.1',
        _PB,
    ]


    def generate(self):
        tc = CMakeToolchain(self, 'Ninja')
        #tc.variables["FOO"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={
            "CXX_VERSION": 17, #TODO
            "INSIDE_CONAN": True
        })
        cmake.build()
# gperf  doxygen ccache lcov
