cmake_minimum_required(VERSION 2.7)

# Name of the project
project(curve)

# Use glob to get the list of all source files.
file(GLOB_RECURSE SOURCES "Source/*.cpp" "glad/src/*.c")

# We don't really need to include header and resource files to build, but it's
# nice to have them show up in IDEs.
file(GLOB_RECURSE HEADERS "Source/*.h" "glad/*/*.h")
file(GLOB_RECURSE GLSL "resources/*.glsl")

set(IRRKLANG_BIN_DIR ${CMAKE_SOURCE_DIR}/irrKlang/bin)
set(IRRKLANG_LIB_DIR ${CMAKE_SOURCE_DIR}/irrKlang/lib)

# Set the executable.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

include_directories("glad/include")
include_directories("irrKlang/include")

# Add GLFW
# Get the GLFW environment variable.
# There should be a CMakeLists.txt in the specified directory.
set(GLFW_DIR "$ENV{GLFW_DIR}")
if(GLFW_DIR)
  message(STATUS "GLFW environment variable found")

  option(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
  option(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
  option(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
  if(CMAKE_BUILD_TYPE MATCHES Release)
    add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
  else()
    add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
  endif()

  include_directories(${GLFW_DIR}/include)
  target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})
else()
  message(STATUS "GLFW environment variable `GLFW_DIR` not found, GLFW3 must be installed with the system")

  find_package(PkgConfig)
  if (PKGCONFIG_FOUND)
    message(STATUS "PkgConfig found")
    pkg_search_module(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${GLFW_LIBRARIES})
  else()
    message(STATUS "No PkgConfig found")
    find_package(glfw3 REQUIRED)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
  endif()
endif()



# Add GLM
# Get the GLM environment variable. Since GLM is a header-only library, we
# just need to add it to the include directory.
set(GLM_INCLUDE_DIR "$ENV{GLM_INCLUDE_DIR}")
if(GLM_INCLUDE_DIR)
  include_directories(${GLM_INCLUDE_DIR})
  message(STATUS "GLM environment variable found")
else()
# If the GLM_INCLUDE_DIR environment variable is not set, we assume
# the user has installed GLM properly on their system
  message(STATUS "GLM environment variable `GLM_INCLUDE_DIR` not found, GLM must be installed with the system")
endif()



# OS specific options and libraries

# WINDOWS
if(WIN32)
  target_link_libraries(${CMAKE_PROJECT_NAME} opengl32.lib)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${IRRKLANG_LIB_DIR}/Winx64-visualStudio/irrKlang.lib)

else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

  # OSX
  if(APPLE)
    target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
    target_link_libraries(${CMAKE_PROJECT_NAME} ${IRRKLANG_BIN_DIR}/macosx-gcc/libirrklang.dylib)

  # LINUX
  else()
    target_link_libraries(${CMAKE_PROJECT_NAME} "GL" "dl")
    target_link_libraries(${CMAKE_PROJECT_NAME} ${IRRKLANG_BIN_DIR}/linux-gcc-64/libIrrKlang.so)
    
  endif()
endif()
