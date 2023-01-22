cmake_minimum_required(VERSION 3.15)

project(tvwcommon CXX)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# Work around https://gitlab.kitware.com/cmake/cmake/-/issues/24209 util cmake 3.25.1
set (CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)

find_package(fmt CONFIG REQUIRED)
find_package(tvision CONFIG REQUIRED)

add_library(tvwcommon 
  "src/tvwcommon/srcview.cpp"
  "src/tvwcommon/form.cpp"
  "src/tvwcommon/inputline.cpp"
  "src/tvwcommon/listwindow.cpp"
)
target_compile_features(tvwcommon PRIVATE cxx_std_17)
target_link_libraries(tvwcommon PRIVATE 
  fmt::fmt 
  tvision::tvision
)
target_include_directories(tvwcommon PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
