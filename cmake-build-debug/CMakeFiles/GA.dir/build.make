# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.12

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2018.2.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2018.2.1\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\Paper-Code\LoGA

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\Paper-Code\LoGA\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/GA.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/GA.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/GA.dir/flags.make

CMakeFiles/GA.dir/main.cpp.obj: CMakeFiles/GA.dir/flags.make
CMakeFiles/GA.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\Paper-Code\LoGA\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/GA.dir/main.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\GA.dir\main.cpp.obj -c D:\Paper-Code\LoGA\main.cpp

CMakeFiles/GA.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/GA.dir/main.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\Paper-Code\LoGA\main.cpp > CMakeFiles\GA.dir\main.cpp.i

CMakeFiles/GA.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/GA.dir/main.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\Paper-Code\LoGA\main.cpp -o CMakeFiles\GA.dir\main.cpp.s

# Object files for target GA
GA_OBJECTS = \
"CMakeFiles/GA.dir/main.cpp.obj"

# External object files for target GA
GA_EXTERNAL_OBJECTS =

GA.exe: CMakeFiles/GA.dir/main.cpp.obj
GA.exe: CMakeFiles/GA.dir/build.make
GA.exe: CMakeFiles/GA.dir/linklibs.rsp
GA.exe: CMakeFiles/GA.dir/objects1.rsp
GA.exe: CMakeFiles/GA.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\Paper-Code\LoGA\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable GA.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\GA.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/GA.dir/build: GA.exe

.PHONY : CMakeFiles/GA.dir/build

CMakeFiles/GA.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\GA.dir\cmake_clean.cmake
.PHONY : CMakeFiles/GA.dir/clean

CMakeFiles/GA.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\Paper-Code\LoGA D:\Paper-Code\LoGA D:\Paper-Code\LoGA\cmake-build-debug D:\Paper-Code\LoGA\cmake-build-debug D:\Paper-Code\LoGA\cmake-build-debug\CMakeFiles\GA.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/GA.dir/depend

