{ lib
, stdenv
, meson
, ninja
, pkg-config
, wrapGAppsHook4
, glib
, gtk4
, libadwaita
, json-glib
, gtk4-layer-shell
, pulseaudio
, util-linux
}:

stdenv.mkDerivation (finalAttrs: {
  pname = "bm-sidebar";
  version = "0.1.0";

  src = lib.cleanSourceWith {
    src = ./.;
    filter = path: type:
      let
        base = baseNameOf path;
      in
      !(base == "build" || base == "dist" || base == "package-root" || lib.hasPrefix "build-" base || base == ".git" || base == "result" || lib.hasPrefix "result-" base);
  };

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
    wrapGAppsHook4
  ];

  buildInputs = [
    glib
    gtk4
    libadwaita
    json-glib
    gtk4-layer-shell
  ];

  preFixup = ''
    gappsWrapperArgs+=(
      --prefix PATH : "${lib.makeBinPath [ pulseaudio util-linux ]}"
    )
  '';

  meta = with lib; {
    description = "Native GTK4/libadwaita Bluetooth sidebar for Wayland desktops";
    homepage = "https://github.com/thefoodiee/bluetooth-manager-sidebar";
    license = licenses.gpl3Plus;
    platforms = platforms.linux;
    mainProgram = "bm-sidebar";
  };
})
