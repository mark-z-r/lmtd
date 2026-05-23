add_custom_command(OUTPUT wlr-layer-shell-client-protocol.h
  COMMAND wayland-scanner client-header 
   ${CMAKE_SOURCE_DIR}/protocols/wlr-layer-shell-unstable-v1.xml
   wlr-layer-shell-client-protocol.h
)

add_custom_command(OUTPUT wlr-layer-shell-protocol.c
  COMMAND wayland-scanner private-code 
  < ${CMAKE_SOURCE_DIR}/protocols/wlr-layer-shell-unstable-v1.xml
  > wlr-layer-shell-protocol.c
)

add_custom_command(OUTPUT xdg-shell-protocol.c
  COMMAND wayland-scanner private-code 
  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
  > xdg-shell-protocol.c
)

add_custom_target(wlr_layer_shell_header 
  DEPENDS  wlr-layer-shell-client-protocol.h
)

add_executable(bgd main.c wlr-layer-shell-protocol.c xdg-shell-protocol.c)

add_dependencies(bgd wlr_layer_shell_header)
target_include_directories(bgd PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bgd wayland-client)

install(TARGETS bgd)
