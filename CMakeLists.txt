CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
IF(UNIX)
	SET (CMAKE_C_COMPILER             "clang")
	SET (CMAKE_C_FLAGS                "-Wall -std=c99")
	SET (CMAKE_C_FLAGS_DEBUG          "-g")
	SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
	SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

	SET (CMAKE_CXX_COMPILER             "clang++")
	SET (CMAKE_CXX_FLAGS                "-Wall -g")
	SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
	SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
	SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")


	SET (CMAKE_AR      "/usr/bin/llvm-ar")
	SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
	SET (CMAKE_NM      "/usr/bin/llvm-nm")
	SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
	SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
ENDIF(UNIX)


project(InvariantInferenceFramework)
add_definitions (-DVARS=2)
add_definitions (-D__PRT)

option(PRINT_ALL "Print All Message" OFF)
if(PRINT_ALL)
	add_definitions (-D__PRT_SVM_I)
	add_definitions (-D__PRT_QUERY)
endif(PRINT_ALL)


if(UNIX)
	set(Z3_ROOT "/usr" CACHE PATH "Root of Z3 distribution.")
	find_path(Z3_INCLUDE_DIR NAMES z3.h z3++.h PATHS ${Z3_ROOT}/include NO_DEFAULT_PATH)
	find_library(Z3_LIBRARY NAMES z3 PATHS ${Z3_ROOT}/lib NO_DEFAULT_PATH)

	mark_as_advanced(Z3_EXECUTABLE Z3_INCLUDE_DIR Z3_LIBRARY)
	find_program (Z3_EXECUTABLE
		NAMES z3 PATHS ${Z3_ROOT} PATH_SUFFIXES bin 
		DOC "z3 command line executable" NO_DEFAULT_PATH)
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
endif(UNIX)








file(GLOB HEADER "include/*.h")
source_group("Header Files" FILES ${HEADERS}) 

file(GLOB TEST "test/*.cpp")
source_group("Test" FILES ${TEST}) 

# add include dir and source dir into project
include_directories (include)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY(test DIR_TEST)

# genearte executable for each test...
foreach(test_src ${TEST})
	#MESSAGE(STATUS "Processing file ${test_src}")
	#string(REGEX REPLACE ".*/(.*).cpp" "/1" test_obj ${test_src})
	string(REGEX REPLACE ".cpp" "" tmp_obj ${test_src})
	string(REGEX REPLACE ".*/" "" test_obj ${tmp_obj})
	string(REGEX REPLACE "([0-9]*)_.*/" "/1" Vars ${test_obj})
	message(${tmp_obj} "\t" ${test_obj} "\t" ${Vars})
	ADD_EXECUTABLE(${test_obj} ${test_src} ${DIR_SRCS} ${HEADER})
	#MESSAGE(${test_obj}
	if(UNIX)
		target_link_libraries(${test_obj} ${Z3_LIBRARY})
	endif(UNIX)
endforeach(test_src)
