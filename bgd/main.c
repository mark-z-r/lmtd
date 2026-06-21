#include <wayland-client.h> 
#include <string.h>
#include <stdlib.h>
#include "wlr-layer-shell-client-protocol.h" // Generated.
#define _GNU_SOURCE 
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct bgd_state {
  struct wl_display *disp;
  struct wl_registry *reg;
  struct wl_compositor *comp;
  struct wl_shm *shm;
  struct wl_surface *wl_surf;
  struct zwlr_layer_shell_v1 *shell;
  struct zwlr_layer_surface_v1 *layer;
};

static void buf_release(void *data, struct wl_buffer *buf){
  wl_buffer_destroy(buf);
}

static const struct wl_buffer_listener buf_listener = {
  .release = buf_release,
};

static int get_image(uint32_t height, uint32_t width){
  open("/home/mark/.cache/bgd/bg-d.xrgb8888",O_RDWR);
}

static struct wl_buffer * share_image(struct bgd_state *state){
  int img_fd = get_image(800, 480);
  struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, memfd, bufsize);
  struct wl_buffer *wl_buf = wl_shm_pool_create_buffer(pool,0,800,480,800*4,WL_SHM_FORMAT_XRGB8888);
  wl_buffer_add_listener( wl_buf, &buf_listener, NULL);
  return wl_buf;
}

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface,
                                   uint32_t version) {
  struct bgd_state *state = data;
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    state->comp = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
  } else if (strcmp(interface, wl_shm_interface.name) == 0){
    state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name)==0){
    state->shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface,1); 
  }
}

static void reg_global_remove(
    void *data, struct wl_registry *reg, uint32_t name 
) {
  // do nothing
}

static void layer_conf(void *data, struct zwlr_layer_surface_v1 *surf, uint32_t serial, uint32_t width, uint32_t height){
  struct bgd_state *state = data;
  zwlr_layer_surface_v1_ack_configure(surf, serial);

  struct wl_buffer *buf = draw_image(state);
  wl_surface_attach(state->wl_surf,buf,0,0);
  wl_surface_commit(state->wl_surf); 
}

static void layer_close(void *data, struct zwlr_layer_surface_v1 *surf){
  // TODO
}

static const struct zwlr_layer_surface_v1_listener layer_listener={
  .configure = layer_conf,
  .closed = layer_close,
};

static const struct wl_registry_listener reg_listener = {
    .global = registry_handle_global,
    .global_remove = reg_global_remove,
};

int main(int argc, char *argv[]) {
  struct bgd_state state = {0};
  state.disp = wl_display_connect(NULL);
  state.reg = wl_display_get_registry(state.disp);
  wl_registry_add_listener(state.reg, &reg_listener, &state);
  wl_display_roundtrip(state.disp);

  state.wl_surf = wl_compositor_create_surface(state.comp);
  state.layer = zwlr_layer_shell_v1_get_layer_surface(state.shell, state.wl_surf, NULL, 0, "background");
  zwlr_layer_surface_v1_set_anchor(
      state.layer,
      ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP  | ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT 
    | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
  );
  zwlr_layer_surface_v1_set_exclusive_zone(state.layer, -1);
  zwlr_layer_surface_v1_set_size(state.layer, 0,0);
  zwlr_layer_surface_v1_add_listener(state.layer, &layer_listener, &state);
  wl_surface_commit(state.wl_surf);
  while(wl_display_dispatch(state.disp)){
  }
  return 0;
}
