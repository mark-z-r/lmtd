add_custom_command(OUTPUT xdg-shell-protocol.c
  COMMAND wayland-scanner private-code 
  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
  > xdg-shell-protocol.c
)

add_custom_command(OUTPUT xdg-shell-protocol.h
  COMMAND wayland-scanner client-header 
  < /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml 
  > xdg-shell-client-protocol.h
)

add_custom_target(xdg_shell_header DEPENDS xdg-shell-protocol.h)
add_executable(bgd main.c xdg-shell-protocol.c)

add_dependencies(bgd xdg_shell_header)
target_include_directories(bgd PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bgd wayland-client)
