from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy
from conan.tools.scm import Version
from conan.errors import ConanInvalidConfiguration
import sys
from os.path import dirname, isfile, join, realpath

here = realpath(dirname(__file__))
sys.path.insert(0, realpath(join(here, '..', 'es', 'bld')))
sys.path.insert(0, realpath(join(here, 'bld')))

try:
    import ic_version
    VERSION = ic_version.deduce()
except ImportError:
    VERSION = open(join(here, 'version.txt'), 'r').read().strip()


def protobuf_version():
    for d in [here, join(here, '..', 'es')]:
        path = join(d, 'bld', 'protobuf_version.txt')
        if isfile(path):
            with open(path) as f:
                return f.read().strip()
    raise Exception("can't find protobuf_version.txt")


class IpfsChromium(ConanFile):
    name = "ipfs_client"
    version = VERSION
    description = "Library for acting as a trustless client of IFPS gateway(s). see: https://specs.ipfs.tech/http-gateways/trustless-gateway/"
    homepage = "https://github.com/little-bear-labs/ipfs-chromium/tree/main/library"
    topics = ("ipfs", "ipns", "dweb", "web", "content-addressed", "network", "client", "io", "api", "file-sharing", "gateway", "kubo")
    license = "MIT,Apache-2.0,https://raw.githubusercontent.com/little-bear-labs/ipfs-chromium/main/library/LICENSE"
    settings = "os", "compiler", "build_type", "arch"
    require_transitively = [
        'abseil/20240116.2',
        'boost/1.81.0',
        'bzip2/1.0.8',
        'c-ares/1.22.1',
        'glog/0.7.1',
        'nlohmann_json/3.11.2',
        'openssl/1.1.1w',
        f'protobuf/{protobuf_version()}',
    ]
    options = {
        "testing": [True, False],
    }
    default_options = {
        "testing": False,
        "boost/*:bzip2": True,
        "boost/*:with_stacktrace_backtrace": True
    }
    tool_requires = [
        'cmake/3.22.6',
        'ninja/1.11.1',
        f'protobuf/{protobuf_version()}',
    ]
    extensions = ['h', 'cc', 'hpp', 'proto']
    exports_sources = ['*.cmake', '*.py', '*.sh', '*.txt', '*.in'] + [f'**/*.{e}' for e in extensions]
    exports = 'version.txt'
    package_type = 'static-library'

    def generate(self):
        tc = CMakeToolchain(self, 'Ninja')
        tc.generate()
        d = CMakeDeps(self)
        d.generate()

    def build(self):
        cmake = CMake(self)
        if self.settings.compiler.cppstd:
            stdver = self.settings.compiler.cppstd
        else:
            stdver = 20
        cmake.configure(variables={
            "CXX_VERSION": stdver,
            "INSIDE_CONAN": True
        })
        cmake.build(build_tool_args=['--verbose'])

    def package(self):
        s = join(self.source_folder, 'library')
        d = join(self.package_folder, "licenses")
        copy(self, pattern="LICENSE*", dst=d, src=s)
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["ipfs_client", "ic_proto"]

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        for lib in self.require_transitively:
            self.requires(lib, transitive_headers=True)
        if self.options.testing:
            self.requires('gtest/1.15.0')

    @property
    def _min_cppstd(self):
        return "20"

    @property
    def _minimum_compilers_version(self):
        return {
            "apple-clang": "14",
            "gcc": "11",
            "msvc": "193"
        }

    def validate(self):
        cc_nm = str(self.settings.compiler)
        if self.settings.compiler.cppstd:
            check_min_cppstd(self, 20)
        minimum_version = self._minimum_compilers_version.get(cc_nm, False)
        if minimum_version and Version(self.settings.compiler.version) < minimum_version:
            raise ConanInvalidConfiguration(f"{self.ref} does not support your compiler.")
