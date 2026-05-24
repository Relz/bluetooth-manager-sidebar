# Bluetooth Manager Sidebar

## Project Shape

- C/GTK source under `src/` is the source of truth; Meson builds two executables: `build/src/bm-sidebar` CLI and `build/src/bm-sidebar-gui` helper.
- Root `bm-sidebar` is only a checkout launcher for `build/src/bm-sidebar`; packaged installs put the CLI under `/usr/bin` and the helper under `/usr/libexec/bm-sidebar/`.
- CLI parsing lives in `src/cli/cli.c`, process startup/helper exec lives in `src/cli/main.c`, the GUI helper entrypoint is `src/app/main.c`, and the Adw application is composed in `src/app/application.c`.
- UI construction is split between `src/sidebar/sidebar-window.c` and `src/app/application-pages.c`; BlueZ, OBEX, settings, paths, audio profile, and IPC integration live in dedicated `src/*/` modules.

## Runtime Integration

- Waybar invokes `/home/relz/.local/bin/bm-sidebar --toggle`; that wrapper execs this repo's `./bm-sidebar`.
- Keep the CLI GTK/libadwaita/BlueZ/OBEX/audio-free: `--help` and IPC attempts must not initialize GUI or live Bluetooth/audio services.
- CLI commands are mutually exclusive: `--toggle`, `--show`, `--hide`, `--quit`, `--reload-css`, `--background`; default is `--toggle`; `--hide`, `--quit`, and `--reload-css` are IPC-only.
- IPC uses a Unix socket named `bm-sidebar.sock` under `XDG_RUNTIME_DIR`, or `/tmp/bm-sidebar-$UID` if unset.
- IPC uses newline-framed commands with acknowledgement responses; `--background` verifies a responsive command server with `ping`.
- Preserve IPC runtime directory owner/type/permission/symlink checks, startup locking, stale-socket probing, and acknowledged responses.
- Settings save to `${XDG_CONFIG_HOME:-~/.config}/bm-sidebar/settings.json`; load falls back to legacy `bluetooth-sidebar/settings.json` only when the new file is absent; default received files directory is `~/Downloads`.
- CSS is loaded from this repo's `bm-sidebar.css` during checkout runs, from `/usr/share/bm-sidebar/bm-sidebar.css` when packaged, and user overrides from `${XDG_CONFIG_HOME:-~/.config}/bm-sidebar/bm-sidebar.css` can be reloaded with `--reload-css`.
- Output targeting comes from `BM_SIDEBAR_OUTPUT`, then Waybar's `WAYBAR_OUTPUT_NAME`, and is forwarded over IPC before showing the layer-shell panel.
- Gtk4LayerShell is required to show the sidebar; keep protocol/version guards for optional APIs and keyboard mode compatibility.
- Audio profile actions shell out to `pactl`; Bluetooth unblock recovery shells out to `rfkill unblock bluetooth`.

## BlueZ And OBEX Gotchas

- BlueZ state comes from ObjectManager on the system bus; OBEX file transfer uses `org.bluez.obex` on the session bus.
- The app registers a default BlueZ pairing agent at `/dev/relz/BmSidebar/Agent` with capability `KeyboardDisplay`.
- Discovery auto-stops after `BLUEZ_SCAN_TIMEOUT_SECONDS` in `src/bluez/bluez-discovery.c`.
- Pair, trust, connect, disconnect, remove, block, power, discoverable, audio profile, and file-transfer actions mutate live system Bluetooth/audio state.
- Ask before adding new behavior that blocks/unblocks, powers off, disconnects, removes, changes trust/pairing, changes audio profiles, or changes file-transfer policy unless the user explicitly requested it.

## Verification

- If `build/` is missing, run `meson setup build`; build with `meson compile -C build`.
- No test-suite, lint, formatter, or typecheck config is present; use focused checks after edits.
- Check CLI parsing without starting the UI using `./bm-sidebar --help`.
- Check the Waybar wrapper with `/home/relz/.local/bin/bm-sidebar --help`.
- In a Wayland graphical session, smoke-test the panel with `(./bm-sidebar --show & pid=$!; sleep 2; ./bm-sidebar --quit; wait $pid)`.
- Package metadata lives under `packaging/`; `.github/workflows/build-packages.yml` builds packages only for `v*.*.*` tags and syncs Meson project version from the tag.
- If adding packaged runtime files, update Meson install rules and `packaging/nfpm.yaml` together.
- If runtime dependencies change, update `README.md`, `packaging/nfpm.yaml`, and `packaging/aur/PKGBUILD.in` together.
