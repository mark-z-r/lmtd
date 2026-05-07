#include <wayland-client.h>
#include <string.h>
#include <stdlib.h>
#include "xdg-shell-client-protocol.h"
#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
struct bgd_state {
  struct wl_display *disp;
  struct wl_registry *reg;
  struct wl_compositor *comp;
  struct wl_shm *shm;
  struct xdg_wm_base *wm;
  struct wl_surface *wl_surf;
  struct xdg_surface *xdg_surf;
  struct xdg_toplevel *top;  
};

static void buf_release(void *data, struct wl_buffer *buf){
  wl_buffer_destroy(buf);
}

static const struct wl_buffer_listener buf_listener = {
  .release = buf_release,
};

static struct wl_buffer * draw_image(struct bgd_state *state){
  int memfd = open("/home/mark/.cache/bgd/bg-d.xrgb8888",O_RDWR);
  int32_t bufsize = 4 * 800 * 480;
  ftruncate(memfd, bufsize);
  struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, memfd, bufsize);
  struct wl_buffer *wl_buf = wl_shm_pool_create_buffer(pool,0,800,480,800*4,WL_SHM_FORMAT_XRGB8888);
  void *buf = mmap(NULL, bufsize, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0);
  wl_buffer_add_listener( wl_buf, &buf_listener, NULL);
  return wl_buf;
}

static void xdg_surf_conf(void * data, struct xdg_surface *surf, uint32_t serial){
  struct bgd_state *state = data;
  xdg_surface_ack_configure(surf, serial);

  struct wl_buffer *buf = draw_image(state);
  wl_surface_attach(state->wl_surf,buf,0,0);
  wl_surface_commit(state->wl_surf);

}

static const struct xdg_surface_listener xdg_surf_listener = {
  .configure = xdg_surf_conf,
};

static void wm_ping(void *data, struct xdg_wm_base *wm, uint32_t serial ){
  xdg_wm_base_pong(wm,serial);
}

static const struct xdg_wm_base_listener wm_listener = {
  .ping  = wm_ping,
};

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface,
                                   uint32_t version) {
  struct bgd_state *state = data;
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    state->comp = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
  } else if (strcmp(interface, wl_shm_interface.name) == 0){
    state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    state->wm = wl_registry_bind(registry,name, &xdg_wm_base_interface,1);
    xdg_wm_base_add_listener(state->wm,&wm_listener,state);
  }
}

static void reg_global_remove(
    void *data, struct wl_registry *reg, uint32_t name 
) {
  // do nothing
}


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
  state.xdg_surf = xdg_wm_base_get_xdg_surface(state.wm, state.wl_surf);
  xdg_surface_add_listener(state.xdg_surf, &xdg_surf_listener, &state);
  state.top = xdg_surface_get_toplevel(state.xdg_surf);
  xdg_toplevel_set_title(state.top, "bgd");
  wl_surface_commit(state.wl_surf);
  while(wl_display_dispatch(state.disp)){
  }
  return 0;
}
