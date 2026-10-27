# zmk-focus-layer

Turn a ZMK keymap layer on while the host's Scroll Lock LED is lit, and have
the Linux desktop light Scroll Lock while a game window is focused. The result:
the keyboard sits on its game layer while you play, drops back to base when you
alt-tab out, and returns when you alt-tab back.

## Firmware module

Add the module to your `config/west.yml`:

```yaml
  remotes:
    - name: garrettfaucher
      url-base: https://github.com/garrettfaucher
  projects:
    - name: zmk-focus-layer
      remote: garrettfaucher
      revision: main
```

and enable it on the central (or non-split) build only, for example in
`build.yaml`:

```yaml
cmake-args: -DCONFIG_ZMK_FOCUS_LAYER=y
```

| Kconfig | Default | Meaning |
| --- | --- | --- |
| `CONFIG_ZMK_FOCUS_LAYER` | `n` | Enable the module. Selects `CONFIG_ZMK_HID_INDICATORS`. |
| `CONFIG_ZMK_FOCUS_LAYER_LAYER` | `1` | Layer ID to drive. |
| `CONFIG_ZMK_FOCUS_LAYER_INDICATOR_BIT` | `2` | HID LED bit to follow (0 Num, 1 Caps, 2 Scroll, 3 Compose, 4 Kana). |

The layer only changes when the LED changes. A manual `&tog` of the same layer
stays in effect until the host next flips the LED. Switching endpoints (USB/BLE
or BT profile) applies the new host's LED state.

Enabling HID indicators adds an LED output report to the HID descriptor. Hosts
paired over Bluetooth may need the keyboard removed and re-paired to see it.

Built against ZMK `v0.3`. ZMK `v0.4` changes `zmk_keymap_layer_activate()` and
`zmk_keymap_layer_deactivate()` to take a `locking` argument; the module will
need to pass `false` there.

## Host (KDE Plasma 6, Wayland)

Requirements: Python 3 with `python-gobject` and `python-evdev`, GameMode, write
access to `/dev/uinput` (for example via the `input` group), `kpackagetool6`,
`kwriteconfig6`, `kreadconfig6`, `qdbus6`.

```sh
host/install.sh
```

This installs:

- `~/.local/bin/zmk-focus-layer` (symlink): a daemon that owns
  `dev.garrettfaucher.ZmkFocusLayer` on the session bus. A window counts as a
  game when its process or one of its ancestors is registered with GameMode
  (launch games with `gamemoderun`). It toggles Scroll Lock by injecting
  `KEY_SCROLLLOCK` through a uinput keyboard named `zmk-focus-layer`.
- `zmk-focus-layer.service`, a systemd user unit tied to the graphical session.
- The KWin script `zmkfocuslayer`, which reports every focus change to the
  daemon.
- The xkb option `scrolllock:mod3`, without which Scroll Lock doesn't light
  LEDs under Wayland. KWin may only pick it up after logging out and back in.

Logs: `journalctl --user -u zmk-focus-layer`.
