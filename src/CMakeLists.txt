if(CMAKE_BUILD_TYPE STREQUAL Debug AND (CMAKE_CXX_COMPILER_ID STREQUAL GNU
                                        OR CMAKE_CXX_COMPILER_ID STREQUAL Clang))
  set(DEBUG_FLAGS -ggdb -Wall -Werror -Wextra -Wno-unknown-pragmas)
endif()

qt_add_executable(
  ${CMAKE_PROJECT_NAME}
  WIN32
  MACOSX_BUNDLE
  cp437.c
  cp437.h
  main.cpp
  nfoviewer.cpp
  nfoviewer.h
  settingsdialog.cpp
  settingsdialog.h
  resources.qrc)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${DEBUG_FLAGS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Widgets)
