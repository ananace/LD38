set(SOURCES
    Messages.hpp
)

add_library(Common STATIC
    ${SOURCES}
)
add_library(LD38::Common ALIAS Common)

set_target_properties(Common PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)
target_include_directories(Common PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
)
target_link_libraries(Common PRIVATE
    ${DEFAULT_LIBRARIES}
    LD38::External::libyojimbo
)
target_compile_options(Common PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)
target_link_libraries(Common PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
