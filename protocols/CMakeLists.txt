add_custom_command(OUTPUT wlr-layer-shell-protocol.c
  COMMAND wayland-scanner private-code 
  < ${CMAKE_CURRENT_SOURCE_DIR}/wlr-layer-shell-unstable-v1.xml
  > wlr-layer-shell-protocol.c
)

add_custom_command(OUTPUT wlr-layer-shell-client-protocol.h
  COMMAND wayland-scanner client-header 
  < ${CMAKE_CURRENT_SOURCE_DIR}/wlr-layer-shell-unstable-v1.xml
  > wlr-layer-shell-client-protocol.h
)

add_library(wlr_layer_shell STATIC wlr-layer-shell-protocol.c)
target_sources(wlr_layer_shell INTERFACE FILE_SET HEADERS 
  FILES wlr-layer-shell-client-protocol.h
)
export(TARGETS wlr_layer_shell)
