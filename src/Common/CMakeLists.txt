set(SOURCES
    Network.cpp
    ScriptException.cpp
    ScriptManager.cpp

    scriptnetwork/scriptnetwork.cpp
)

source_group("Header files\\" REGULAR_EXPRESSION "[^./]+.(hpp|inl)")
source_group("Source files\\" REGULAR_EXPRESSION "[^./]+.cpp")
source_group("Header files\\Game" REGULAR_EXPRESSION "scriptnetwork/[^./]+.(hpp|inl)")
source_group("Source files\\Game" REGULAR_EXPRESSION "scriptnetwork/[^./]+.cpp")

add_library(Common STATIC
    ${SOURCES}
)
add_library(LD38::Common ALIAS Common)

set_target_properties(Common PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)
target_include_directories(Common SYSTEM PRIVATE
    ${DEFAULT_SYSTEM_INCLUDE_DIRECTORIES}
)
target_include_directories(Common PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
)
target_link_libraries(Common PRIVATE
    ${DEFAULT_LIBRARIES}
    LD38::External::libyojimbo
    LD38::External::as_addons
)
target_compile_options(Common PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)
target_link_libraries(Common PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
