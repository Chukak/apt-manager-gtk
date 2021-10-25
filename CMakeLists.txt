cmake_minimum_required(VERSION 3.0)

project(apt-manager-gtk LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17) # 20 standard in next releases

set(SOURCE_FILES
    src/main.cpp
    src/package/cache.cpp
    src/utils.cpp
    src/package/textacquirestatus.cpp
    src/widget/sections.cpp
    src/widget/mainwindow.cpp
    src/widget/button.cpp
    src/widget/candidates.cpp
    src/widget/progressbar.cpp
    src/extension/progressrange.cpp
    src/extension/progresspulse.cpp
    src/package/progressacquirestatus.cpp
    src/widget/logwindow.cpp
    src/widget/menu.cpp)

set(PRIVATE_HEADER_FILES
    src/package/cache.h
    src/utils.h
    src/package/textacquirestatus.h
    src/widget/sections.h
    src/type.h
    src/widget/mainwindow.h
    src/widget/button.h
    src/widget/candidates.h
    src/widget/progressbar.h
    src/extension/progressrange.h
    src/extension/progresspulse.h
    src/package/progressacquirestatus.h
    src/widget/logwindow.h
    src/widget/menu.h)

set(CXX_PROJECT_COMPILE_FLAGS -O -fstack-protector
    -Wall -pedantic -Wextra -Wcast-align -Wcomment -Wconversion
    -Wdisabled-optimization -Werror -Wfloat-equal
    -Wmissing-braces -Wmissing-field-initializers
    -Wmissing-noreturn -Wpacked -Wparentheses #-Wredundant-decls
    -Wreturn-type -Wsequence-point  -Wshadow -Wsign-compare
    -Wuninitialized
    -Wunknown-pragmas -Wunused -Wunused-function -Wunused-parameter
    -Wunused-value -Wunused-variable -Wwrite-strings
    -Wnull-dereference  -Wtautological-compare -Wmissing-declarations)

set(CXX_PROJECT_LINK_FLAGS
    )

set(CXX_PROJECT_LINK_DIRECTORIES
    )

set(CXX_PROJECT_COMPILE_DEFINITIONS
    -DEXECUTABLE_PROJECT_NAME=\"${PROJECT_NAME}\")

set(CXX_PROJECT_INCLUDE_DIRECTORIES
    )

# libasan
# WARNING: Pass this before running application with libasan:
# ASAN_OPTIONS=new_delete_type_mismatch=0
#
# See more: https://gitlab.gnome.org/GNOME/gtkmm/-/issues/65
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND CXX_PROJECT_COMPILE_FLAGS -g)
        if(UNIX)
            list(APPEND CXX_PROJECT_COMPILE_FLAGS -fno-omit-frame-pointer -fsanitize=address)
            list(APPEND CXX_PROJECT_LINK_FLAGS -lasan)
        endif()
    endif()
endif()

# GTK3 (c++)
find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTKMM REQUIRED gtkmm-3.0)

list(APPEND CXX_PROJECT_LINK_FLAGS ${GTKMM_LIBRARIES})
list(APPEND CXX_PROJECT_LINK_DIRECTORIES ${GTKMM_LIBRARY_DIRS})
list(APPEND CXX_PROJECT_INCLUDE_DIRECTORIES ${GTKMM_INCLUDE_DIRS})

# libapt-pkg
find_library(APT_PKG_LIBRARIES NAMES apt-pkg REQUIRED)

list(APPEND CXX_PROJECT_LINK_FLAGS ${APT_PKG_LIBRARIES})

set(CXX_COMPILED_RESOURCES ${PROJECT_NAME}-gtk-resources)
set(CXX_COMPILED_RESOURCE_SRC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.gresource)
set(CXX_UI_RESOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/ui/main.ui)
find_program(GLIB_COMPILE_RESOURCES_CMD NAMES glib-compile-resources REQUIRED)

if(GLIB_COMPILE_RESOURCES_CMD)
    set(CXX_RESOURCE_MAIN_FILE
        resources.xml)
    set(CXX_RESOURCE_MAIN_FILE_PATH
        src/resources)
    set(CXX_RESOURCE_SRC_FILES
        src/resources/ui/main.ui)

    add_custom_command(
        OUTPUT ${CXX_COMPILED_RESOURCE_SRC_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CXX_RESOURCE_MAIN_FILE_PATH}
        COMMAND ${GLIB_COMPILE_RESOURCES_CMD}
        ARGS
            --target=${CXX_COMPILED_RESOURCE_SRC_FILE}
            ${CXX_RESOURCE_MAIN_FILE}
        VERBATIM
        MAIN_DEPENDENCY ${CXX_RESOURCE_MAIN_FILE_PATH}/${CXX_RESOURCE_MAIN_FILE}
        DEPENDS ${CXX_RESOURCE_SRC_FILES})

    add_custom_target(${CXX_COMPILED_RESOURCES}
        DEPENDS ${CXX_COMPILED_RESOURCE_SRC_FILE} SOURCES ${CXX_RESOURCE_SRC_FILES})

    set(UI_FILENAME /ui/main.ui)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(UI_RESOURCE_FILE ${CXX_COMPILED_RESOURCE_SRC_FILE})
    elseif(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")
        message(${CMAKE_INSTALL_PREFIX})
        set(UI_RESOURCE_FILE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/resource/${PROJECT_NAME}.gresource)
    endif()

    list(APPEND CXX_PROJECT_COMPILE_DEFINITIONS
        -DUI_RESOURCE_FILE=\"${UI_RESOURCE_FILE}\"
        -DUI_FILENAME=\"${UI_FILENAME}\"
        )
else()
    message(FATAL "No compiled resources.")
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${PRIVATE_HEADER_FILES}
    ${CXX_COMPILED_RESOURCE_SRC_FILE})
set_source_files_properties(
    ${CXX_COMPILED_RESOURCE_SRC_FILE}
    PROPERTIES GENERATED TRUE)
add_dependencies(${PROJECT_NAME} ${CXX_COMPILED_RESOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_LINK_FLAGS})
target_link_directories(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_LINK_DIRECTORIES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PRIVATE src ${CXX_PROJECT_INCLUDE_DIRECTORIES})

if (CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "None")
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT binary)
    install(FILES ${CXX_COMPILED_RESOURCE_SRC_FILE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/resource/)
    install(FILES ${CXX_UI_RESOURCE_FILE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/resource/ui/)
    # icons
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/image DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/resource/)
endif()

