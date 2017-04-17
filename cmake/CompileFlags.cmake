string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

set(X64 OFF)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X64 ON)
endif()

SET(DEFAULT_PROJECT_OPTIONS
    DEBUG_POSTFIX             "d"
    CXX_STANDARD              11
    LINKER_LANGUAGE           "CXX"
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET     "hidden"
)

set(DEFAULT_SYSTEM_INCLUDE_DIRECTORIES
    "${PROJECT_PATH}/external"
)
set(DEFAULT_INCLUDE_DIRECTORIES
    "${PROJECT_PATH}/src/Common"
)
set(DEFAULT_LIBRARIES)
set(DEFAULT_COMPILE_DEFINITIONS
    SYSTEM_${SYSTEM_NAME}
)
set(DEFAULT_COMPILE_OPTIONS)
set(DEFAULT_LINKER_OPTIONS)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    list(APPEND DEFAULT_COMPILE_DEFINITIONS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_WARNINGS
    )

    list(APPEND DEFAULT_COMPILE_OPTIONS
        /MP     # Multiprocessor compilation
        /W4     # Warning level 4

        /wd4592 # 'sym': symbol will be dynamically initialized

        /EHsc   # Only exception handling on C++ code
        /FC     # Print full paths in diagnostics

        $<$<CONFIG:Release>:
            /Ox # Full optimization
            /Gw # Whole-program global data optimization
            /GL # Whole-program optimization
            /GF # String pooling
        >
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    list(APPEND DEFAULT_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wunused
        -Wpedantic

        -Wreorder
        -Wignored-qualifiers
        -Wmissing-braces
        -Wreturn-type
        -Wswitch
        -Wswitch-default
        -Wuninitialized
        -Wmissing-field-initializers
        -Woverloaded-virtual
        -Wold-style-cast
        -Wnon-virtual-dtor

        $<$<CXX_COMPILER_ID:GNU>:
            -Wmaybe-uninitialized
            -Wreturn-local-addr
        >
    )
    
    list(APPEND DEFAULT_LINKER_OPTIONS
        -pthread
    )
endif()

