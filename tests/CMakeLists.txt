find_package(GTest QUIET)
if(TARGET GTest::GTest)
  message("GTest has been installed previously")
  include_directories(${GTEST_INCLUDE_DIRS})

else()
  message("no target GTest, Cloning into your build dir......")

  include(FetchContent)
  # Download and configure Catch2 for the tests
  FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG master
    )

  FetchContent_GetProperties(gtest)
  if (NOT gtest_POPULATED)
    FetchContent_Populate(gtest)
    add_subdirectory(${gtest_SOURCE_DIR} ${gtest_BINARY_DIR})
    list(APPEND CMAKE_MODULE_PATH ${gtest_SOURCE_DIR}/contrib)
    # include(gtest)
  endif()

endif()

enable_testing()

add_executable(inf inf.cpp)

target_link_libraries(inf gtest gtest_main)

include_directories( ${CMAKE_SOURCE_DIR}/includes )

set_target_properties(inf PROPERTIES CXX_STANDARD 20)

add_test(NAME test COMMAND inf)
