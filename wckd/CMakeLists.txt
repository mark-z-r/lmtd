add_executable(wckd main.c)

target_compile_definitions(wckd PUBLIC WLR_USE_UNSTABLE)

target_include_directories(wckd PUBLIC /usr/include/wlroots-0.19/
  /usr/include/pixman-1/)

target_link_libraries(wckd wlroots-0.19 wayland-server xkbcommon)
