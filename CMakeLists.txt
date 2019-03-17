cmake_minimum_required(VERSION 3.13)
project(rsocket VERSION 0.4 DESCRIPTION "C++ wrapper for socket library")

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_library(rsocket SHARED
        server_socket.h
        server_socket.cpp
        client_socket.h
        client_socket.cpp
        utilities.cpp
        utilities.h)

set_target_properties(rsocket PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rsocket PROPERTIES SOVERSION 0.4)

# Public API
#set_target_properties(rsocket PROPERTIES PUBLIC_HEADER include/rsocket.h)

# pkg-config
#configure_file(rsocket.pc.in rsocket.pc @ONLY)
#install(FILES ${CMAKE_BINARY_DIR}/rsocket.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# Install
#include(GNUInstallDirs)
#install(TARGETS rsocket
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
