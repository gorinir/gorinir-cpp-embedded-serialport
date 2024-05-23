cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(COMPILER "None" CACHE STRING "...")
set(USE_CONAN "FALSE" CACHE BOOL "...")
set(USE_CUBE "FALSE" CACHE BOOL "...")
set(SKIP_TEST "TRUE" CACHE BOOL "...")
set(TEST_GROUP "all" CACHE STRING "...")
set(TEST_CASE "all" CACHE STRING "...")

block()
    set(fileName "util.cmake")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/${fileName}")
        file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}")
        file(DOWNLOAD
            "https://raw.githubusercontent.com/exqudens/exqudens-cmake/1.0.1/src/main/cmake/${fileName}"
            "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}"
            EXPECTED_MD5 "d7dd546adf3830d9eb6cca7b0dda7d4e"
            STATUS downloadStatus
        )
        if(NOT "${downloadStatus}" STREQUAL "0;\"No error\"")
            message(FATAL_ERROR "downloadStatus: '${downloadStatus}'")
        endif()
        file(COPY "${CMAKE_CURRENT_BINARY_DIR}/tmp/${fileName}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../common")
    endif()
endblock()

include("${CMAKE_CURRENT_BINARY_DIR}/../common/util.cmake")

string(REPLACE "." ";" PRESET_NAME_ELEMENTS "${PRESET_NAME}")
if("msvc-16-x64-x64" IN_LIST "PRESET_NAME_ELEMENTS")
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/msvc-16-x64-x64-toolchain.cmake")
        set_msvc_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            VERSION "16"
            HOST "x64"
            TARGET "x64"
            #NO_CACHE "FALSE"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../common/msvc-16-x64-x64-toolchain.cmake"
        )
    endif()
    if("${USE_CONAN}")
        find_program(CONAN_COMMAND NAMES "conan.exe" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
elseif("gcc" IN_LIST PRESET_NAME_ELEMENTS)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/gcc-toolchain.cmake")
        find_file(COMPILER_FILE NAMES "gcc" PATHS ENV GCC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
        set_gnu_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            PATH "${COMPILER_FILE}"
            #NO_CACHE "FALSE"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../common/gcc-toolchain.cmake"
        )
    endif()
    if("${USE_CONAN}")
        find_program(CONAN_COMMAND NAMES "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
elseif("gcc-arm" IN_LIST PRESET_NAME_ELEMENTS)
    if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../common/gcc-arm-toolchain.cmake")
        find_file(COMPILER_FILE NAMES "arm-none-eabi-gcc.exe" PATHS ENV GCC_ARM_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
        set_gnu_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "armv7"
            OS "Generic"
            PATH "${COMPILER_FILE}"
            OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/../common/gcc-arm-toolchain.cmake"
        )
    endif()
    if("${USE_CONAN}")
        find_program(CONAN_COMMAND NAMES "conan.exe" "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
    find_program(STM_CUBE_FILE NAMES "STM32CubeMX.exe" "STM32CubeMX" PATHS ENV STM_CUBE_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    find_program(STM_CUBE_JAVA NAMES "java.exe" "java" PATHS "${STM_CUBE_FILE}/../jre/bin" REQUIRED NO_CACHE NO_DEFAULT_PATH)
    find_program(OPENOCD_COMMAND NAMES "openocd.exe" "openocd" PATHS ENV OPEN_OCD_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/name-version.txt" INFO)
string(REPLACE ":" ";" INFO_LIST "${INFO}")
list(GET INFO_LIST 0 RAW_NAME)
list(GET INFO_LIST 1 RAW_VERSION)
string(STRIP "${RAW_NAME}" NAME)
string(STRIP "${RAW_VERSION}" VERSION)
project("${NAME}" VERSION "${VERSION}" LANGUAGES "NONE")

message(STATUS "project: '${PROJECT_NAME}-${PROJECT_VERSION}'")

string(REPLACE "-cpp-" "-" BASE_DIR "${PROJECT_NAME}")
string(REPLACE "-" "/" BASE_DIR "${BASE_DIR}")

message(STATUS "BASE_DIR: '${BASE_DIR}'")

string(REPLACE "/" "_" BASE_NAME "${BASE_DIR}")
string(TOUPPER "${BASE_NAME}" BASE_NAME)

message(STATUS "BASE_NAME: '${BASE_NAME}'")

if("$ENV{CLION_IDE}")
    cmake_path(CONVERT "$ENV{PATH}" TO_CMAKE_PATH_LIST TMP_CMAKE_ENV_PATH NORMALIZE)
    list(REMOVE_AT TMP_CMAKE_ENV_PATH 0)
    cmake_path(CONVERT "${TMP_CMAKE_ENV_PATH}" TO_NATIVE_PATH_LIST TMP_NATIVE_ENV_PATH NORMALIZE)
    set("ENV{PATH}" "${TMP_NATIVE_ENV_PATH}")
endif()

enable_language("C")
enable_language("CXX")
if("gcc-arm" IN_LIST PRESET_NAME_ELEMENTS)
    enable_language("ASM")
endif()
include("GenerateExportHeader")
enable_testing()
include("GoogleTest")

if("${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/cmake-install" CACHE PATH "..." FORCE)
endif()
set(CMAKE_VERBOSE_MAKEFILE "TRUE" CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE" CACHE BOOL "" FORCE)

set(CMAKE_OBJECT_PATH_MAX 1000)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_STAGING_PREFIX "")
set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH TRUE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_ROOT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_PATH TRUE)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG FALSE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
set(CMAKE_BUILD_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS FALSE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

if("${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/cmake-install" CACHE PATH "..." FORCE)
endif()

separate_arguments(CMAKE_CXX_FLAGS NATIVE_COMMAND "${CMAKE_CXX_FLAGS}")
if(MSVC)
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} setupapi.lib")
    if(NOT "/EHa" IN_LIST CMAKE_CXX_FLAGS AND "/EHsc" IN_LIST CMAKE_CXX_FLAGS)
        list(REMOVE_ITEM CMAKE_CXX_FLAGS "/EHsc")
        list(APPEND CMAKE_CXX_FLAGS "/EHa")
    endif()
endif()
string(JOIN " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

if("${BUILD_SHARED_LIBS}")
    set(CONAN_RELEASE_COMPILER_RUNTIME "MD")
else()
    set(CONAN_RELEASE_COMPILER_RUNTIME "MT")
    #[[if("${MINGW}")
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()]]
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Generic")
    set(CONAN_OS "baremetal")
else()
    set(CONAN_OS "${CMAKE_SYSTEM_NAME}")
endif()

set(TARGET_CMAKE_INSTALL_DEPENDS_ON "${PROJECT_NAME}")
if(NOT "${SKIP_TEST}")
    set(TARGET_CMAKE_INSTALL_DEPENDS_ON "cmake-test")
    if("${TEST_GROUP}" STREQUAL "all" AND "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^.+\\..+$")
    elseif(NOT "${TEST_GROUP}" STREQUAL "all" AND "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^${TEST_GROUP}\\..+$")
    elseif("${TEST_GROUP}" STREQUAL "all" AND NOT "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^.+\\.${TEST_CASE}$")
    elseif(NOT "${TEST_GROUP}" STREQUAL "all" AND NOT "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^${TEST_GROUP}\\.${TEST_CASE}$")
    endif()

    message(STATUS "TEST_GROUP: '${TEST_GROUP}'")
    message(STATUS "TEST_CASE: '${TEST_CASE}'")
    message(STATUS "TEST_REGEXP: '${TEST_REGEXP}'")
endif()

set(CONAN_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/conan-install")
set(CONAN_INSTALL_TEST_PREFIX "${PROJECT_BINARY_DIR}/conan-install-test")

file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/lib")
file(MAKE_DIRECTORY "${CONAN_INSTALL_TEST_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_TEST_PREFIX}/lib")

if("${USE_CONAN}" AND ("msvc-16-x64-x64" IN_LIST PRESET_NAME_ELEMENTS OR "gcc" IN_LIST PRESET_NAME_ELEMENTS))
    set_conan_architecture(CONAN_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    set_conan_compiler(CONAN_COMPILER "${CMAKE_CXX_COMPILER_ID}")
    set_conan_compiler_version(CONAN_COMPILER_VERSION "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}" MAX_ELEMENTS "1")
    set_conan_compiler_runtime(CONAN_COMPILER_RUNTIME "${CMAKE_MSVC_RUNTIME_LIBRARY}")

    set_python_boolean(CONAN_INTERFACE_ONLY "${INTERFACE_ONLY}")
    set_python_boolean(CONAN_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")

    set_conan_settings(CONAN_SETTINGS
        "arch=${CONAN_ARCH}"
        "os=${CONAN_OS}"
        "compiler=${CONAN_COMPILER}"
        "compiler.version=${CONAN_COMPILER_VERSION}"
        "compiler.runtime=${CONAN_COMPILER_RUNTIME}"
        "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
        "build_type=${CMAKE_BUILD_TYPE}"
    )
    set_conan_options(CONAN_OPTIONS
        "interface=${CONAN_INTERFACE_ONLY}"
        "shared=${CONAN_BUILD_SHARED_LIBS}"
    )
    if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
        execute_process(
            COMMAND "${CONAN_COMMAND}" "install"
                    "--install-folder" "${CONAN_INSTALL_PREFIX}"
                    ${CONAN_SETTINGS}
                    ${CONAN_OPTIONS}
                    "${PROJECT_SOURCE_DIR}/conanfile.py"
            COMMAND_ECHO "STDOUT"
            ENCODING "UTF-8"
            COMMAND_ERROR_IS_FATAL "ANY"
        )
    endif()
    if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
        message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/conan-packages.cmake'")
    endif()
    include("${CONAN_INSTALL_PREFIX}/conan-packages.cmake")
    list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}")
    foreach(
        conanPackageName
        cmakePackageName
        cmakePackageVersion
        cmakePackagePath
        IN ZIP_LISTS
        "${PROJECT_NAME}_CONAN_PACKAGE_NAMES"
        "${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"
        "${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"
        "${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"
    )
        if(
            "" STREQUAL "${conanPackageName}"
            OR "" STREQUAL "${cmakePackageName}"
            OR "" STREQUAL "${cmakePackageVersion}"
            OR "" STREQUAL "${cmakePackagePath}"
        )
            string(JOIN " " errorMessage
                "Empty value 'conanPackageName': '${conanPackageName}'"
                "or 'cmakePackageName': '${cmakePackageName}'"
                "or 'cmakePackageVersion': '${cmakePackageVersion}'"
                "or 'cmakePackagePath': '${cmakePackagePath}'!"
            )
            message(FATAL_ERROR "${errorMessage}")
        elseif(NOT IS_DIRECTORY "${cmakePackagePath}")
            message(FATAL_ERROR "Not is directory 'cmakePackagePath': '${cmakePackagePath}'!")
        endif()

        if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL "")
            set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION" "${cmakePackageVersion}")
        endif()

        if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}" STREQUAL "")
            set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH" "${cmakePackagePath}")
        endif()

        list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
        list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
    endforeach()
    if(NOT "${SKIP_TEST}")
        if(NOT EXISTS "${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake")
            execute_process(
                COMMAND "${CONAN_COMMAND}" "install"
                        "--install-folder" "${CONAN_INSTALL_TEST_PREFIX}"
                        ${CONAN_SETTINGS}
                        ${CONAN_OPTIONS}
                        "${PROJECT_SOURCE_DIR}/test-conanfile.py"
                COMMAND_ECHO "STDOUT"
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL "ANY"
            )
        endif()
        if(NOT EXISTS "${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake")
            message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake'")
        endif()
        include("${CONAN_INSTALL_TEST_PREFIX}/conan-packages.cmake")
        list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_TEST_PREFIX}")
        foreach(
            conanPackageName
            cmakePackageName
            cmakePackageVersion
            cmakePackagePath
            IN ZIP_LISTS
            "${PROJECT_NAME}_CONAN_PACKAGE_NAMES"
            "${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"
            "${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"
            "${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"
        )
            if(
                "" STREQUAL "${conanPackageName}"
                OR "" STREQUAL "${cmakePackageName}"
                OR "" STREQUAL "${cmakePackageVersion}"
                OR "" STREQUAL "${cmakePackagePath}"
            )
                string(JOIN " " errorMessage
                    "Empty value 'conanPackageName': '${conanPackageName}'"
                    "or 'cmakePackageName': '${cmakePackageName}'"
                    "or 'cmakePackageVersion': '${cmakePackageVersion}'"
                    "or 'cmakePackagePath': '${cmakePackagePath}'!"
                )
                message(FATAL_ERROR "${errorMessage}")
            elseif(NOT IS_DIRECTORY "${cmakePackagePath}")
                message(FATAL_ERROR "Not is directory 'cmakePackagePath': '${cmakePackagePath}'!")
            endif()

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION" "${cmakePackageVersion}")
            endif()

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH" "${cmakePackagePath}")
            endif()

            list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
            list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
        endforeach()
    endif()
endif()

if("${${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES}" STREQUAL "")
    #[[set("${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
        "serial"
    )]]
    if(NOT "${SKIP_TEST}")
        list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
            "Easyloggingpp"
            "GTest"
        )
    endif()
