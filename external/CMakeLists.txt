set(IDE_FOLDER "External")

set(ASADDON_SOURCES
    as_addons/contextmgr/contextmgr.cpp
    as_addons/datetime/datetime.cpp
    as_addons/debugger/debugger.cpp
    as_addons/scriptany/scriptany.cpp
    as_addons/scriptarray/scriptarray.cpp
    as_addons/scriptbuilder/scriptbuilder.cpp
    as_addons/scriptdictionary/scriptdictionary.cpp
    as_addons/scriptgrid/scriptgrid.cpp
    as_addons/scripthandle/scripthandle.cpp
    as_addons/scripthelper/scripthelper.cpp
    as_addons/scriptmath/scriptmath.cpp
    as_addons/scriptmath/scriptmathcomplex.cpp
    as_addons/scriptstdstring/scriptstdstring.cpp
    as_addons/scriptstdstring/scriptstdstring_utils.cpp
    as_addons/serializer/serializer.cpp
    as_addons/weakref/weakref.cpp
)

add_library(as_addons STATIC
    ${ASADDON_SOURCES}
)
add_library(LD38::External::as_addons ALIAS as_addons)

set_target_properties(as_addons PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)
target_include_directories(as_addons PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${Angelscript_INCLUDE_DIRS}
)
target_link_libraries(as_addons PRIVATE
    ${DEFAULT_LIBRARIES}
    ${Angelscript_LIBRARIES}
)
target_link_libraries(as_addons PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)

set(YOJIMBO_SOURCES
    libyojimbo/yojimbo.cpp
    libyojimbo/yojimbo_address.cpp
    libyojimbo/yojimbo_allocator.cpp
    libyojimbo/yojimbo_channel.cpp
    libyojimbo/yojimbo_client.cpp
    libyojimbo/yojimbo_common.cpp
    libyojimbo/yojimbo_connection.cpp
    libyojimbo/yojimbo_encryption.cpp
    libyojimbo/yojimbo_matcher.cpp
    libyojimbo/yojimbo_network.cpp
    libyojimbo/yojimbo_network_simulator.cpp
    libyojimbo/yojimbo_packet.cpp
    libyojimbo/yojimbo_packet_processor.cpp
    libyojimbo/yojimbo_platform.cpp
    libyojimbo/yojimbo_server.cpp
    libyojimbo/yojimbo_sockets.cpp
    libyojimbo/yojimbo_tokens.cpp
    libyojimbo/yojimbo_transport.cpp
)

add_library(libyojimbo STATIC
    ${YOJIMBO_SOURCES}
)
add_library(LD38::External::libyojimbo ALIAS libyojimbo)

set_target_properties(libyojimbo PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)
target_include_directories(libyojimbo PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    $<$<PLATFORM_ID:Windows>:
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows"
    >
)
target_link_libraries(libyojimbo PRIVATE
    ${DEFAULT_LIBRARIES}
    $<$<PLATFORM_ID:Windows>:
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/mbedcrypto-release.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/mbedtls-release.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/mbedx509-release.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/sodium-release.lib"
    >
    $<$<CONFIG:Debug>:
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/mbedcrypto-debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/mbedtls-debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/mbedx509-debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/libyojimbo/windows/sodium-debug.lib"
    >
    >
)
target_compile_options(libyojimbo PRIVATE
    $<$<PLATFORM_ID:Windows>:
        /MP
        /FC
        $<$<CONFIG:Release>:
            /Ox
            /Gw
            /GL
            /GF
        >
    >
#   ${DEFAULT_COMPILE_OPTIONS}
)
target_link_libraries(libyojimbo PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)
