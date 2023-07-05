list(APPEND tests
  array-utf8
  fixed32
  fixed64
  int
  int8
  int16
  int32
  int64
  uint
  uint8
  uint16
  uint32
  uint64
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(
    ${test}
    PRIVATE
      compact_static
  )

  target_include_directories(
    ${test}
    PRIVATE
      $<TARGET_PROPERTY:compact,INTERFACE_INCLUDE_DIRECTORIES>
  )

  add_test(
    NAME ${test}
    COMMAND ${test}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

  set_tests_properties(
    ${test}
    PROPERTIES
    TIMEOUT 30
  )
endforeach()
