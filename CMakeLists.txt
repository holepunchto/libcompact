cmake_minimum_required(VERSION 3.25)

project(compact C)

if(NOT TARGET utf)
  add_subdirectory(vendor/libutf EXCLUDE_FROM_ALL)
endif()

add_library(compact OBJECT)

set_target_properties(
  compact
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  compact
  INTERFACE
    include/compact.h
  PRIVATE
    src/array.c
    src/fixed32.c
    src/fixed64.c
    src/float32.c
    src/float64.c
    src/int.c
    src/int8.c
    src/int16.c
    src/int32.c
    src/int64.c
    src/uint.c
    src/uint8.c
    src/uint8array.c
    src/uint16.c
    src/uint32.c
    src/uint64.c
    src/utf8.c
    src/zig-zag.h
)

target_include_directories(
  compact
  PUBLIC
    include
    $<TARGET_PROPERTY:utf,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(compact_shared SHARED $<TARGET_OBJECTS:compact>)

set_target_properties(
  compact_shared
  PROPERTIES
  OUTPUT_NAME compact
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

add_library(compact_static STATIC $<TARGET_OBJECTS:compact>)

set_target_properties(
  compact_static
  PROPERTIES
  OUTPUT_NAME compact
  PREFIX lib
)

install(TARGETS compact_shared compact_static)

install(FILES include/compact.h DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test)
endif()
