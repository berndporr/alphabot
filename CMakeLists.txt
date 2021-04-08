cmake_minimum_required(VERSION 3.1.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

project(Alphabot_Library LANGUAGES CXX)
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall)

find_package (Threads)
find_package (Curses)

set(LIBSRC alphabot.cpp)
set(LIBINCLUDE alphabot.h)

add_library(alphabot
  SHARED
  ${LIBSRC}
  )

set_target_properties(alphabot PROPERTIES
  SOVERSION 1
  PUBLIC_HEADER "${LIBINCLUDE}")

target_link_libraries(alphabot ${CMAKE_THREAD_LIBS_INIT} pigpiod_if2 rt)

install(TARGETS alphabot EXPORT alphabot-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_library(alphabot_static
  STATIC
  ${LIBSRC}
  )

set_target_properties(alphabot_static PROPERTIES
  PUBLIC_HEADER "${LIBINCLUDE}")

target_link_libraries(alphabot_static ${CMAKE_THREAD_LIBS_INIT} pigpiod_if2 rt)

install(TARGETS alphabot_static EXPORT alphabot_static-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_executable (testIO testIO.cpp)
target_link_libraries(testIO alphabot_static ncurses)

add_executable (testMotor testMotor.cpp)
target_link_libraries(testMotor alphabot_static ncurses)

add_executable (obstacleAvoid obstacleAvoid.cpp)
target_link_libraries(obstacleAvoid alphabot_static ncurses)
