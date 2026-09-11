"""Conan 2 recipe for PointCloudPipeline.

Third-party dependencies are pinned from ConanCenter. GoogleTest is the unit-test
framework (design section 5). spdlog is an optional logging backend. Open3D is
required only for the optional interactive visualization path (PCP_ENABLE_VIZ);
the default custom PPM renderer needs no third-party dependency.
"""

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, cmake_layout


class PointCloudPipelineConan(ConanFile):
    name = "pointcloudpipeline"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "with_viz": [True, False],
        "with_spdlog": [True, False],
    }
    default_options = {
        "with_viz": False,
        "with_spdlog": True,
    }

    def requirements(self):
        if self.options.with_spdlog:
            self.requires("spdlog/1.14.1")
        if self.options.with_viz:
            self.requires("open3d/0.18.0")

    def build_requirements(self):
        self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["PCP_ENABLE_VIZ"] = bool(self.options.with_viz)
        tc.variables["PCP_ENABLE_SPDLOG"] = bool(self.options.with_spdlog)
        tc.generate()
