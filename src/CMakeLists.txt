set(chealpix_sources
  bitmap.c
  io.c
  map.c
  mem.c
  misc.c
  order_conversion.c
  positions.c
  query_disc.c)

add_definitions(-Wall --pedantic)

include_directories(${cHealpix_SOURCE_DIR}/src)
link_directories(${cHealpix_BINARY_DIR}/src)

add_library(chealpix ${chealpix_sources})

target_link_libraries(chealpix
  cfitsio z pthread m)

# Install directives
install(TARGETS chealpix
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES chpx.h DESTINATION include)
