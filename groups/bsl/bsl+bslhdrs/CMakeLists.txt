set(pkg "bsl+bslhdrs")

bbs_setup_header_only_pkg(${pkg})

# Add include files to the parent group
list(APPEND bsl_INCLUDE_FILES ${${pkg}_INCLUDE_FILES})
set(bsl_INCLUDE_FILES  ${bsl_INCLUDE_FILES}  PARENT_SCOPE)
