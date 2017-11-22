#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libcmark-gfmextensions" for configuration "Release"
set_property(TARGET libcmark-gfmextensions APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libcmark-gfmextensions PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/cmark-gfmextensions.lib"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "libcmark-gfm"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/cmark-gfmextensions.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS libcmark-gfmextensions )
list(APPEND _IMPORT_CHECK_FILES_FOR_libcmark-gfmextensions "${_IMPORT_PREFIX}/lib/cmark-gfmextensions.lib" "${_IMPORT_PREFIX}/bin/cmark-gfmextensions.dll" )

# Import target "libcmark-gfmextensions_static" for configuration "Release"
set_property(TARGET libcmark-gfmextensions_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libcmark-gfmextensions_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/cmark-gfmextensions_static.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libcmark-gfmextensions_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_libcmark-gfmextensions_static "${_IMPORT_PREFIX}/lib/cmark-gfmextensions_static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
