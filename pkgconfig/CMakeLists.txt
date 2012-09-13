set(pkg_config_prefix ${CMAKE_INSTALL_PREFIX})
set(pkg_config_exec_prefix ${CMAKE_INSTALL_PREFIX}/bin)
set(pkg_config_libdir ${CMAKE_INSTALL_PREFIX}/lib)
set(pkg_config_sharedlibdir ${CMAKE_INSTALL_PREFIX}/lib)
set(pkg_config_includedir ${CMAKE_INSTALL_PREFIX}/include)

configure_file(libchealpix.pc.in libchealpix.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libchealpix.pc
  DESTINATION lib/pkgconfig)
