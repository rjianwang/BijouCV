# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/sony/Projects/bijou_cv/svm_demo_using_opencv

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build

# Include any dependencies generated for this target.
include bin/CMakeFiles/non_linear_svms.dir/depend.make

# Include the progress variables for this target.
include bin/CMakeFiles/non_linear_svms.dir/progress.make

# Include the compile flags for this target's objects.
include bin/CMakeFiles/non_linear_svms.dir/flags.make

bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o: bin/CMakeFiles/non_linear_svms.dir/flags.make
bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o: ../src/svm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o"
	cd /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/non_linear_svms.dir/svm.cpp.o -c /home/sony/Projects/bijou_cv/svm_demo_using_opencv/src/svm.cpp

bin/CMakeFiles/non_linear_svms.dir/svm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/non_linear_svms.dir/svm.cpp.i"
	cd /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sony/Projects/bijou_cv/svm_demo_using_opencv/src/svm.cpp > CMakeFiles/non_linear_svms.dir/svm.cpp.i

bin/CMakeFiles/non_linear_svms.dir/svm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/non_linear_svms.dir/svm.cpp.s"
	cd /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sony/Projects/bijou_cv/svm_demo_using_opencv/src/svm.cpp -o CMakeFiles/non_linear_svms.dir/svm.cpp.s

bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.requires:

.PHONY : bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.requires

bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.provides: bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.requires
	$(MAKE) -f bin/CMakeFiles/non_linear_svms.dir/build.make bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.provides.build
.PHONY : bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.provides

bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.provides.build: bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o


# Object files for target non_linear_svms
non_linear_svms_OBJECTS = \
"CMakeFiles/non_linear_svms.dir/svm.cpp.o"

# External object files for target non_linear_svms
non_linear_svms_EXTERNAL_OBJECTS =

bin/non_linear_svms: bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o
bin/non_linear_svms: bin/CMakeFiles/non_linear_svms.dir/build.make
bin/non_linear_svms: /usr/local/lib/libopencv_videostab.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_ts.a
bin/non_linear_svms: /usr/local/lib/libopencv_superres.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_stitching.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_contrib.so.2.4.9
bin/non_linear_svms: /usr/lib/i386-linux-gnu/libGLU.so
bin/non_linear_svms: /usr/lib/i386-linux-gnu/libGL.so
bin/non_linear_svms: /usr/local/lib/libopencv_nonfree.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_ocl.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_gpu.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_photo.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_objdetect.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_legacy.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_video.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_ml.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_calib3d.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_features2d.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_highgui.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_imgproc.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_flann.so.2.4.9
bin/non_linear_svms: /usr/local/lib/libopencv_core.so.2.4.9
bin/non_linear_svms: bin/CMakeFiles/non_linear_svms.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable non_linear_svms"
	cd /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/non_linear_svms.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
bin/CMakeFiles/non_linear_svms.dir/build: bin/non_linear_svms

.PHONY : bin/CMakeFiles/non_linear_svms.dir/build

bin/CMakeFiles/non_linear_svms.dir/requires: bin/CMakeFiles/non_linear_svms.dir/svm.cpp.o.requires

.PHONY : bin/CMakeFiles/non_linear_svms.dir/requires

bin/CMakeFiles/non_linear_svms.dir/clean:
	cd /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin && $(CMAKE_COMMAND) -P CMakeFiles/non_linear_svms.dir/cmake_clean.cmake
.PHONY : bin/CMakeFiles/non_linear_svms.dir/clean

bin/CMakeFiles/non_linear_svms.dir/depend:
	cd /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sony/Projects/bijou_cv/svm_demo_using_opencv /home/sony/Projects/bijou_cv/svm_demo_using_opencv/src /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin /home/sony/Projects/bijou_cv/svm_demo_using_opencv/build/bin/CMakeFiles/non_linear_svms.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : bin/CMakeFiles/non_linear_svms.dir/depend

