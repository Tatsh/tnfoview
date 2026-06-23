cmake_minimum_required(VERSION 3.25)
project(
  tnfoview
  VERSION 0.1.0
  LANGUAGES C CXX
  DESCRIPTION "A simple NFO viewer."
  HOMEPAGE_URL "https://github.com/Tatsh/tnfoview")

include(FeatureSummary)
include(GNUInstallDirs)

set(DISPLAY_NAME "Tatsh NFO Viewer")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_DOCS "Build documentation." OFF)
option(BUILD_DOCS_ONLY "Build documentation only." OFF)
option(INSTALL_DOCS "Install documentation." OFF)

if(BUILD_DOCS OR BUILD_DOCS_ONLY)
  set(DOXYGEN_PROJECT_NAME "${DISPLAY_NAME}")
  set(DOXYGEN_GENERATE_TAGFILE "${CMAKE_BINARY_DIR}/html/${CMAKE_PROJECT_NAME}.tags")
  set(DOXYGEN_INTERACTIVE_SVG YES)
  set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
  set(DOXYGEN_TAGFILES
      ./.docs/cppreference-doxygen-web.tag.xml=https://en.cppreference.com/w/
      ./.docs/qtcore.tags=http://doc.qt.io/qt-6/ ./.docs/qtgui.tags=http://doc.qt.io/qt-6/
      ./.docs/qtwidgets.tags=http://doc.qt.io/qt-6/)
  find_package(Doxygen REQUIRED COMPONENTS dot)
  doxygen_add_docs(docs README.md src ALL)
  if(INSTALL_DOCS)
    install(
      DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION})
  endif()
endif()

if(BUILD_DOCS_ONLY)
  return()
endif()

option(COVERAGE "Enable code coverage (GCC/Clang only)." OFF)
if(COVERAGE)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage)
    add_link_options(--coverage)
  else()
    message(WARNING "COVERAGE is only supported with GCC or Clang.")
  endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets CONFIG)
set_package_properties(Qt6Widgets PROPERTIES TYPE REQUIRED)

add_compile_definitions(
  QT_NO_CAST_FROM_ASCII
  QT_NO_CAST_FROM_BYTEARRAY
  QT_NO_CAST_TO_ASCII
  QT_NO_SIGNALS_SLOTS_KEYWORDS
  QT_NO_URL_CAST_FROM_STRING
  QT_STRICT_ITERATORS
  QT_USE_FAST_OPERATOR_PLUS
  QT_USE_QSTRINGBUILDER
  VERSION="${CMAKE_PROJECT_VERSION}")

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)

option(BUILD_TESTS "Build tests." OFF)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

set(CPACK_PACKAGE_VENDOR "Tatsh")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
if(DEFINED CPACK_SYSTEM_NAME AND NOT CPACK_SYSTEM_NAME STREQUAL "")
  set(__cpack_system_name ${CPACK_SYSTEM_NAME})
else()
  set(__cpack_system_name ${CMAKE_SYSTEM_NAME})
  if(__cpack_system_name MATCHES "Windows")
    if(CMAKE_GENERATOR_PLATFORM)
      string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" __cpack_winarch)
    else()
      string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" __cpack_winarch)
    endif()
    if(__cpack_winarch MATCHES "^(arm64|aarch64)$")
      set(__cpack_system_name winarm64)
    elseif(__cpack_winarch MATCHES "^(amd64|x86_64|x64)$" OR CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(__cpack_system_name win64)
    else()
      set(__cpack_system_name win32)
    endif()
  elseif(APPLE)
    if(CMAKE_APPLE_SILICON_PROCESSOR)
      set(__cpack_system_name Darwin-arm64)
    else()
      set(__cpack_system_name Darwin-x86_64)
    endif()
  endif()
endif()
set(CPACK_THREADS 0)
set(CPACK_PACKAGE_FILE_NAME
    ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${__cpack_system_name}-${CMAKE_BUILD_TYPE})

if(WIN32)
  set(CPACK_GENERATOR NSIS ZIP)
elseif(APPLE)
  set(CPACK_GENERATOR DragNDrop ZIP)
else()
  set(CPACK_GENERATOR ZIP)
endif()
include(CPack)
