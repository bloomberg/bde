include(bde_package)

macro(process)
    bde_process_package(${ARGN})
        # Do not use bde/cmake/defaults/bde_process_package.cmake to
        # avoid setting BSL_OVERRIDES_STD
endmacro()