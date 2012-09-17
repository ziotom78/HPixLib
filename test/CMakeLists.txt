set(LIST_OF_TESTS
  angles_to_pixel
  bmp_projection
  map_io
  nest_to_ring
  npixel_to_nside
  nside_to_npixel
  pixel_to_angles
  query_disc
  ring_to_nest
  switch_order)

foreach(test ${LIST_OF_TESTS})
  include_directories(${PROJECT_SOURCE_DIR}/src)
  link_directories(${PROJECT_BINARY_DIR}/src)

  add_executable(${test} ${test}.c)
  target_link_libraries(${test} hpxlib)

  add_test(${test} ${test})
endforeach()
