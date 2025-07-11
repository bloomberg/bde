# This script is incomplete and is only meant as an aid for building
# and testing sljit in platforms without GNU make.
# You are better off install GNU make and using that instead.

cmake_minimum_required(VERSION 3.12)

project(sljit C)

# https://gist.github.com/tusharpm/d71dd6cab8a00320ddb48cc82bf7f64c

if(POLICY CMP0007)
  cmake_policy(SET CMP0007 NEW)
endif()

function(ReadVariables MKFile)
  file(READ "${MKFile}" FileContents)
  string(REPLACE "\\\n" "" FileContents ${FileContents})
  string(REPLACE "\n" ";" FileLines ${FileContents})
  list(REMOVE_ITEM FileLines "")
  foreach(line ${FileLines})
    if(line MATCHES "^[ A-Z]*=")
      string(REPLACE "=" ";" line_split ${line})
      list(GET line_split -1 value)
      string(STRIP "${value}" value)
      separate_arguments(value)
      list(REMOVE_AT line_split -1)
      foreach(var_name ${line_split})
        string(STRIP ${var_name} var_name)
        set(${var_name} ${value} PARENT_SCOPE)
      endforeach()
    endif()
  endforeach()
endfunction()

ReadVariables(GNUmakefile)

find_package(Threads REQUIRED)
include_directories(${SRCDIR} ${TESTDIR})
add_executable(sljit_test ${TESTDIR}/sljitMain.c ${TESTDIR}/sljitTest.c ${SRCDIR}/sljitLir.c)
target_compile_definitions(sljit_test PRIVATE SLJIT_HAVE_CONFIG_PRE)
target_link_libraries(sljit_test Threads::Threads)
if(MSVC)
  set_target_properties(sljit_test PROPERTIES LINK_FLAGS "/STACK:0x400000")
else()
  set_target_properties(sljit_test PROPERTIES LINK_FLAGS "-Wl,--stack,4194304")
endif()