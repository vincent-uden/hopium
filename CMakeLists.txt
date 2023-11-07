cmake_minimum_required (VERSION 3.0.0 FATAL_ERROR)
include(CMakePrintHelpers)

# Project name
project (Lesson_04 CXX)

# OpenCascade
find_package(OpenCASCADE)

# Qt
# find_package(Qt)

set(CMAKE_BUILD_TYPE Debug)

# Configure C++ compiler's includes dir
include_directories ( SYSTEM ${OpenCASCADE_INCLUDE_DIR} )

include_directories(include)

# Add executable
add_executable (Lesson_04 main.cpp Ui.cpp Renderer.cpp Colorscheme.cpp)
add_executable (test test.cpp Ui.cpp Renderer.cpp Colorscheme.cpp)

# Add linker options
foreach (LIB ${OpenCASCADE_LIBRARIES})
  cmake_print_variables(LIB)
  target_link_libraries(Lesson_04 ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
  target_link_libraries(test ${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so)
endforeach()

target_link_libraries(Lesson_04 /opt/sources/lib/libraylib.so.4.5.0)
target_link_libraries(test /opt/sources/lib/libraylib.so.4.5.0)

# Adjust runtime environment
# set_property(TARGET Lesson_04 PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<$<CONFIG:DEBUG>:${OpenCASCADE_BINARY_DIR}d>$<$<NOT:$<CONFIG:DEBUG>>:${OpenCASCADE_BINARY_DIR}>;%PATH%")
