include(bde_package)

macro(process_package)
    bde_process_package(${ARGN})
        # Call bde_process_package directly to avoid setting BSL_OVERRIDES_STD
endmacro()