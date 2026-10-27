function report(w) {
    callDBus("dev.garrettfaucher.ZmkFocusLayer", "/dev/garrettfaucher/ZmkFocusLayer",
             "dev.garrettfaucher.ZmkFocusLayer", "ActiveWindowChanged",
             String(w ? w.pid : 0), String(w ? w.resourceClass : ""));
}
workspace.windowActivated.connect(report);
report(workspace.activeWindow);
