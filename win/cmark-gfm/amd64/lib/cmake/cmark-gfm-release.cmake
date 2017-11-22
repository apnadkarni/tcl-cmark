#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cmark-gfm" for configuration "Release"
set_property(TARGET cmark-gfm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(cmark-gfm PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/cmark-gfm.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS cmark-gfm )
list(APPEND _IMPORT_CHECK_FILES_FOR_cmark-gfm "${_IMPORT_PREFIX}/bin/cmark-gfm.exe" )

# Import target "libcmark-gfm" for configuration "Release"
set_property(TARGET libcmark-gfm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libcmark-gfm PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/cmark-gfm.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/cmark-gfm.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libcmark-gfm )
list(APPEND _IMPORT_CHECK_FILES_FOR_libcmark-gfm "${_IMPORT_PREFIX}/lib/cmark-gfm.lib" "${_IMPORT_PREFIX}/bin/cmark-gfm.dll" )

# Import target "libcmark-gfm_static" for configuration "Release"
set_property(TARGET libcmark-gfm_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libcmark-gfm_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/cmark-gfm_static.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libcmark-gfm_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_libcmark-gfm_static "${_IMPORT_PREFIX}/lib/cmark-gfm_static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
