# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/ebotti/Redes/videoTransfer/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ebotti/Redes/videoTransfer/src

# Include any dependencies generated for this target.
include CMakeFiles/servidor.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/servidor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/servidor.dir/flags.make

CMakeFiles/servidor.dir/servidor.cc.o: CMakeFiles/servidor.dir/flags.make
CMakeFiles/servidor.dir/servidor.cc.o: servidor.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ebotti/Redes/videoTransfer/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/servidor.dir/servidor.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/servidor.dir/servidor.cc.o -c /home/ebotti/Redes/videoTransfer/src/servidor.cc

CMakeFiles/servidor.dir/servidor.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/servidor.dir/servidor.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ebotti/Redes/videoTransfer/src/servidor.cc > CMakeFiles/servidor.dir/servidor.cc.i

CMakeFiles/servidor.dir/servidor.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/servidor.dir/servidor.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ebotti/Redes/videoTransfer/src/servidor.cc -o CMakeFiles/servidor.dir/servidor.cc.s

CMakeFiles/servidor.dir/servidor.cc.o.requires:

.PHONY : CMakeFiles/servidor.dir/servidor.cc.o.requires

CMakeFiles/servidor.dir/servidor.cc.o.provides: CMakeFiles/servidor.dir/servidor.cc.o.requires
	$(MAKE) -f CMakeFiles/servidor.dir/build.make CMakeFiles/servidor.dir/servidor.cc.o.provides.build
.PHONY : CMakeFiles/servidor.dir/servidor.cc.o.provides

CMakeFiles/servidor.dir/servidor.cc.o.provides.build: CMakeFiles/servidor.dir/servidor.cc.o


# Object files for target servidor
servidor_OBJECTS = \
"CMakeFiles/servidor.dir/servidor.cc.o"

# External object files for target servidor
servidor_EXTERNAL_OBJECTS =

servidor: CMakeFiles/servidor.dir/servidor.cc.o
servidor: CMakeFiles/servidor.dir/build.make
servidor: /usr/local/lib/libopencv_dnn.so.4.0.0
servidor: /usr/local/lib/libopencv_ml.so.4.0.0
servidor: /usr/local/lib/libopencv_objdetect.so.4.0.0
servidor: /usr/local/lib/libopencv_shape.so.4.0.0
servidor: /usr/local/lib/libopencv_stitching.so.4.0.0
servidor: /usr/local/lib/libopencv_superres.so.4.0.0
servidor: /usr/local/lib/libopencv_videostab.so.4.0.0
servidor: /usr/local/lib/libopencv_photo.so.4.0.0
servidor: /usr/local/lib/libopencv_video.so.4.0.0
servidor: /usr/local/lib/libopencv_calib3d.so.4.0.0
servidor: /usr/local/lib/libopencv_features2d.so.4.0.0
servidor: /usr/local/lib/libopencv_flann.so.4.0.0
servidor: /usr/local/lib/libopencv_highgui.so.4.0.0
servidor: /usr/local/lib/libopencv_videoio.so.4.0.0
servidor: /usr/local/lib/libopencv_imgcodecs.so.4.0.0
servidor: /usr/local/lib/libopencv_imgproc.so.4.0.0
servidor: /usr/local/lib/libopencv_core.so.4.0.0
servidor: CMakeFiles/servidor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ebotti/Redes/videoTransfer/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable servidor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/servidor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/servidor.dir/build: servidor

.PHONY : CMakeFiles/servidor.dir/build

CMakeFiles/servidor.dir/requires: CMakeFiles/servidor.dir/servidor.cc.o.requires

.PHONY : CMakeFiles/servidor.dir/requires

CMakeFiles/servidor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/servidor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/servidor.dir/clean

CMakeFiles/servidor.dir/depend:
	cd /home/ebotti/Redes/videoTransfer/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ebotti/Redes/videoTransfer/src /home/ebotti/Redes/videoTransfer/src /home/ebotti/Redes/videoTransfer/src /home/ebotti/Redes/videoTransfer/src /home/ebotti/Redes/videoTransfer/src/CMakeFiles/servidor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/servidor.dir/depend
