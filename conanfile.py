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
