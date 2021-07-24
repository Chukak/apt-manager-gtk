cmake_minimum_required(VERSION 3.0)

project(package-manager LANGUAGES CXX)

# TODO: c++ standard
#set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    src/main.cpp
    src/cache.cpp
    src/utils.cpp
    src/textacquirestatus.cpp)

set(PRIVATE_HEADER_FILES
    src/cache.h
    src/utils.h
    src/textacquirestatus.h)

set(CXX_PROJECT_COMPILE_FLAGS -g -O -fstack-protector
    -Wall -pedantic -Wextra -Wcast-align -Wcomment -Wconversion
    -Wdisabled-optimization -Werror -Wfloat-equal
    -Wmissing-braces -Wmissing-field-initializers
    -Wmissing-noreturn -Wpacked -Wparentheses -Wredundant-decls
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

# GTK3 (c++)
find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtkmm-3.0)

list(APPEND CXX_PROJECT_LINK_FLAGS ${GTKMM_LIBRARIES})
list(APPEND CXX_PROJECT_LINK_DIRECTORIES ${GTKMM_LIBRARY_DIRS})
list(APPEND CXX_PROJECT_INCLUDE_DIRECTORIES ${GTKMM_INCLUDE_DIRS})
list(APPEND CXX_PROJECT_COMPILE_DEFINITIONS ${GTKMM_CFLAGS_OTHER})

# libapt-pkg
find_library(APT_PKG_LIBRARIES NAMES apt-pkg REQUIRED)

list(APPEND CXX_PROJECT_LINK_FLAGS ${APT_PKG_LIBRARIES})

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})
target_compile_options(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_LINK_FLAGS})
target_link_directories(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_LINK_DIRECTORIES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PRIVATE src PUBLIC ${CXX_PROJECT_INCLUDE_DIRECTORIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT binary)

