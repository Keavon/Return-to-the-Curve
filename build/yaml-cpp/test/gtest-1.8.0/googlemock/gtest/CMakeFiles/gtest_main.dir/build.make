# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gyatso/Documents/cpe476/CPE-476-Project-Curve

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build

# Include any dependencies generated for this target.
include yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/depend.make

# Include the progress variables for this target.
include yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/progress.make

# Include the compile flags for this target's objects.
include yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/flags.make

yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o: yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/flags.make
yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o: ../yaml-cpp/test/gtest-1.8.0/googletest/src/gtest_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o"
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest_main.dir/src/gtest_main.cc.o -c /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/yaml-cpp/test/gtest-1.8.0/googletest/src/gtest_main.cc

yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest_main.dir/src/gtest_main.cc.i"
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/yaml-cpp/test/gtest-1.8.0/googletest/src/gtest_main.cc > CMakeFiles/gtest_main.dir/src/gtest_main.cc.i

yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest_main.dir/src/gtest_main.cc.s"
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/yaml-cpp/test/gtest-1.8.0/googletest/src/gtest_main.cc -o CMakeFiles/gtest_main.dir/src/gtest_main.cc.s

# Object files for target gtest_main
gtest_main_OBJECTS = \
"CMakeFiles/gtest_main.dir/src/gtest_main.cc.o"

# External object files for target gtest_main
gtest_main_EXTERNAL_OBJECTS =

yaml-cpp/test/gtest-1.8.0/googlemock/gtest/libgtest_main.a: yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/src/gtest_main.cc.o
yaml-cpp/test/gtest-1.8.0/googlemock/gtest/libgtest_main.a: yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/build.make
yaml-cpp/test/gtest-1.8.0/googlemock/gtest/libgtest_main.a: yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libgtest_main.a"
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean_target.cmake
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/build: yaml-cpp/test/gtest-1.8.0/googlemock/gtest/libgtest_main.a

.PHONY : yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/build

yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/clean:
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest_main.dir/cmake_clean.cmake
.PHONY : yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/clean

yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/depend:
	cd /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gyatso/Documents/cpe476/CPE-476-Project-Curve /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/yaml-cpp/test/gtest-1.8.0/googletest /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest /home/gyatso/Documents/cpe476/CPE-476-Project-Curve/build/yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : yaml-cpp/test/gtest-1.8.0/googlemock/gtest/CMakeFiles/gtest_main.dir/depend
