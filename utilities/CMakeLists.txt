################################################################################
# Check if we can link to Cairo or not

find_package(PkgConfig)

pkg_check_modules(CAIRO REQUIRED cairo)

################################################################################

if(GCC)
  add_definitions(-Wall --pedantic)
endif()

include_directories(${PROJECT_SOURCE_DIR}/src ${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})

add_executable(map2fig map2fig.c gopt.c)
target_link_libraries(map2fig hpix ${CAIRO_LIBRARIES})
