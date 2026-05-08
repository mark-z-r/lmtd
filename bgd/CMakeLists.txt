add_executable(bgd main.c)

target_include_directories(bgd PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(bgd wayland-client wlr_layer_shell)