endif()
message(STATUS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES:")
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    message(STATUS "  ${cmakePackageName}")
endforeach()
unset(NOT_FOUND_PACKAGE_NAMES)
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    if("Easyloggingpp" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_TEST_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET CONFIG)
    elseif("GTest" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_TEST_PREFIX}/Find${cmakePackageName}.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET CONFIG)
    else()
        message("Ignore cmakePackageName: '${cmakePackageName}'")
    endif()
endforeach()
set_not_found_package_names(NOT_FOUND_PACKAGE_NAMES ${NOT_FOUND_PACKAGE_NAMES})
if(NOT "" STREQUAL "${NOT_FOUND_PACKAGE_NAMES}")
    foreach(package IN LISTS NOT_FOUND_PACKAGE_NAMES)
        message(SEND_ERROR "${package}_FOUND: '${${package}_FOUND}' ${package}_NOT_FOUND_MESSAGE: '${${package}_NOT_FOUND_MESSAGE}'")
    endforeach()
    message(FATAL_ERROR "Some package not found!")
endif()

if("${USE_CUBE}" AND NOT EXISTS "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}")
    set(IOC_FILE "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.ioc")
    set(CUBE_OUTPUT_DIR "${PROJECT_BINARY_DIR}/../common/stm32cubemx-generated")

    configure_file("${PROJECT_SOURCE_DIR}/stm-cube-script.txt.in" "${PROJECT_BINARY_DIR}/stm-cube-script.txt" @ONLY NEWLINE_STYLE UNIX)

    execute_process(
        COMMAND "${STM_CUBE_JAVA}" "-Dfile.encoding=UTF-8" "-jar" "${STM_CUBE_FILE}" -q "${PROJECT_BINARY_DIR}/stm-cube-script.txt"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        COMMAND_ECHO "STDOUT"
        ENCODING "UTF-8"
        COMMAND_ERROR_IS_FATAL ANY
    )

    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/s/${BASE_DIR}")
    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}")

    file(COPY "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}/Core" DESTINATION "${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}")
    file(COPY "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}/Drivers" DESTINATION "${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}")
    file(COPY "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}/Middlewares" DESTINATION "${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}")
    file(COPY "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}/USB_DEVICE" DESTINATION "${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}")

    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx")
    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Drivers/CMSIS")
    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver")
    file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library")

    file(
        COPY "src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/LICENSE.txt"
        DESTINATION "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx"
    )
    file(
        COPY "src/main/c/${BASE_DIR}/Drivers/CMSIS/LICENSE.txt"
        DESTINATION "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Drivers/CMSIS"
    )
    file(
        COPY "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/LICENSE.txt"
        DESTINATION "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver"
    )
    file(
        COPY "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/LICENSE.txt"
        DESTINATION "${PROJECT_SOURCE_DIR}/src/main/resources/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library"
    )

    file(REMOVE "src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/LICENSE.txt") # remove license file
    file(REMOVE "src/main/c/${BASE_DIR}/Drivers/CMSIS/LICENSE.txt") # remove license file
    file(REMOVE "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/LICENSE.txt") # remove license file
    file(REMOVE "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/LICENSE.txt") # remove license file
    file(REMOVE_RECURSE "${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Source") # remove empty dir

    file(COPY "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}/startup_stm32h7a3xxq.s" DESTINATION "${PROJECT_SOURCE_DIR}/src/main/s/${BASE_DIR}")
    file(COPY "${CUBE_OUTPUT_DIR}/${PROJECT_NAME}/STM32H7A3ZITxQ_FLASH.ld" DESTINATION "${PROJECT_SOURCE_DIR}")
