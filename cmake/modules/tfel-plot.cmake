# find the tfel library
if(TFELPLOT_INSTALL_PATH)
  set(TFELPLOTHOME "${TFELPLOT_INSTALL_PATH}")
else(TFELPLOT_INSTALL_PATH)
  set(TFELPLOTHOME $ENV{TFELPLOTHOME})
endif(TFELPLOT_INSTALL_PATH)

find_program(TFELPLOT_CONFIG  tfel-plot-config
  HINTS "${TFELPLOTHOME}/bin")

IF(NOT (TFELPLOT_CONFIG))
  MESSAGE(FATAL_ERROR "tfel-plot not found")
ENDIF(NOT (TFELPLOT_CONFIG))

EXECUTE_PROCESS(COMMAND ${TFELPLOT_CONFIG} "--includes"
  OUTPUT_VARIABLE TFELPLOT_INCLUDE_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
STRING(LENGTH ${TFELPLOT_INCLUDE_PATH}  TFELPLOT_INCLUDE_PATH_LENGTH)
MATH(EXPR TFELPLOT_INCLUDE_PATH_LENGTH "${TFELPLOT_INCLUDE_PATH_LENGTH} - 2")
STRING(SUBSTRING ${TFELPLOT_INCLUDE_PATH} 2 ${TFELPLOT_INCLUDE_PATH_LENGTH} TFELPLOT_INCLUDE_PATH)
EXECUTE_PROCESS(COMMAND ${TFELPLOT_CONFIG} "--libs"
  OUTPUT_VARIABLE TFELPLOT_LIBRARY_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
STRING(LENGTH ${TFELPLOT_LIBRARY_PATH}  TFELPLOT_LIBRARY_PATH_LENGTH)
MATH(EXPR TFELPLOT_LIBRARY_PATH_LENGTH "${TFELPLOT_LIBRARY_PATH_LENGTH} - 2")
STRING(SUBSTRING ${TFELPLOT_LIBRARY_PATH} 2 ${TFELPLOT_LIBRARY_PATH_LENGTH} TFELPLOT_LIBRARY_PATH)

find_library(TFELPlot
  NAMES TFELPlot
  HINTS ${TFELPLOT_LIBRARY_PATH})
if(NOT TFELPlot)
  MESSAGE(FATAL_ERROR "TFELPlot library not found")
endif(NOT TFELPlot)

find_tfel_library(TFELPlot)

MESSAGE(STATUS "tfel-plot-config  : ${TFELPLOT_CONFIG}")
MESSAGE(STATUS "tfel-plot include : ${TFELPLOT_INCLUDE_PATH}")
MESSAGE(STATUS "tfel-plot libs    : ${TFELPLOT_LIBRARY_PATH}")

SET(HAVE_TFELPLOT ON)