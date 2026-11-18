cmake_minimum_required(VERSION 3.24)

project(fp VERSION 1.0.0 LANGUAGES C)

add_library(${PROJECT_NAME} INTERFACE)

add_library(FP::fp ALIAS fp)

target_compile_features(fp INTERFACE c_std_11)

target_include_directories(fp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

  $<INSTALL_INTERFACE:include>
)

