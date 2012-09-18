set(hpxlib_sources
  bitmap.c
  io.c
  map.c
  mem.c
  misc.c
  order_conversion.c
  positions.c
  query_disc.c)

add_definitions(-Wall --pedantic)

include_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_BINARY_DIR}/src)

add_library(hpix ${hpxlib_sources})

target_link_libraries(hpix
  cfitsio z pthread m)

# Install directives
install(TARGETS hpix
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES hpix.h DESTINATION include)
