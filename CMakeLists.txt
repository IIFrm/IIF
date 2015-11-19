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



OPTION(PRINT_ALL "Print All Message" ON)
#IF(PRINT_ALL)
#	ADD_DEFINITIONS (-D__PRT)
#ENDIF(PRINT_ALL)

## add new macro definition
ADD_DEFINITIONS (-D__PRT)

#SET(CMAKE_BUILD_TYPE "Debug")  
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")  
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
PROJECT(InvariantInferenceFramework)

#MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
#MESSAGE(STATUS "This is SOURCE dir " ${PROJECT_SOURCE_DIR})

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


INCLUDE_DIRECTORIES(include)
FILE(GLOB HEADERS "include/*.h")
SOURCE_GROUP("Header Files" FILES ${HEADERS}) 
#add_executable(${MODULE_NAME} ${SOURCES} ${CURRENT_HEADERS})




#include_directories(include/program)
#aux_source_directory(include DIR_SRCS)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY(examples DIR_SRCS)

ADD_EXECUTABLE(IIF_learn targets/iif_learn.cpp ${DIR_SRCS} ${HEADERS})
target_link_libraries(IIF_learn ${Z3_LIBRARY})

ADD_EXECUTABLE(z3_driver targets/z3_driver.cpp ${DIR_SRCS} ${HEADERS})
target_link_libraries(z3_driver ${Z3_LIBRARY})
#ADD_EXECUTABLE(svm_learn target/main.cpp ${DIR_SRCS} ${HEADERS})
#ADD_EXECUTABLE(perceptron_learn target/main_perceptron.cpp ${DIR_SRCS} ${HEADERS})
