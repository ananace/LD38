find_package(SFML 2
    COMPONENTS system graphics window
    REQUIRED
)

set(SOURCES
    main.cpp
)

add_executable(Client
    ${SOURCES}
)
add_executable(LD38::Client ALIAS Client)

set_target_properties(Client PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)
target_include_directories(Client SYSTEM PRIVATE
    ${DEFAULT_SYSTEM_INCLUDE_DIRECTORIES}
)
target_include_directories(Client PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${SFML_INCLUDE_DIR}
)
target_link_libraries(Client PRIVATE
    ${DEFAULT_LIBRARIES}
    ${SFML_LIBRARIES}
    LD38::External::libyojimbo
    LD38::Common
)
target_compile_options(Client PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)
target_link_libraries(Client PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)

