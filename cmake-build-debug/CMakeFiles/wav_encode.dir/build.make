# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\CLion 2020.2.5\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\CLion 2020.2.5\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\projects\audio_encode

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\projects\audio_encode\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/wav_encode.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/wav_encode.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/wav_encode.dir/flags.make

CMakeFiles/wav_encode.dir/tuiliu_update.cpp.obj: CMakeFiles/wav_encode.dir/flags.make
CMakeFiles/wav_encode.dir/tuiliu_update.cpp.obj: CMakeFiles/wav_encode.dir/includes_CXX.rsp
CMakeFiles/wav_encode.dir/tuiliu_update.cpp.obj: ../tuiliu_update.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\projects\audio_encode\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/wav_encode.dir/tuiliu_update.cpp.obj"
	C:\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\wav_encode.dir\tuiliu_update.cpp.obj -c D:\projects\audio_encode\tuiliu_update.cpp

CMakeFiles/wav_encode.dir/tuiliu_update.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wav_encode.dir/tuiliu_update.cpp.i"
	C:\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\projects\audio_encode\tuiliu_update.cpp > CMakeFiles\wav_encode.dir\tuiliu_update.cpp.i

CMakeFiles/wav_encode.dir/tuiliu_update.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wav_encode.dir/tuiliu_update.cpp.s"
	C:\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\projects\audio_encode\tuiliu_update.cpp -o CMakeFiles\wav_encode.dir\tuiliu_update.cpp.s

CMakeFiles/wav_encode.dir/ptsdts.cpp.obj: CMakeFiles/wav_encode.dir/flags.make
CMakeFiles/wav_encode.dir/ptsdts.cpp.obj: CMakeFiles/wav_encode.dir/includes_CXX.rsp
CMakeFiles/wav_encode.dir/ptsdts.cpp.obj: ../ptsdts.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\projects\audio_encode\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/wav_encode.dir/ptsdts.cpp.obj"
	C:\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\wav_encode.dir\ptsdts.cpp.obj -c D:\projects\audio_encode\ptsdts.cpp

CMakeFiles/wav_encode.dir/ptsdts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wav_encode.dir/ptsdts.cpp.i"
	C:\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\projects\audio_encode\ptsdts.cpp > CMakeFiles\wav_encode.dir\ptsdts.cpp.i

CMakeFiles/wav_encode.dir/ptsdts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wav_encode.dir/ptsdts.cpp.s"
	C:\mingw-w64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\projects\audio_encode\ptsdts.cpp -o CMakeFiles\wav_encode.dir\ptsdts.cpp.s

# Object files for target wav_encode
wav_encode_OBJECTS = \
"CMakeFiles/wav_encode.dir/tuiliu_update.cpp.obj" \
"CMakeFiles/wav_encode.dir/ptsdts.cpp.obj"

# External object files for target wav_encode
wav_encode_EXTERNAL_OBJECTS =

wav_encode.exe: CMakeFiles/wav_encode.dir/tuiliu_update.cpp.obj
wav_encode.exe: CMakeFiles/wav_encode.dir/ptsdts.cpp.obj
wav_encode.exe: CMakeFiles/wav_encode.dir/build.make
wav_encode.exe: D:/opencv/opencv/build/mingw64_build/install/x64/mingw/lib/libopencv_highgui451.dll.a
wav_encode.exe: D:/opencv/opencv/build/mingw64_build/install/x64/mingw/lib/libopencv_videoio451.dll.a
wav_encode.exe: D:/opencv/opencv/build/mingw64_build/install/x64/mingw/lib/libopencv_imgcodecs451.dll.a
wav_encode.exe: D:/opencv/opencv/build/mingw64_build/install/x64/mingw/lib/libopencv_imgproc451.dll.a
wav_encode.exe: D:/opencv/opencv/build/mingw64_build/install/x64/mingw/lib/libopencv_core451.dll.a
wav_encode.exe: CMakeFiles/wav_encode.dir/linklibs.rsp
wav_encode.exe: CMakeFiles/wav_encode.dir/objects1.rsp
wav_encode.exe: CMakeFiles/wav_encode.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\projects\audio_encode\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable wav_encode.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\wav_encode.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/wav_encode.dir/build: wav_encode.exe

.PHONY : CMakeFiles/wav_encode.dir/build

CMakeFiles/wav_encode.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\wav_encode.dir\cmake_clean.cmake
.PHONY : CMakeFiles/wav_encode.dir/clean

CMakeFiles/wav_encode.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\projects\audio_encode D:\projects\audio_encode D:\projects\audio_encode\cmake-build-debug D:\projects\audio_encode\cmake-build-debug D:\projects\audio_encode\cmake-build-debug\CMakeFiles\wav_encode.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/wav_encode.dir/depend
