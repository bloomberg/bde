add_subdirectory("inteldfp/LIBRARY")

# This flags is for use in vcpkg build where pcre2 library is provided
# in a separate package.
if (NOT BDE_USE_EXTERNAL_PCRE2)
    add_subdirectory(pcre2)
endif()

add_subdirectory(bbryu)

