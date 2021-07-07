cmake_minimum_required(VERSION 3.0)

project(packet-manager LANGUAGES CXX)

# TODO: c++ standard
#set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    src/main.cpp)

set(PRIVATE_HEADER_FILES
    )

set(CXX_PROJECT_COMPILE_FLAGS -g -O -fstack-protector
    -Wall -pedantic -Wextra -Wcast-align -Wcomment -Wconversion
    -Wdisabled-optimization -Werror -Wfloat-equal
    -Wmissing-braces -Wmissing-field-initializers
    -Wmissing-noreturn -Wpacked -Wparentheses -Wredundant-decls
    -Wreturn-type -Wsequence-point  -Wshadow -Wsign-compare
    -Wswitch-default -Wuninitialized
    -Wunknown-pragmas -Wunused -Wunused-function -Wunused-parameter
    -Wunused-value -Wunused-variable -Wwrite-strings
    -Wnull-dereference  -Wtautological-compare -Wmissing-declarations)

set(CXX_PROJECT_LINK_FLAGS
    )

set(CXX_PROJECT_COMPILE_DEFINITIONS
    -DEXECUTABLE_PROJECT_NAME=\"${PROJECT_NAME}\")



add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})
target_compile_options(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_COMPILE_FLAGS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_LINK_FLAGS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${CXX_PROJECT_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PRIVATE src)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT binary)

