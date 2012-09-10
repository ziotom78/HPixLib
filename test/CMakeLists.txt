set(LIST_OF_TESTS
  test_angles_to_pixel
  test_map_io
  test_nest_to_ring
  test_npixel_to_nside
  test_nside_to_npixel
  test_pixel_to_angles
  test_query_disc
  test_ring_to_nest
  test_switch_order)

foreach(test ${LIST_OF_TESTS})
  include_directories(${cHealpix_SOURCE_DIR}/src)
  link_directories(${cHealpix_BINARY_DIR}/src)

  add_executable(${test} ${test}.c)
  target_link_libraries(${test} chealpix)

  add_test(${test} ${test})
endforeach()
