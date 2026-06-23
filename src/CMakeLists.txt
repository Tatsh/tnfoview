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
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                                         ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE ${DEBUG_FLAGS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Widgets)

string(TIMESTAMP YEAR "%Y")
configure_file(sh.tat.${CMAKE_PROJECT_NAME}.metainfo.xml.in
               sh.tat.${CMAKE_PROJECT_NAME}.metainfo.xml @ONLY)

if(WIN32)
  configure_file("version.rc.in" version.rc @ONLY)
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE version.rc)
  set_source_files_properties(version.rc PROPERTIES COMPILE_DEFINITIONS
                                                    $<$<CONFIG:Release>:NDEBUG=1>)
endif()

if(APPLE)
  # Temporary: generate an icns file from the PNG icons until a proper macOS icon is made.
  set(ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.iconset")
  set(ICNS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.icns")
  file(MAKE_DIRECTORY ${ICONSET_DIR})
  add_custom_command(
    OUTPUT ${ICNS_FILE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_16.png
            ${ICONSET_DIR}/icon_16x16.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_32.png
            ${ICONSET_DIR}/icon_16x16@2x.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_32.png
            ${ICONSET_DIR}/icon_32x32.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_64.png
            ${ICONSET_DIR}/icon_32x32@2x.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_128.png
            ${ICONSET_DIR}/icon_128x128.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_256.png
            ${ICONSET_DIR}/icon_128x128@2x.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_256.png
            ${ICONSET_DIR}/icon_256x256.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_512.png
            ${ICONSET_DIR}/icon_256x256@2x.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_512.png
            ${ICONSET_DIR}/icon_512x512.png
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_512@2x.png
            ${ICONSET_DIR}/icon_512x512@2x.png
    COMMAND iconutil -c icns -o ${ICNS_FILE} ${ICONSET_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_16.png
            ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_32.png
            ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_64.png
            ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_128.png
            ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_256.png
            ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_512.png
            ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_512@2x.png
    COMMENT "Generating ${CMAKE_PROJECT_NAME}.icns from PNG icons."
    VERBATIM)
  add_custom_target(${CMAKE_PROJECT_NAME}_icns ALL DEPENDS ${ICNS_FILE})
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES OUTPUT_NAME ${DISPLAY_NAME}
               MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_BUNDLE_NAME ${DISPLAY_NAME}
               MACOSX_BUNDLE_ICON_FILE "${CMAKE_PROJECT_NAME}.icns"
               MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
               MACOSX_BUNDLE_GUI_IDENTIFIER "sh.tat.${CMAKE_PROJECT_NAME}"
               MACOSX_BUNDLE_COPYRIGHT "Copyright (c) ${YEAR} ${DISPLAY_NAME} authors."
               MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  add_dependencies(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_icns)
  set(deploy_tool_options_arg "")
  set(executable_path "$<TARGET_FILE_NAME:${CMAKE_PROJECT_NAME}>.app")
  set(executable_dir ".")
elseif(WIN32)
  set(deploy_tool_options_arg --no-compiler-runtime)
  set(executable_path "$<TARGET_FILE_NAME:${CMAKE_PROJECT_NAME}>")
  set(executable_dir "${CMAKE_INSTALL_BINDIR}")
endif()
if(APPLE OR WIN32)
  qt_generate_deploy_script(
    TARGET
    ${CMAKE_PROJECT_NAME}
    OUTPUT_SCRIPT
    deploy_script
    CONTENT
    [[
qt_deploy_runtime_dependencies(
  EXECUTABLE "${executable_dir}/${executable_path}"
  DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
  GENERATE_QT_CONF
  VERBOSE)
]])
endif()

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME_DEPENDENCY_SET ${CMAKE_PROJECT_NAME}-deps
        BUNDLE DESTINATION .)
if(APPLE OR WIN32)
  install(SCRIPT ${deploy_script})
endif()
if(APPLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.icns
          DESTINATION "$<TARGET_FILE_NAME:${CMAKE_PROJECT_NAME}>.app/Contents/Resources")
  install(
    FILES ../LICENSE.txt ../README.md ../CHANGELOG.md ../SECURITY.md ../CITATION.cff
    DESTINATION "$<TARGET_FILE_NAME:${CMAKE_PROJECT_NAME}>.app/Contents/Resources")
elseif(WIN32)
  install(FILES ../LICENSE.txt ../README.md ../CHANGELOG.md ../SECURITY.md TYPE DOC)
  install(FILES ../CITATION.cff DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME})
  get_filename_component(_compiler_bin_dir ${CMAKE_CXX_COMPILER} DIRECTORY)
  install(
    RUNTIME_DEPENDENCY_SET
    ${CMAKE_PROJECT_NAME}-deps
    PRE_EXCLUDE_REGEXES
    [=[api-ms-]=]
    [=[ext-ms-]=]
    [[kernel32\.dll]]
    POST_EXCLUDE_REGEXES
    [=[.*system32\/.*\.dll]=]
    DIRECTORIES
    ${_compiler_bin_dir}
    $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)
else()
  # Linux: install binary, desktop entry, icons, AppStream metainfo, and the man page.
  install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.desktop.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/sh.tat.${CMAKE_PROJECT_NAME}.desktop" @ONLY)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/sh.tat.${CMAKE_PROJECT_NAME}.desktop"
          DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
  foreach(
    size
    16
    32
    48
    64
    96
    128
    256
    512)
    install(
      FILES "${CMAKE_CURRENT_SOURCE_DIR}/icons/icon_${size}.png"
      DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${size}x${size}/apps"
      RENAME sh.tat.${CMAKE_PROJECT_NAME}.png)
  endforeach()
  install(FILES ../README.md TYPE DOC)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sh.tat.${CMAKE_PROJECT_NAME}.metainfo.xml
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
endif()

# CPack configuration (Windows NSIS installer).
if(WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/extra.nsh.in ${CMAKE_BINARY_DIR}/extra.nsh @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/extra-uninstall.nsh.in
                 ${CMAKE_BINARY_DIR}/extra-uninstall.nsh @ONLY)
  string(REPLACE "/" "\\\\" CMAKE_BINARY_DIR_ESCAPED ${CMAKE_BINARY_DIR})
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
      "  !include \\\"${CMAKE_BINARY_DIR_ESCAPED}\\\\extra.nsh\\\""
      PARENT_SCOPE)
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
      "  !include \\\"${CMAKE_BINARY_DIR_ESCAPED}\\\\extra-uninstall.nsh\\\""
      PARENT_SCOPE)
  set(CPACK_NSIS_DEFINES
      "RequestExecutionLevel user
!define /date CURRENT_YEAR \\\"%Y\\\"
VIProductVersion \\\"${PROJECT_VERSION}.0\\\"
VIAddVersionKey \\\"ProductName\\\" \\\"${DISPLAY_NAME}\\\"
VIAddVersionKey \\\"CompanyName\\\" \\\"Tatsh\\\"
VIAddVersionKey \\\"FileDescription\\\" \\\"${DISPLAY_NAME} installer.\\\"
VIAddVersionKey \\\"FileVersion\\\" \\\"${PROJECT_VERSION}\\\"
VIAddVersionKey \\\"ProductVersion\\\" \\\"${PROJECT_VERSION}.0\\\"
VIAddVersionKey \\\"LegalCopyright\\\" \\\"Copyright © \\\${CURRENT_YEAR} Tatsh.\\\"
VIAddVersionKey \\\"Comments\\\" \\\"${PROJECT_HOMEPAGE_URL}\\\""
      PARENT_SCOPE)
  set(CPACK_NSIS_INSTALL_ROOT
      "$PROGRAMFILES64"
      PARENT_SCOPE)
  set(CPACK_NSIS_CONTACT
      "https://bsky.app/profile/tatsh.bsky.social"
      PARENT_SCOPE)
  set(CPACK_NSIS_INSTALLED_ICON_NAME
      "bin\\\\${CMAKE_PROJECT_NAME}.exe"
      PARENT_SCOPE)
  set(CPACK_PACKAGE_EXECUTABLES
      "${CMAKE_PROJECT_NAME};${DISPLAY_NAME}"
      PARENT_SCOPE)
  set(CPACK_CREATE_DESKTOP_LINKS
      "${CMAKE_PROJECT_NAME};${DISPLAY_NAME}"
      PARENT_SCOPE)
  set(CPACK_NSIS_MANIFEST_DPI_AWARE
      ON
      PARENT_SCOPE)
  set(CPACK_NSIS_DISPLAY_NAME
      ${DISPLAY_NAME}
      PARENT_SCOPE)
  set(CPACK_NSIS_PACKAGE_NAME
      ${DISPLAY_NAME}
      PARENT_SCOPE)
  set(CPACK_NSIS_MODIFY_PATH
      ON
      PARENT_SCOPE)
  set(CPACK_NSIS_URL_INFO_ABOUT
      ${PROJECT_HOMEPAGE_URL}/issues
      PARENT_SCOPE)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY
      ${DISPLAY_NAME}
      PARENT_SCOPE)
endif()
