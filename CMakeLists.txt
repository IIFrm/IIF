cmake_minimum_required (VERSION 2.8)
set(Nv 2)
project(InvariantInferenceFramework)
set(PRECISION 3)

add_definitions (-D__PRT)
#add_definitions (-D__PRT_Z3SOLVE)
#add_definitions (-D__PRT_TRACE)
#add_definitions (-D__PRT_EQUATION)
#add_definitions (-D__PRT_SVM_I)
#add_definitions (-D__PRT_QUERY)
#option(PRINT_ALL "Print All Message" ON)
option(PRINT_ALL "Print All Message" OFF)
if(PRINT_ALL)
	add_definitions (-D__PRT_SVM_I)
	add_definitions (-D__PRT_QUERY)
	add_definitions (-D__PRT_TRACE)
	add_definitions (-D__PRT_EQUATION)
endif(PRINT_ALL)


if(UNIX)
	set(CMAKE_C_COMPILER             "clang")
	set(CMAKE_C_FLAGS                "-Wall -std=c99")
	set(CMAKE_C_FLAGS_DEBUG          "-g")
	set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	set(CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

	set(CMAKE_CXX_COMPILER             "clang++")
	set(CMAKE_CXX_FLAGS                "-Wall -g")
	#set(CMAKE_CXX_FLAGS                "-Wall -g -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG          "-g")
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

	set(CMAKE_AR      "/usr/bin/llvm-ar")
	set(CMAKE_LINKER  "/usr/bin/llvm-ld")
	set(CMAKE_NM      "/usr/bin/llvm-nm")
	set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
	set(CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
endif(UNIX)

# config for Z3 solver...
if(UNIX)
	set(Z3_ROOT "/usr" CACHE PATH "Root of Z3 distribution.")
	#find_library(Z3_LIBRARY NAMES z3 PATHS ${Z3_ROOT}/lib NO_DEFAULT_PATH)
	find_library(Z3_LIBRARY NAMES z3 PATHS ${Z3_ROOT}/lib /usr/local/lib)
endif(UNIX)
if(WIN32)
	set(Z3_ROOT "C:/Program Files/z3" CACHE PATH "Root of Z3 distribution.")
	find_library(Z3_LIBRARY NAMES libz3.lib PATHS ${Z3_ROOT}/bin NO_DEFAULT_PATH)
endif(WIN32)

#find_path(Z3_INCLUDE_DIR NAMES z3.h z3++.h PATHS ${Z3_ROOT}/include NO_DEFAULT_PATH)
find_path(Z3_INCLUDE_DIR NAMES z3.h z3++.h PATHS ${Z3_ROOT}/include /usr/local/include)
mark_as_advanced(Z3_EXECUTABLE Z3_INCLUDE_DIR Z3_LIBRARY)
find_program (Z3_EXECUTABLE
	NAMES z3 PATHS ${Z3_ROOT} PATH_SUFFIXES bin 
	#DOC "z3 command line executable" NO_DEFAULT_PATH)
	DOC "z3 command line executable" /usr/local/bin)
mark_as_advanced(Z3_EXECUTABLE)

if (Z3_EXECUTABLE)
	execute_process (COMMAND ${Z3_EXECUTABLE} -version
		OUTPUT_VARIABLE z3_version
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if (z3_version MATCHES "^Z3 version [0-9]")
		string (REPLACE "Z3 version " "" Z3_VERSION_STRING ${z3_version})
	endif()
endif()
mark_as_advanced(Z3_VERSION_STRING)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(Z3
	REQUIRED_VARS Z3_LIBRARY Z3_INCLUDE_DIR Z3_EXECUTABLE
	VERSION_VAR Z3_VERSION_STRING)
include_directories(${Z3_INCLUDE_DIR})
#endif(UNIX)



file(GLOB HEADER "include/*.h")
source_group("Header Files" FILES ${HEADERS}) 

file(GLOB TEST "test/*.cpp")
source_group("Test" FILES ${TEST}) 

# add include dir and source dir into project
configure_file(config.h.in ../include/config.h)
include_directories (include)
#include_directories(.)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY(test DIR_TEST)

add_executable(PV1 test/PV1.cpp ${DIR_SRCS} ${HEADER})
target_link_libraries(PV1 ${Z3_LIBRARY})
