from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools import files
import os   

required_conan_version = ">=1.45.0"

class GenH5Conan(ConanFile):
    name = "genh5"
    version = "3.0.0"
    license = "GTlab license"
    url = "https://gitlab.dlr.de/at-twk/gtlab-hdf5-wrapper"
    homepage = "https://gitlab.dlr.de/at-twk/gtlab-hdf5-wrapper"
    topics = "hdf5"
    description = ("Wrapper for the HDF5 C++ API. Provides a higher level interface for working with HDF5 files and objects. " 
                   "It implements a generic, template based data serialization system, simplifying read/write operations for "
                   "strings, varlen and array data - as well as custom data classes.")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "build": ["stable", "unstable"]}
    default_options = {"shared": True, "fPIC": True, "build": "unstable"}
    exports_sources = "src/*", "README.md"

    def generate(self):
        CMakeToolchain(self).generate()
        CMakeDeps(self).generate()
  
  
    def requirements(self):
        self.requires("hdf5/1.12.1")

        if self.options.build  == "stable":
            self.requires("qt/5.12.5@dlr-at/stable")
        else:
            self.requires("qt/5.15.2")
            self.requires("zlib/1.2.13", override=True)


    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        # linking a static library into a shared genh5 makes the tests fail
        self.options["hdf5"].shared = self.options.shared
        self.options["qt"].shared = True

    def build(self):    
        cmake = CMake(self)
        cmake.configure(build_script_folder="src")
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

        files.rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        files.copy(self, "README.md", dst=os.path.join(self.package_folder, "licenses"), src=self.source_folder)

    def package_info(self):      
        self.cpp_info.includedirs.append(os.path.join("include", "h5"))


        self.cpp_info.libdirs = ['lib', 'lib/h5']
        
        if self.settings.build_type != "Debug":
            self.cpp_info.libs = ['GenH5']
        else:
            self.cpp_info.libs = ['GenH5-d']

        if not self.options.shared:
            self.cpp_info.defines.append("GENH5_STATIC")

        self.cpp_info.set_property("cmake_file_name", "GenH5")
        self.cpp_info.set_property("cmake_target_name", "GenH5")
