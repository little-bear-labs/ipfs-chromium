from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from shutil import copyfile, which
import sys
from os.path import dirname, isfile, join, realpath

here = realpath(dirname(__file__))
sys.path.append(realpath(join(here, '..', 'cmake')))
sys.path.append(here)

try:
    import version
    VERSION = version.deduce()
except ImportError:
    VERSION = open(join(here,'version.txt'), 'r').read().strip()


class IpfsChromium(ConanFile):
    name = "ipfs_client"
    version = VERSION
    settings = "os", "compiler", "build_type", "arch"
    # generators = "CMakeDeps", 'CMakeToolchain'
    _PB = 'protobuf/3.20.0'
    require_transitively = [
        'abseil/20230125.3',
        'boost/1.81.0',
        'bzip2/1.0.8',
        'c-ares/1.22.1',
        'nlohmann_json/3.11.2',
        'openssl/1.1.1t',
        _PB,
    ]
    # default_options = {"boost/*:header_only": True}
    default_options = {
        "boost/*:bzip2": True,
        "boost/*:with_stacktrace_backtrace": True
    }
    tool_requires = [
        'cmake/3.22.6',
        'ninja/1.11.1',
        _PB,
    ]
    extensions = ['h', 'cc', 'hpp', 'proto']
    exports_sources = [ '*.txt' ] + [f'**/*.{e}' for e in extensions]
    exports = 'version.txt'
    package_type = 'static-library'


    def generate(self):
        tc = CMakeToolchain(self, 'Ninja')
        tc.generate()
        d = CMakeDeps(self)
        d.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={
            "CXX_VERSION": 20,
            "INSIDE_CONAN": True
        })
        cmake.build(build_tool_args=['--verbose'])

    def package(self):
        cmake = CMake(self)
        cmake.install()
        print(self.cpp_info.objects)

    def package_info(self):
        self.cpp_info.libs = ["ipfs_client"]

    # def build_requirements(self):
    #     if not which("doxygen"):
    #         self.tool_requires("doxygen/1.9.4")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        for l in self.require_transitively:
            self.requires(l, transitive_headers=True)
