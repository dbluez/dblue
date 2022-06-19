cmake_minimum_required(VERSION 3.0.0)
project(deepblue)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

# 指定编译选项
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -std=c++11 -O0 -ggdb -Wall -Werror")

# -rdynamic: 将所有符号都加入到符号表中，便于使用dlopen或者backtrace追踪到符号
# -fPIC: 生成位置无关的代码，便于动态链接
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -fPIC")

# -Wno-unused-function: 不要警告未使用函数
# -Wno-builtin-macro-redefined: 不要警告内置宏重定义，用于重定义内置的__FILE__宏
# -Wno-deprecated: 不要警告过时的特性
# -Wno-deprecated-declarations: 不要警告使用带deprecated属性的变量，类型，函数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated -Wno-deprecated-declarations")


include_directories (${PROJECT_SOURCE_DIR}/deepblue/include)
# include_directories (/usr/local/include)
# link_directories(/usr/local/lib64)

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
message("***", ${YAMLCPP})

set(LIB_SRC
    deepblue/log.cc
    deepblue/env.cc
    deepblue/config.cc
    deepblue/util.cpp
    deepblue/thread.cc
    deepblue/mutex.cc
    deepblue/fiber.cc
    deepblue/scheduler.cc
    deepblue/iomanager.cc
    deepblue/timer.cc
    deepblue/fd_manager.cc
    deepblue/hook.cc
    deepblue/address.cc
    deepblue/socket.cc
    deepblue/bytearray.cc
    deepblue/http/thirdparty/http_parser.c
    deepblue/http/http.cc
    deepblue/http/http_parser.cc
    deepblue/tcp_server.cc
    deepblue/stream.cc
    deepblue/streams/socket_stream.cc
    deepblue/http/http_session.cc
    deepblue/http/http_server.cc
    deepblue/http/servlet.cc
    deepblue/uri.cc
    deepblue/http/http_connection.cc
    deepblue/daemon.cc
    deepblue/application.cc
    )

set(LIBS
    deepblue
    dl
    pthread
    ${YAMLCPP}
)

message("***", ${LIBS})


add_library(deepblue SHARED ${LIB_SRC})
redefine_file_macro(deepblue)
#add_library(deepblue_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (deepblue_static PROPERTIES OUTPUT_NAME "deepblue")

add_executable(test tests/test.cc)
add_dependencies (test deepblue)
redefine_file_macro(test)
target_link_libraries (test ${LIBS})

add_executable(test_config tests/test_config.cc)
add_dependencies (test_config deepblue)
redefine_file_macro(test_config)
target_link_libraries (test_config  ${LIBS})

add_executable(test_thread tests/test_thread.cc)
add_dependencies (test_thread deepblue)
redefine_file_macro(test_thread)
target_link_libraries (test_thread  ${LIBS})

add_executable(test_util tests/test_util.cc)
add_dependencies (test_util deepblue)
redefine_file_macro(test_util)
target_link_libraries (test_util  ${LIBS})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies (test_fiber deepblue)
redefine_file_macro(test_fiber)
target_link_libraries (test_fiber  ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies (test_scheduler deepblue)
redefine_file_macro(test_scheduler)
target_link_libraries (test_scheduler  ${LIBS})

add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies (test_iomanager deepblue)
redefine_file_macro(test_iomanager)
target_link_libraries (test_iomanager  ${LIBS})

add_executable(test_hook tests/test_hook.cc)
add_dependencies (test_hook deepblue)
redefine_file_macro(test_hook)
target_link_libraries (test_hook  ${LIBS})

add_executable(test_address tests/test_address.cc)
add_dependencies (test_address deepblue)
redefine_file_macro(test_address)
target_link_libraries (test_address  ${LIBS})

add_executable(test_socket tests/test_socket.cc)
add_dependencies (test_socket deepblue)
redefine_file_macro(test_socket)
target_link_libraries (test_socket  ${LIBS})

add_executable(test_bytearray tests/test_bytearray.cc)
add_dependencies (test_bytearray deepblue)
redefine_file_macro(test_bytearray)
target_link_libraries (test_bytearray  ${LIBS})

add_executable(test_http tests/test_http.cc)
add_dependencies (test_http deepblue)
redefine_file_macro(test_http)
target_link_libraries (test_http  ${LIBS})

add_executable(test_http_parser tests/test_http_parser.cc)
add_dependencies (test_http_parser deepblue)
redefine_file_macro(test_http_parser)
target_link_libraries (test_http_parser  ${LIBS})

add_executable(test_tcp_server tests/test_tcp_server.cc)
add_dependencies (test_tcp_server deepblue)
redefine_file_macro(test_tcp_server)
target_link_libraries (test_tcp_server  ${LIBS})

add_executable(echo_server examples/echo_server.cc)
add_dependencies (echo_server deepblue)
redefine_file_macro(echo_server)
target_link_libraries (echo_server  ${LIBS})

add_executable(test_http_server tests/test_http_server.cc)
add_dependencies (test_http_server deepblue)
redefine_file_macro(test_http_server)
target_link_libraries (test_http_server  ${LIBS})

add_executable(test_http_connection tests/test_http_connection.cc)
add_dependencies (test_http_connection deepblue)
redefine_file_macro(test_http_connection)
target_link_libraries (test_http_connection  ${LIBS})

add_executable(test_uri tests/test_uri.cc)
add_dependencies (test_uri deepblue)
redefine_file_macro(test_uri)
target_link_libraries (test_uri  ${LIBS})

add_executable(my_http_server workbench/my_http_server.cc)
add_dependencies (my_http_server deepblue)
redefine_file_macro(my_http_server)
target_link_libraries (my_http_server  ${LIBS})

add_executable(test_daemon tests/test_daemon.cc)
add_dependencies (test_daemon deepblue)
redefine_file_macro(test_daemon)
target_link_libraries (test_daemon  ${LIBS})

add_executable(test_env tests/test_env.cc)
add_dependencies (test_env deepblue)
redefine_file_macro(test_env)
target_link_libraries (test_env  ${LIBS})

add_executable(test_application tests/test_application.cc)
add_dependencies (test_application deepblue)
redefine_file_macro(test_application)
target_link_libraries (test_application  ${LIBS})

add_executable(test_node tests/test_node.cc)
target_link_libraries (test_node ${YAMLCPP})

SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)