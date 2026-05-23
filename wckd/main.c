#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/backend/session.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>

struct sample_state {
	struct wl_display *display;
	struct wl_listener new_output;
	struct wl_listener new_input;
	struct wlr_renderer *renderer;
	struct wlr_allocator *allocator;
	struct timespec last_frame;
	float color[4];
	int dec;
};

struct wckd_output {
	struct sample_state *sample;
	struct wlr_output *output;
	struct wl_listener frame;
	struct wl_listener destroy;
};

struct wckd_keyboard {
	struct sample_state *sample;
	struct wlr_keyboard *wlr_keyboard;
	struct wl_listener key;
	struct wl_listener destroy;
};

static void output_frame_notify(struct wl_listener *listener, void *data) {
	struct wckd_output *wckd_output =
		wl_container_of(listener, wckd_output, frame);
	struct sample_state *sample = wckd_output->sample;
	struct wlr_output *wlr_output = wckd_output->output;

	struct wlr_output_state state;
	wlr_output_state_init(&state);
	struct wlr_render_pass *pass = wlr_output_begin_render_pass(wlr_output, &state, NULL);
	wlr_render_pass_submit(pass);

	wlr_output_commit_state(wlr_output, &state);
	wlr_output_state_finish(&state);
}

static void output_remove_notify(struct wl_listener *listener, void *data) {
	struct wckd_output *wckd_output =
		wl_container_of(listener, wckd_output, destroy);
	wlr_log(WLR_DEBUG, "Output removed");
	wl_list_remove(&wckd_output->frame.link);
	wl_list_remove(&wckd_output->destroy.link);
	free(wckd_output);
}

static void new_output_notify(struct wl_listener *listener, void *data) {
	struct wlr_output *output = data;
	struct sample_state *sample =
		wl_container_of(listener, sample, new_output);

	wlr_output_init_render(output, sample->allocator, sample->renderer);

	struct wckd_output *wckd_output = calloc(1, sizeof(*wckd_output));
	wckd_output->output = output;
	wckd_output->sample = sample;
	wl_signal_add(&output->events.frame, &wckd_output->frame);
	wckd_output->frame.notify = output_frame_notify;
	wl_signal_add(&output->events.destroy, &wckd_output->destroy);
	wckd_output->destroy.notify = output_remove_notify;

	struct wlr_output_state state;
	wlr_output_state_init(&state);
	wlr_output_state_set_enabled(&state, true);
	struct wlr_output_mode *mode = wlr_output_preferred_mode(output);
	if (mode != NULL) {
		wlr_output_state_set_mode(&state, mode);
	}
	wlr_output_commit_state(output, &state);
	wlr_output_state_finish(&state);
}

static void keyboard_key_notify(struct wl_listener *listener, void *data) {
	struct wckd_keyboard *keyboard = wl_container_of(listener, keyboard, key);
	struct sample_state *sample = keyboard->sample;
	struct wlr_keyboard_key_event *event = data;
	uint32_t keycode = event->keycode + 8;
	const xkb_keysym_t *syms;
	int nsyms = xkb_state_key_get_syms(keyboard->wlr_keyboard->xkb_state,
			keycode, &syms);
	for (int i = 0; i < nsyms; i++) {
		xkb_keysym_t sym = syms[i];
		if (sym == XKB_KEY_Escape) {
			wl_display_terminate(sample->display);
		}
	}
}

static void keyboard_destroy_notify(struct wl_listener *listener, void *data) {
	struct wckd_keyboard *keyboard =
		wl_container_of(listener, keyboard, destroy);
	wl_list_remove(&keyboard->destroy.link);
	wl_list_remove(&keyboard->key.link);
	free(keyboard);
}

static void new_input_notify(struct wl_listener *listener, void *data) {
	struct wlr_input_device *device = data;
	struct sample_state *sample = wl_container_of(listener, sample, new_input);
	switch (device->type) {
	case WLR_INPUT_DEVICE_KEYBOARD:;
		struct wckd_keyboard *keyboard = calloc(1, sizeof(*keyboard));
		keyboard->wlr_keyboard = wlr_keyboard_from_input_device(device);
		keyboard->sample = sample;
		wl_signal_add(&device->events.destroy, &keyboard->destroy);
		keyboard->destroy.notify = keyboard_destroy_notify;
		wl_signal_add(&keyboard->wlr_keyboard->events.key, &keyboard->key);
		keyboard->key.notify = keyboard_key_notify;
		struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		if (!context) {
			wlr_log(WLR_ERROR, "Failed to create XKB context");
			exit(1);
		}
		struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, NULL,
			XKB_KEYMAP_COMPILE_NO_FLAGS);
		if (!keymap) {
			wlr_log(WLR_ERROR, "Failed to create XKB keymap");
			exit(1);
		}
		wlr_keyboard_set_keymap(keyboard->wlr_keyboard, keymap);
		xkb_keymap_unref(keymap);
		xkb_context_unref(context);
		break;
	default:
		break;
	}
}

int main(void) {
	wlr_log_init(WLR_DEBUG, NULL);
	struct wl_display *display = wl_display_create();
	struct sample_state state = {
		.color = { 1.0, 0.0, 0.0, 1.0 },
		.dec = 0,
		.last_frame = { 0 },
		.display = display
	};
	struct wlr_backend *backend = wlr_backend_autocreate(wl_display_get_event_loop(display), NULL);
	if (!backend) {
		exit(1);
	}

	state.renderer = wlr_renderer_autocreate(backend);
	state.allocator = wlr_allocator_autocreate(backend, state.renderer);

	wl_signal_add(&backend->events.new_output, &state.new_output);
	state.new_output.notify = new_output_notify;
	wl_signal_add(&backend->events.new_input, &state.new_input);
	state.new_input.notify = new_input_notify;
	clock_gettime(CLOCK_MONOTONIC, &state.last_frame);

	if (!wlr_backend_start(backend)) {
		wlr_log(WLR_ERROR, "Failed to start backend");
		wlr_backend_destroy(backend);
		exit(1);
	}
	wl_display_run(display);
	wl_display_destroy(display);
}
