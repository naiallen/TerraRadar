# This file contains the definitions of the source code rules (cpplint flags)
#
#  Author: Raian Vargas Maretto <raian@dpi.inpe.br>
#          Thales Sehn Korting <tkorting@dpi.inpe.br>
#          Emiliano Ferreira Castejon <castejon@dpi.inpe.br>
#

# Maximum lengh allowed for each line of the source code
set(MAX_LINE_LENGTH 100)

set(STYLE_FILTER)

# disable unwanted filters
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/braces,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/semicolon,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/blank_line,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/comma,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/operators,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/parens,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/indent,)
# set(STYLE_FILTER ${STYLE_FILTER}+whitespace/comments,)
# set(STYLE_FILTER ${STYLE_FILTER}+whitespace/newline,)
# set(STYLE_FILTER ${STYLE_FILTER}-whitespace/tab,)

# set(STYLE_FILTER ${STYLE_FILTER}-build/include_order,)
# set(STYLE_FILTER ${STYLE_FILTER}-build/namespaces,)
# set(STYLE_FILTER ${STYLE_FILTER}-build/include_what_you_use,)

# set(STYLE_FILTER ${STYLE_FILTER}-readability/streams,)
# set(STYLE_FILTER ${STYLE_FILTER}-readability/todo,)

# set(STYLE_FILTER ${STYLE_FILTER}-runtime/references,)
# set(STYLE_FILTER ${STYLE_FILTER}-runtime/int,)
# set(STYLE_FILTER ${STYLE_FILTER}-runtime/explicit,)
# set(STYLE_FILTER ${STYLE_FILTER}-runtime/printf,)