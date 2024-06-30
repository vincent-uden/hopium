cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)
include(CMakePrintHelpers)

# Project name
project (Lesson_04 CXX)

# OpenCascade
find_package(OpenCASCADE)

# Qt
# find_package(Qt)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

include_directories(include)

link_directories(${libPath})
include_directories(${includePath})

file(GLOB_RECURSE SRCS src/*.cpp)

# Add executable
add_executable (Lesson_04 main.cpp ${SRCS})
add_executable (test test/test.cpp ${SRCS})

get_filename_component(libPath ./lib REALPATH)
get_filename_component(includePath ./include REALPATH)
get_filename_component(raylibPath ./lib/libraylib.so.4.5.0 REALPATH)

target_link_libraries(Lesson_04 ${raylibPath})
target_compile_options(Lesson_04 PRIVATE -Wswitch)
target_link_libraries(test ${raylibPath})

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  cmake_print_variables(LIB)
  target_link_libraries(Lesson_04 ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
  target_link_libraries(test ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
endforeach()



# Adjust runtime environment
# set_property(TARGET Lesson_04 PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
