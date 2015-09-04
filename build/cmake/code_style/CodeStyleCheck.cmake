# This file generates the target for the verification of the source code style 
#
#  Author: Raian Vargas Maretto <raian@dpi.inpe.br>
#          Thales Sehn Korting <tkorting@dpi.inpe.br>
#          Emiliano Ferreira Castejon <castejon@dpi.inpe.br>
#

include(CodeStyleConfiguration)

find_path(GEODMA_CPPLINT_DIR cpplint.py
    HINTS  ${GEODMA_ROOT_DIR}/tests/cpplint
)

# Add a target that runs cpplint.py
#
# Parameters:
# - TARGET_NAME the name of the target to add
# - SOURCES_LIST a complete list of source and include files to check
function(add_style_check_target TARGET_NAME SOURCES_LIST)
    
    list(REMOVE_DUPLICATES SOURCES_LIST)
    list(SORT SOURCES_LIST)

    # add custom command with PRE_BUILD option
    # builds only if there are no cpplint errors
    add_custom_command(TARGET ${TARGET_NAME}
        PRE_BUILD
        COMMAND "${CMAKE_COMMAND}" -E chdir
        "${CMAKE_SOURCE_DIR}"
        "${PYTHON_EXECUTABLE}"
        "${GEODMA_CPPLINT_DIR}/cpplint.py"
        # "--filter=${STYLE_FILTER}"
        "--counting=detailed"
        "--extensions=cpp,hpp,h"
        "--linelength=${MAX_LINE_LENGTH}"
        ${SOURCES_LIST}
        DEPENDS ${SOURCES_LIST}
        COMMENT "Checking code style."
        VERBATIM
    )
endfunction()