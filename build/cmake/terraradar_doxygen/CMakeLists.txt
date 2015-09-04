#  Script for generating the documentation with Doxygen.
#
#  Author: Etore Marcari Jr. <etore@dpi.inpe.br>
#          Raian Vargas Maretto <raian@dpi.inpe.br>
#

configure_file(${TERRARADAR_DOC_DIR}/api/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(terraradar_doxygen
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with Doxygen" VERBATIM
)

install(CODE "execute_process(COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
        DESTINATION share/terraradar COMPONENT devel)