# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build

# Include any dependencies generated for this target.
include CMakeFiles/stltest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/stltest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/stltest.dir/flags.make

CMakeFiles/stltest.dir/test_my.o: CMakeFiles/stltest.dir/flags.make
CMakeFiles/stltest.dir/test_my.o: ../test_my.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/stltest.dir/test_my.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/stltest.dir/test_my.o -c /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/test_my.cpp

CMakeFiles/stltest.dir/test_my.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/stltest.dir/test_my.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/test_my.cpp > CMakeFiles/stltest.dir/test_my.i

CMakeFiles/stltest.dir/test_my.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/stltest.dir/test_my.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/test_my.cpp -o CMakeFiles/stltest.dir/test_my.s

# Object files for target stltest
stltest_OBJECTS = \
"CMakeFiles/stltest.dir/test_my.o"

# External object files for target stltest
stltest_EXTERNAL_OBJECTS =

../bin/stltest: CMakeFiles/stltest.dir/test_my.o
../bin/stltest: CMakeFiles/stltest.dir/build.make
../bin/stltest: CMakeFiles/stltest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/stltest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/stltest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/stltest.dir/build: ../bin/stltest

.PHONY : CMakeFiles/stltest.dir/build

CMakeFiles/stltest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/stltest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/stltest.dir/clean

CMakeFiles/stltest.dir/depend:
	cd /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build /home/lsl/Desktop/code/Project/TinyStl/MyTinySTL_my/Test/build/CMakeFiles/stltest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/stltest.dir/depend

