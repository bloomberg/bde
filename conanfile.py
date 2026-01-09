# ***************************************************************
# This is an internal Bloomberg Conan recipe.                   *
# This recipe does not work outside of Bloomberg infrastructure *
# ***************************************************************

import re
import os
from conan import ConanFile

from conan.tools.cmake import CMake
from conan.tools.files import collect_libs

class Package(ConanFile):
    python_requires = "conan-dpkg-recipe/[>=0.19]@test/unstable"
    python_requires_extend = "conan-dpkg-recipe.BdeBuildSystem"

    def get_build_target(self) -> str | None:
        target = super().get_build_target()
        return target.replace('-', '_') if target else None

    def get_install_components(self) -> list[str] | None:
        target = super().get_build_target()
        return [target, f"{target}-headers"] if target else None
