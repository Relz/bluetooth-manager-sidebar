#pragma once

#define APP_ID "dev.relz.BmSidebar"
#define APP_NAME "Bluetooth Manager Sidebar"
#define COMMAND_NAME "bm-sidebar"
#define SOCKET_NAME "bm-sidebar.sock"
#define WIDTH 440
#define LAYER_SHELL_REQUIRED_MESSAGE "Gtk4LayerShell support is required to show Bluetooth Manager Sidebar"

#ifndef SOURCE_DIR
#define SOURCE_DIR "."
#endif

#ifndef INSTALLED_CSS_PATH
#define INSTALLED_CSS_PATH "/usr/share/bm-sidebar/bm-sidebar.css"
#endif

#ifndef INSTALLED_GUI_PATH
#define INSTALLED_GUI_PATH "/usr/libexec/bm-sidebar/bm-sidebar-gui"
#endif
