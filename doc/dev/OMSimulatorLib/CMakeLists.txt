

## Set the relevant doxygen options. Refer to the Doxygen file in this directory for
## more information about these options. Note that the Doxygen file is not used by this.
## It is just left there documentation purposes right now.

set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)

set(DOXYGEN_PROJECT_NAME OMSimulatorLib)
set(DOXYGEN_PROJECT_BRIEF "The OMSimulator project is a FMI-based co-simulation environment that supports ordinary (i.e., non-delayed) and TLM connections.")

set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_EXTRACT_PACKAGE YES)
set(DOXYGEN_EXTRACT_STATIC YES)
set(DOXYGEN_EXTRACT_RECURSIVE YES)

set(DOXYGEN_TEMPLATE_RELATIONS YES)
set(DOXYGEN_CALL_GRAPH YES)
set(DOXYGEN_CALLER_GRAPH YES)

set(DOXYGEN_DOT_IMAGE_FORMAT svg)
set(DOXYGEN_INTERACTIVE_SVG YES)


set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md)

doxygen_add_docs(doc-doxygen
                ${PROJECT_SOURCE_DIR}/src/OMSimulatorLib/ ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.md
                COMMENT "Generating doxygen documentation."
)

# add_custom_target(doc-doxygen
#                   COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