endif()

if("gcc-arm" IN_LIST PRESET_NAME_ELEMENTS)
    set(COMPILER_FLAGS
        "-mcpu=cortex-m7" # Specify the name of the target CPU.
        "-mthumb" # Generate thumb instructions.
        "-mfpu=fpv5-d16" # Specify the name of the target floating point hardware/format.
        "-mfloat-abi=hard" # Specify if floating point hardware should be used.
        "-fdata-sections" # Place each data item into its own section in the output file.
        "-ffunction-sections" # Place each function item into its own section in the output file.
        "-Wall" # Print only standard warnings, for all use Wextra.
        "-fomit-frame-pointer" # Omit the frame pointer in functions that don't need one.
        "-mabi=aapcs" # Defines enums to be a variable sized type.
        "-O0" # No optimizations, reduce compilation time and make debugging produce the expected results.
        "-fno-builtin" # Do not use built-in functions provided by GCC.

        #"-marm" # Generate code in 32 bit ARM state.
        #"-ffat-lto-objects" # Output lto objects containing both the intermediate language and binary output.
    )
    string(JOIN " " COMPILER_FLAGS ${COMPILER_FLAGS})

    set(CMAKE_C_FLAGS   "${COMPILER_FLAGS} -std=gnu${CMAKE_C_STANDARD}")
    set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")
    set(CMAKE_ASM_FLAGS "${COMPILER_FLAGS} -x assembler-with-cpp")

    set(LINKER_FLAGS
        "-T${PROJECT_SOURCE_DIR}/STM32H7A3ZITxQ_FLASH.ld" # Linker script.
        "-Wl,-Map=${PROJECT_NAME}.map,--cref" # Defines map file.
        "-mcpu=cortex-m7" # Specify the name of the target CPU.
        "-mthumb" # Generate thumb instructions.
        "-mfpu=fpv5-d16" # Specify the name of the target floating point hardware/format.
        "-mfloat-abi=hard" # Specify if floating point hardware should be used.
        "--specs=nano.specs" # Link with newlib-nano.
        "-Wl,--gc-sections" # Perform the dead code elimination.
        "-static" # Do not link against shared libraries.
        "--specs=nosys.specs" # No syscalls, provide empty implementations for the POSIX system calls.
        "-mabi=aapcs" # Defines enums to be a variable sized type.
    )
    string(JOIN " " LINKER_FLAGS ${LINKER_FLAGS})

    set(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS}")

    set(C_CXX_FLAGS_DEBUG
        "-g" # Produce debugging information in the operating system's native format.
    )
    string(JOIN " " C_CXX_FLAGS_DEBUG ${C_CXX_FLAGS_DEBUG})

    set(CMAKE_C_FLAGS_DEBUG "${C_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG "${C_CXX_FLAGS_DEBUG}")

    set(ASM_FLAGS_DEBUG
        "-g" # Produce debugging information in the operating system's native format.
    )
    string(JOIN " " ASM_FLAGS_DEBUG ${ASM_FLAGS_DEBUG})

    set(CMAKE_ASM_FLAGS_DEBUG "${ASM_FLAGS_DEBUG}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")

    set(C_CXX_FLAGS_RELEASE
        "-Os" # Optimize for size. -Os enables all -O2 optimizations.

        #"-flto" # Runs the standard link-time optimizer.
    )
    string(JOIN " " C_CXX_FLAGS_RELEASE ${C_CXX_FLAGS_RELEASE})

    set(CMAKE_C_FLAGS_RELEASE "${C_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELEASE "${C_CXX_FLAGS_RELEASE}")

    set(LINKER_FLAGS_RELEASE
        #"-flto" # Runs the standard link-time optimizer.
    )
    string(JOIN " " LINKER_FLAGS_RELEASE ${LINKER_FLAGS_RELEASE})

    set(CMAKE_ASM_FLAGS_RELEASE "")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${LINKER_FLAGS_RELEASE}")

    add_library("${PROJECT_NAME}-hal"
        "src/main/c/${BASE_DIR}/Core/Inc/main.h"
        "src/main/c/${BASE_DIR}/Core/Inc/stm32h7xx_hal_conf.h"
        "src/main/c/${BASE_DIR}/Core/Src/main.c"
        "src/main/c/${BASE_DIR}/Core/Src/stm32h7xx_hal_msp.c"
        "src/main/c/${BASE_DIR}/Core/Src/system_stm32h7xx.c"

        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include/stm32h7a3xxq.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include/stm32h7xx.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include/system_stm32h7xx.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_armcc.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_armclang.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_armclang_ltm.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_compiler.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_gcc.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_iccarm.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/cmsis_version.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_armv81mml.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_armv8mbl.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_armv8mml.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm0.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm0plus.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm1.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm23.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm3.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm33.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm35p.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm4.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_cm7.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_sc000.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/core_sc300.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/mpu_armv7.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/mpu_armv8.h"
        "src/main/c/${BASE_DIR}/Drivers/CMSIS/Include/tz_context.h"

        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_cortex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_def.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dma.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dma_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_exti.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_flash.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_flash_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_gpio.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_gpio_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_hsem.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_i2c.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_i2c_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_mdma.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pcd.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pcd_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pwr.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pwr_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rcc.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rcc_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_tim.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_tim_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_uart.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_uart_ex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_bus.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_cortex.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_crs.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_dma.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_dmamux.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_exti.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_gpio.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_hsem.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_lpuart.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_pwr.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_rcc.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_system.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_usart.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_usb.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_utils.h"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c"
        "src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usb.c"

        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/usbd_cdc.h"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_core.h"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_ctlreq.h"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_def.h"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/usbd_ioreq.h"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c"
        "src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c"

        "src/main/c/${BASE_DIR}/USB_DEVICE/App/usbd_cdc_if.c"
        "src/main/c/${BASE_DIR}/USB_DEVICE/App/usbd_cdc_if.h"
        "src/main/c/${BASE_DIR}/USB_DEVICE/App/usbd_desc.c"
        "src/main/c/${BASE_DIR}/USB_DEVICE/App/usbd_desc.h"
        "src/main/c/${BASE_DIR}/USB_DEVICE/App/usb_device.c"
        "src/main/c/${BASE_DIR}/USB_DEVICE/App/usb_device.h"
        "src/main/c/${BASE_DIR}/USB_DEVICE/Target/usbd_conf.c"
        "src/main/c/${BASE_DIR}/USB_DEVICE/Target/usbd_conf.h"
    )
    target_include_directories("${PROJECT_NAME}-hal" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Core/Inc>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Drivers/CMSIS/Include>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/USB_DEVICE/App>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/USB_DEVICE/Target>"
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/c/${BASE_DIR}/st-integration>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_compile_definitions("${PROJECT_NAME}-hal" PUBLIC
        "USE_HAL_DRIVER"
        "STM32H7A3xxQ"
    )
    set_target_properties("${PROJECT_NAME}-hal" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"
    )

    set("${PROJECT_NAME}-source-files"
        "src/main/cpp/${BASE_DIR}/Hardware.cpp"
    )
elseif("msvc-16-x64-x64" IN_LIST PRESET_NAME_ELEMENTS OR "gcc" IN_LIST PRESET_NAME_ELEMENTS)
    set("${PROJECT_NAME}-source-files"
        "src/main/cpp/${BASE_DIR}/HardwareStub.cpp"
    )
endif()

configure_file("versions.hpp.in" "generated/src/main/cpp/${BASE_DIR}/versions.hpp" @ONLY NEWLINE_STYLE UNIX)

add_library("${PROJECT_NAME}"
    "src/main/cpp/${BASE_DIR}/Application.hpp"
    "src/main/cpp/${BASE_DIR}/Application.cpp"
    "src/main/cpp/${BASE_DIR}/IHardware.hpp"
    "src/main/cpp/${BASE_DIR}/Hardware.hpp"
    ${${PROJECT_NAME}-source-files}
)
generate_export_header("${PROJECT_NAME}" BASE_NAME "${BASE_NAME}" EXPORT_FILE_NAME "generated/src/main/cpp/${BASE_DIR}/export.hpp")
target_include_directories("${PROJECT_NAME}" PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/cpp>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries("${PROJECT_NAME}"
    "${PROJECT_NAME}-hal"
)
set_target_properties("${PROJECT_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/bin"

    ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"

    LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"
)

if("gcc-arm" IN_LIST PRESET_NAME_ELEMENTS)
    add_executable("${PROJECT_NAME}-app"
        "src/main/c/${BASE_DIR}/Core/Inc/stm32h7xx_it.h"
        "src/main/c/${BASE_DIR}/Core/Src/stm32h7xx_it.c"
        "src/main/s/${BASE_DIR}/startup_stm32h7a3xxq.s"
        "src/main/cpp/main.cpp"
    )
    target_link_libraries("${PROJECT_NAME}-app"
        "${PROJECT_NAME}"
    )
    set_target_properties("${PROJECT_NAME}-app" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"
    )
    add_custom_command(TARGET "${PROJECT_NAME}-app"
        POST_BUILD
        COMMAND "${CMAKE_SIZE}" "$<TARGET_FILE:${PROJECT_NAME}-app>"
        COMMAND "${CMAKE_COMMAND}" "-E" "copy" "$<TARGET_FILE:${PROJECT_NAME}-app>" "$<TARGET_FILE_DIR:${PROJECT_NAME}-app>/${PROJECT_NAME}-app.elf"
        COMMAND "${CMAKE_OBJCOPY}" "-O" "ihex" "$<TARGET_FILE_DIR:${PROJECT_NAME}-app>/${PROJECT_NAME}-app.elf" "$<TARGET_FILE_DIR:${PROJECT_NAME}-app>/${PROJECT_NAME}-app.hex"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )
    install(
        PROGRAMS "$<TARGET_FILE:${PROJECT_NAME}-app>"
                 "$<TARGET_FILE_DIR:${PROJECT_NAME}-app>/${PROJECT_NAME}-app.elf"
                 "$<TARGET_FILE_DIR:${PROJECT_NAME}-app>/${PROJECT_NAME}-app.hex"
        DESTINATION "bin"
    )

    add_custom_target("cmake-flash-write"
        COMMAND "${OPENOCD_COMMAND}" "-f" "interface/stlink.cfg" "-f" "target/stm32h7x.cfg" "-c" "program $<TARGET_FILE:${PROJECT_NAME}-app>.elf verify reset exit"
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        USES_TERMINAL
        VERBATIM
    )
endif()

if(NOT "${SKIP_TEST}" AND ("msvc-16-x64-x64" IN_LIST PRESET_NAME_ELEMENTS OR "gcc" IN_LIST PRESET_NAME_ELEMENTS))
    add_library("test-lib" STATIC
        "src/test/cpp/TestConfiguration.hpp"
        "src/test/cpp/TestConfiguration.cpp"
        "src/test/cpp/TestUtils.hpp"
        "src/test/cpp/TestUtils.cpp"
        "src/test/cpp/TestLogging.hpp"
        "src/test/cpp/TestLogging.cpp"
        "src/test/cpp/TestApplication.hpp"
        "src/test/cpp/TestApplication.cpp"
        "src/test/cpp/TestThreadPool.hpp"
        "src/test/cpp/TestThreadPool.cpp"
        "src/test/cpp/${BASE_DIR}/OtherUnitTests.hpp"
    )
    target_include_directories("test-lib" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/test/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_link_libraries("test-lib" PUBLIC
        "${PROJECT_NAME}"
        "Easyloggingpp::Easyloggingpp"
        "GTest::gmock"
        "GTest::gtest"
        "GTest::gmock_main"
        "GTest::gtest_main"
    )
    set_target_properties("test-lib" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
    )

    add_executable("test-app" "src/test/cpp/main.cpp")
    target_link_libraries("test-app"
        "test-lib"
    )
    set_target_properties("test-app" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
    )
    if("${BUILD_SHARED_LIBS}")
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_TEST_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                USES_TERMINAL
                VERBATIM
            )
        else()
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},LIBRARY_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_TEST_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                USES_TERMINAL
                VERBATIM
            )
        endif()
    endif()

    gtest_discover_tests("test-app"
        EXTRA_ARGS "--logging-config" "${PROJECT_SOURCE_DIR}/src/test/resources/logging-config.txt"
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/test/bin"
        PROPERTIES XML_OUTPUT_DIR "${PROJECT_BINARY_DIR}/test/report/xml"
                   #ENVIRONMENT "PARENT_PATH=;PATH="
    )

    add_custom_target("cmake-test"
        COMMAND "${CMAKE_CTEST_COMMAND}" --preset "${PRESET_NAME}" "-R" "${TEST_REGEXP}"
        DEPENDS "test-app"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )
endif()

set_target_names(ALL_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "all-targets:")
foreach(t IN LISTS ALL_TARGETS)
    message(STATUS "${t}")
endforeach()

# Generate vscode files if env var VSCODE value is "true"
if("$ENV{VSCODE}")
    include("${PROJECT_SOURCE_DIR}/cmake/vscode.cmake")
    if("$ENV{VSCODE_SETTINGS}")
        vscode(SETTINGS_FILE "${PROJECT_SOURCE_DIR}/.vscode/settings.json")
    endif()
    if("$ENV{VSCODE_C_CPP_PROPERTIES}")
        vscode(C_CPP_PROPERTIES_FILE "${PROJECT_SOURCE_DIR}/.vscode/c_cpp_properties.json")
    endif()
    if("$ENV{VSCODE_TASKS}")
        vscode(TASKS_FILE "${PROJECT_SOURCE_DIR}/.vscode/tasks.json")
    endif()
    if("$ENV{VSCODE_LAUNCH}")
        vscode(LAUNCH_FILE "${PROJECT_SOURCE_DIR}/.vscode/launch.json")
    endif()
endif()
