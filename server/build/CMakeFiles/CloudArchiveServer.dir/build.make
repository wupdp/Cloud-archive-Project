# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gabr/Projects/GitHub/Cloud-archive-Project/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gabr/Projects/GitHub/Cloud-archive-Project/server/build

# Include any dependencies generated for this target.
include CMakeFiles/CloudArchiveServer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/CloudArchiveServer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/CloudArchiveServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CloudArchiveServer.dir/flags.make

CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o: CMakeFiles/CloudArchiveServer.dir/flags.make
CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o: /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/main.cpp
CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o: CMakeFiles/CloudArchiveServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o -MF CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o.d -o CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o -c /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/main.cpp

CMakeFiles/CloudArchiveServer.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CloudArchiveServer.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/main.cpp > CMakeFiles/CloudArchiveServer.dir/src/main.cpp.i

CMakeFiles/CloudArchiveServer.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CloudArchiveServer.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/main.cpp -o CMakeFiles/CloudArchiveServer.dir/src/main.cpp.s

CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o: CMakeFiles/CloudArchiveServer.dir/flags.make
CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o: /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/server.cpp
CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o: CMakeFiles/CloudArchiveServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o -MF CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o.d -o CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o -c /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/server.cpp

CMakeFiles/CloudArchiveServer.dir/src/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CloudArchiveServer.dir/src/server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/server.cpp > CMakeFiles/CloudArchiveServer.dir/src/server.cpp.i

CMakeFiles/CloudArchiveServer.dir/src/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CloudArchiveServer.dir/src/server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/server.cpp -o CMakeFiles/CloudArchiveServer.dir/src/server.cpp.s

CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o: CMakeFiles/CloudArchiveServer.dir/flags.make
CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o: /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/socket.cpp
CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o: CMakeFiles/CloudArchiveServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o -MF CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o.d -o CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o -c /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/socket.cpp

CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/socket.cpp > CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.i

CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/socket.cpp -o CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.s

CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o: CMakeFiles/CloudArchiveServer.dir/flags.make
CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o: /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/ftp_commands.cpp
CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o: CMakeFiles/CloudArchiveServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o -MF CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o.d -o CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o -c /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/ftp_commands.cpp

CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/ftp_commands.cpp > CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.i

CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/ftp_commands.cpp -o CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.s

CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o: CMakeFiles/CloudArchiveServer.dir/flags.make
CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o: /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/utilities.cpp
CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o: CMakeFiles/CloudArchiveServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o -MF CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o.d -o CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o -c /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/utilities.cpp

CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/utilities.cpp > CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.i

CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabr/Projects/GitHub/Cloud-archive-Project/server/src/utilities.cpp -o CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.s

# Object files for target CloudArchiveServer
CloudArchiveServer_OBJECTS = \
"CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o" \
"CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o" \
"CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o" \
"CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o" \
"CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o"

# External object files for target CloudArchiveServer
CloudArchiveServer_EXTERNAL_OBJECTS =

CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/src/main.cpp.o
CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/src/server.cpp.o
CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/src/socket.cpp.o
CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/src/ftp_commands.cpp.o
CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/src/utilities.cpp.o
CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/build.make
CloudArchiveServer: CMakeFiles/CloudArchiveServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable CloudArchiveServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CloudArchiveServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CloudArchiveServer.dir/build: CloudArchiveServer
.PHONY : CMakeFiles/CloudArchiveServer.dir/build

CMakeFiles/CloudArchiveServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CloudArchiveServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CloudArchiveServer.dir/clean

CMakeFiles/CloudArchiveServer.dir/depend:
	cd /home/gabr/Projects/GitHub/Cloud-archive-Project/server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gabr/Projects/GitHub/Cloud-archive-Project/server /home/gabr/Projects/GitHub/Cloud-archive-Project/server /home/gabr/Projects/GitHub/Cloud-archive-Project/server/build /home/gabr/Projects/GitHub/Cloud-archive-Project/server/build /home/gabr/Projects/GitHub/Cloud-archive-Project/server/build/CMakeFiles/CloudArchiveServer.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/CloudArchiveServer.dir/depend

