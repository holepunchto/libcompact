list(APPEND tests
  array-utf8
  fixed32
  fixed64
  int
  int8
  int16
  int24
  int32
  int40
  int48
  int56
  int64
  uint
  uint8
  uint16
  uint24
  uint32
  uint40
  uint48
  uint56
  uint64
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(
    ${test}
    PRIVATE
      compact_static
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
