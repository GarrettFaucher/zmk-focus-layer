#!/bin/sh
# Install the zmk-focus-layer host side for the current user (KDE Plasma 6, Wayland).
# Safe to re-run.
set -eu

repo=$(realpath "$(dirname "$0")/..")

mkdir -p "$HOME/.local/bin"
ln -sfn "$repo/host/zmk-focus-layer" "$HOME/.local/bin/zmk-focus-layer"

install -Dm644 "$repo/host/systemd/zmk-focus-layer.service" \
    "$HOME/.config/systemd/user/zmk-focus-layer.service"
systemctl --user daemon-reload
systemctl --user enable zmk-focus-layer.service
systemctl --user restart zmk-focus-layer.service

if kpackagetool6 --type=KWin/Script -s zmkfocuslayer >/dev/null 2>&1; then
    kpackagetool6 --type=KWin/Script -u "$repo/host/kwin"
else
    kpackagetool6 --type=KWin/Script -i "$repo/host/kwin"
fi
kwriteconfig6 --file kwinrc --group Plugins --key zmkfocuslayerEnabled true
qdbus6 org.kde.KWin /KWin reconfigure

# Scroll Lock only toggles (and lights LEDs) under Wayland when xkb maps it to a
# real modifier.
options=$(kreadconfig6 --file kxkbrc --group Layout --key Options)
case ",$options," in
    *,scrolllock:mod3,*) ;;
    *)
        kwriteconfig6 --file kxkbrc --group Layout --key Options "${options:+$options,}scrolllock:mod3"
        kwriteconfig6 --file kxkbrc --group Layout --key ResetOldOptions true
        dbus-send --session --type=signal /Layouts org.kde.keyboard.reloadConfig
        echo "zmk-focus-layer: added xkb option scrolllock:mod3; log out and back in if Scroll Lock doesn't light"
        ;;
esac
