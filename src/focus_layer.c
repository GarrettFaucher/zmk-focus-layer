/*
 * Copyright (c) 2026 Garrett Faucher
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/*
 * Only LED transitions move the layer. Caps/Num changes and repeated reports
 * leave it alone, so a manual &tog of the same layer holds until the host
 * flips the LED again. Endpoint switches report the new host's LEDs through
 * the same event, so the new host's state wins.
 */
static bool last_lit;

static int focus_layer_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    bool lit = ev->indicators & BIT(CONFIG_ZMK_FOCUS_LAYER_INDICATOR_BIT);
    if (lit == last_lit) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    last_lit = lit;

    int ret = lit ? zmk_keymap_layer_activate(CONFIG_ZMK_FOCUS_LAYER_LAYER)
                  : zmk_keymap_layer_deactivate(CONFIG_ZMK_FOCUS_LAYER_LAYER);
    if (ret < 0) {
        LOG_WRN("focus-layer: %s layer %d failed: %d", lit ? "activate" : "deactivate",
                CONFIG_ZMK_FOCUS_LAYER_LAYER, ret);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(focus_layer, focus_layer_listener);
ZMK_SUBSCRIPTION(focus_layer, zmk_hid_indicators_changed);
