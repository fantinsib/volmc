cmake_minimum_required(VERSION 3.20)
project(volmc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ----------------------------
# Core library
# ----------------------------
add_library(volmc
    src/schemes/qe/qe.cpp
    src/schemes/euler/eulerheston.cpp
    src/schemes/euler/euler.cpp
    src/engine/montecarlo.cpp
    src/models/dupire.cpp
    src/models/heston.cpp
    src/models/blackscholes.cpp
    src/models/vasicek.cpp
    src/types/date.cpp
    src/types/simulationresult.cpp
    src/options/optioncontract.cpp
    src/surface/local_vol.cpp
    src/instruments/instrument.cpp
    src/pricing/pricer.cpp
)

target_include_directories(volmc
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# PIC helps when linking into a python module (safe default)
set_target_properties(volmc PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Warnings (portable-ish)
if (MSVC)
    target_compile_options(volmc PRIVATE /W4)
else()
    target_compile_options(volmc PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ----------------------------
# Catch2 (tests)
# ----------------------------
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(volmc_tests
    tests/test_cpp/test_models.cpp
    tests/test_cpp/test_schemes/test_euler.cpp
    tests/test_cpp/test_schemes/test_qe.cpp
    tests/test_cpp/test_engine/test_montecarlo.cpp
    tests/test_cpp/test_types/test_date.cpp
    tests/test_cpp/test_types/test_simulationresult.cpp
    tests/test_cpp/test_options/test_europeanoptions.cpp
    tests/test_cpp/test_surface/test_local_vol.cpp
    tests/test_cpp/test_options/test_pricer.cpp
    tests/test_cpp/test_options/test_barrier.cpp
    
)

target_link_libraries(volmc_tests
    PRIVATE
        volmc
        Catch2::Catch2WithMain
)

target_include_directories(volmc_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(NAME volmc_tests COMMAND volmc_tests)

# ----------------------------
# pybind11 module
# ----------------------------
cmake_policy(SET CMP0148 NEW)

find_package(Python REQUIRED COMPONENTS Interpreter Development)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.11.1
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(_volmc
  src/bindings/bindings.cpp
  src/bindings/types_bindings.cpp
  src/bindings/models_bindings.cpp
  src/bindings/schemes_bindings.cpp
  src/bindings/engines_bindings.cpp
  src/bindings/surface_bindings.cpp
  src/bindings/option_bindings.cpp
  src/bindings/pricer_bindings.cpp
)


install(TARGETS _volmc
    LIBRARY DESTINATION volmc
    ARCHIVE DESTINATION volmc
    RUNTIME DESTINATION volmc
)

target_link_libraries(_volmc PRIVATE volmc)


# --------------
# OpenMP
# --------------

find_package(OpenMP)

if(NOT OpenMP_CXX_FOUND AND APPLE)
  message(STATUS "OpenMP not found by CMake, trying manual flags for AppleClang + libomp...")

  set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")  
  set(OpenMP_CXX_LIB_NAMES "omp")
  set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")

  include_directories("/opt/homebrew/opt/libomp/include")
endif()

find_package(OpenMP REQUIRED)


target_link_libraries(volmc PRIVATE OpenMP::OpenMP_CXX)
