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
CMAKE_COMMAND = /opt/clion-2018.3.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2018.3.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hutao/CLionProjects/AssVec-ADMM-MPI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Ass.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Ass.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Ass.dir/flags.make

CMakeFiles/Ass.dir/main.cpp.o: CMakeFiles/Ass.dir/flags.make
CMakeFiles/Ass.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Ass.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Ass.dir/main.cpp.o -c /home/hutao/CLionProjects/AssVec-ADMM-MPI/main.cpp

CMakeFiles/Ass.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Ass.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hutao/CLionProjects/AssVec-ADMM-MPI/main.cpp > CMakeFiles/Ass.dir/main.cpp.i

CMakeFiles/Ass.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Ass.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hutao/CLionProjects/AssVec-ADMM-MPI/main.cpp -o CMakeFiles/Ass.dir/main.cpp.s

# Object files for target Ass
Ass_OBJECTS = \
"CMakeFiles/Ass.dir/main.cpp.o"

# External object files for target Ass
Ass_EXTERNAL_OBJECTS =

Ass: CMakeFiles/Ass.dir/main.cpp.o
Ass: CMakeFiles/Ass.dir/build.make
Ass: CMakeFiles/Ass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Ass"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Ass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Ass.dir/build: Ass

.PHONY : CMakeFiles/Ass.dir/build

CMakeFiles/Ass.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Ass.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Ass.dir/clean

CMakeFiles/Ass.dir/depend:
	cd /home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hutao/CLionProjects/AssVec-ADMM-MPI /home/hutao/CLionProjects/AssVec-ADMM-MPI /home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug /home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug /home/hutao/CLionProjects/AssVec-ADMM-MPI/cmake-build-debug/CMakeFiles/Ass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Ass.dir/depend

