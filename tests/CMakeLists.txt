find_package(Qt6 REQUIRED COMPONENTS Test)

set(TNFOVIEW_SRC_DIR ${CMAKE_SOURCE_DIR}/src)

function(tnfoview_add_test test_name test_src extra_sources extra_libs)
  add_executable(${test_name} ${test_src} ${extra_sources})
  target_include_directories(${test_name} PRIVATE ${TNFOVIEW_SRC_DIR})
  target_link_libraries(${test_name} PRIVATE Qt6::Core Qt6::Test ${extra_libs})
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# cp437test
tnfoview_add_test(cp437test ${CMAKE_CURRENT_SOURCE_DIR}/cp437test.cpp ${TNFOVIEW_SRC_DIR}/cp437.c "")
