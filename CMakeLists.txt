cmake_minimum_required(VERSION 3.16)
project(SuperJupiterChannelAlign VERSION 0.1.0 LANGUAGES CXX)

set(_superjupiter_channel_align_top_level OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(_superjupiter_channel_align_top_level ON)
endif()
option(SUPERJUPITER_CHANNEL_ALIGN_BUILD_TESTS "Build ChannelAlign tests" ${_superjupiter_channel_align_top_level})
option(SUPERJUPITER_CHANNEL_ALIGN_BUILD_EXAMPLES "Build ChannelAlign host example" ${_superjupiter_channel_align_top_level})
option(SUPERJUPITER_CHANNEL_ALIGN_INSTALL "Generate ChannelAlign install rules" ${_superjupiter_channel_align_top_level})

add_library(SuperJupiterChannelAlign INTERFACE)
add_library(SuperJupiter::ChannelAlign ALIAS SuperJupiterChannelAlign)
set_target_properties(SuperJupiterChannelAlign PROPERTIES EXPORT_NAME ChannelAlign)
target_include_directories(SuperJupiterChannelAlign INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_features(SuperJupiterChannelAlign INTERFACE cxx_std_11)

if(SUPERJUPITER_CHANNEL_ALIGN_BUILD_TESTS)
  enable_testing()
  add_executable(superjupiter-channel-align-tests tests/test.cpp)
  target_link_libraries(superjupiter-channel-align-tests PRIVATE SuperJupiter::ChannelAlign)
  add_test(NAME superjupiter-channel-align-tests COMMAND superjupiter-channel-align-tests)
  add_executable(superjupiter-channel-align-namespace-test tests/namespace_isolation.cpp)
  target_link_libraries(superjupiter-channel-align-namespace-test PRIVATE SuperJupiter::ChannelAlign)
  add_test(NAME superjupiter-channel-align-namespace-test COMMAND superjupiter-channel-align-namespace-test)
endif()
if(SUPERJUPITER_CHANNEL_ALIGN_BUILD_EXAMPLES)
  add_executable(superjupiter-channel-align-replay tools/replay.cpp)
  target_link_libraries(superjupiter-channel-align-replay PRIVATE SuperJupiter::ChannelAlign)
endif()
if(SUPERJUPITER_CHANNEL_ALIGN_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_superjupiter_channel_align_package_dir lib/cmake/SuperJupiterChannelAlign)
  install(TARGETS SuperJupiterChannelAlign EXPORT SuperJupiterChannelAlignTargets)
  install(FILES src/SuperJupiterChannelAlign.h DESTINATION include)
  install(EXPORT SuperJupiterChannelAlignTargets NAMESPACE SuperJupiter::
    DESTINATION ${_superjupiter_channel_align_package_dir})
  configure_package_config_file(cmake/SuperJupiterChannelAlignConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterChannelAlignConfig.cmake
    INSTALL_DESTINATION ${_superjupiter_channel_align_package_dir})
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterChannelAlignConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion ARCH_INDEPENDENT)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterChannelAlignConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterChannelAlignConfigVersion.cmake
    DESTINATION ${_superjupiter_channel_align_package_dir})
  install(FILES LICENSE DESTINATION share/licenses/SuperJupiterChannelAlign)
endif()
