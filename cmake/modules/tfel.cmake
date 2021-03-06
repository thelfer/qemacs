# find the tfel library
if(TFEL_INSTALL_PATH)
  set(TFELHOME "${TFEL_INSTALL_PATH}")
else(TFEL_INSTALL_PATH)
  set(TFELHOME $ENV{TFELHOME})
endif(TFEL_INSTALL_PATH)

if(LIB_SUFFIX)
  add_definitions("-DLIB_SUFFIX=\\\"\"${LIB_SUFFIX}\"\\\"")
endif(LIB_SUFFIX)

# type of architecture
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  add_definitions("-DTFEL_ARCH64")
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  add_definitions("-DTFEL_ARCH32")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

find_program(MFRONT       mfront
  HINTS "${TFELHOME}/bin")
find_program(TFEL_CONFIG  tfel-config
  HINTS "${TFELHOME}/bin")

IF(NOT (TFEL_CONFIG AND MFRONT))
  MESSAGE(FATAL_ERROR "tfel not found")
ENDIF(NOT (TFEL_CONFIG AND MFRONT))

EXECUTE_PROCESS(COMMAND ${TFEL_CONFIG} "--includes"
  OUTPUT_VARIABLE TFEL_INCLUDE_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
STRING(LENGTH ${TFEL_INCLUDE_PATH}  TFEL_INCLUDE_PATH_LENGTH)
MATH(EXPR TFEL_INCLUDE_PATH_LENGTH "${TFEL_INCLUDE_PATH_LENGTH} - 2")
STRING(SUBSTRING ${TFEL_INCLUDE_PATH} 2 ${TFEL_INCLUDE_PATH_LENGTH} TFEL_INCLUDE_PATH)
EXECUTE_PROCESS(COMMAND ${TFEL_CONFIG} "--libs"
  OUTPUT_VARIABLE TFEL_LIBRARY_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
STRING(LENGTH ${TFEL_LIBRARY_PATH}  TFEL_LIBRARY_PATH_LENGTH)
MATH(EXPR TFEL_LIBRARY_PATH_LENGTH "${TFEL_LIBRARY_PATH_LENGTH} - 2")
STRING(SUBSTRING ${TFEL_LIBRARY_PATH} 2 ${TFEL_LIBRARY_PATH_LENGTH} TFEL_LIBRARY_PATH)

macro(find_tfel_library name)
  find_library(${name}
    NAMES ${name}
    HINTS ${TFEL_LIBRARY_PATH})
  if(NOT ${name})
    MESSAGE(FATAL_ERROR "${name} library not found")
  endif(NOT ${name})
endmacro(find_tfel_library name)

find_tfel_library(TFELTests)
find_tfel_library(TFELException)
find_tfel_library(TFELUtilities)
find_tfel_library(TFELMath)
find_tfel_library(TFELMaterial)

# list of available behaviour interfaces
EXECUTE_PROCESS(COMMAND ${MFRONT} "--list-behaviour-interfaces"
  OUTPUT_VARIABLE MFRONT_BEHAVIOUR_INTERFACES_TMP
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCHALL "[a-zA-Z]+"
       MFRONT_BEHAVIOUR_INTERFACES ${MFRONT_BEHAVIOUR_INTERFACES_TMP})

# check for the Abaqus/Standard interface
list (FIND MFRONT_BEHAVIOUR_INTERFACES "abaqus" abaqus_interface_index)
if (${abaqus_interface_index} GREATER -1)
  add_definitions("-DTFEL_HAS_ABAQUS_INTERFACE_SUPPORT")
endif()

# check for the Abaqus/Explicit interface
list (FIND MFRONT_BEHAVIOUR_INTERFACES "abaqusexplicit" abaqus_explicit_interface_index)
if (${abaqus_explicit_interface_index} GREATER -1)
  add_definitions("-DTFEL_HAS_ABAQUS_EXPLICIT_INTERFACE_SUPPORT")
endif()

# check for the Ansys interface
list (FIND MFRONT_BEHAVIOUR_INTERFACES "ansys" ansys_interface_index)
if (${ansys_interface_index} GREATER -1)
  add_definitions("-DTFEL_HAS_ANSYS_INTERFACE_SUPPORT")
endif()

MESSAGE(STATUS "mfront        : ${MFRONT}")
MESSAGE(STATUS "tfel-config   : ${TFEL_CONFIG}")
MESSAGE(STATUS "tfel include  : ${TFEL_INCLUDE_PATH}")
MESSAGE(STATUS "tfel libs     : ${TFEL_LIBRARY_PATH}")
MESSAGE(STATUS "TFELTests     : ${TFELTests}")
MESSAGE(STATUS "TFELTests     : ${TFELTests}")
MESSAGE(STATUS "TFELException : ${TFELException}")
MESSAGE(STATUS "TFELUtilities : ${TFELUtilities}")
MESSAGE(STATUS "TFELMath      : ${TFELMath}")	
MESSAGE(STATUS "TFELMaterial  : ${TFELMaterial}") 
SET(HAVE_TFEL ON)
