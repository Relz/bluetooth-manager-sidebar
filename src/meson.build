glib_dep = dependency('glib-2.0', version: '>=2.76')
gio_dep = dependency('gio-2.0', version: '>=2.76')
gio_unix_dep = dependency('gio-unix-2.0', version: '>=2.76')
gtk_dep = dependency('gtk4', version: '>=4.10')
adw_dep = dependency('libadwaita-1', version: '>=1.4')
json_glib_dep = dependency('json-glib-1.0')
layer_shell_dep = dependency('gtk4-layer-shell-0')

installed_css_path = join_paths(get_option('prefix'), get_option('datadir'), 'bm-sidebar', 'bm-sidebar.css')
installed_gui_path = join_paths(get_option('prefix'), get_option('libexecdir'), 'bm-sidebar', 'bm-sidebar-gui')

native_args = [
  '-DSOURCE_DIR="' + meson.project_source_root() + '"',
  '-DINSTALLED_CSS_PATH="' + installed_css_path + '"',
  '-DINSTALLED_GUI_PATH="' + installed_gui_path + '"',
]

cli_sources = files(
  'cli/cli.c',
  'cli/main.c',
)

app_sources = files(
  'app/application-actions.c',
  'app/application-adapter-actions.c',
  'app/application-adapter-info-model.c',
  'app/application-adapter-info-page.c',
  'app/application-audio-actions.c',
  'app/application-audio-adapter.c',
  'app/application-audio-commands.c',
  'app/application-audio-page-loader.c',
  'app/application-audio-profile-model.c',
  'app/application-audio-runtime.c',
  'app/application-bluetooth-commands.c',
  'app/application-bluez-adapter.c',
  'app/application-bluez-runtime.c',
  'app/application-chrome-state.c',
  'app/application-chrome.c',
  'app/application-command-runtime.c',
  'app/application-commands.c',
  'app/application-data-access.c',
  'app/application-device-actions.c',
  'app/application-device-audio-section-actions.c',
  'app/application-device-audio-section-model.c',
  'app/application-device-audio-section-view.c',
  'app/application-device-audio-section.c',
  'app/application-device-battery-section.c',
  'app/application-device-collection.c',
  'app/application-device-details-section.c',
  'app/application-device-file-section.c',
  'app/application-device-identity-section.c',
  'app/application-device-presentation.c',
  'app/application-device-property-display.c',
  'app/application-device-removal-dialog.c',
  'app/application-device-removal-flow.c',
  'app/application-device-removal-section.c',
  'app/application-device-security-section.c',
  'app/application-device-settings-context.c',
  'app/application-device-settings-page.c',
  'app/application-device-view.c',
  'app/application-file-actions.c',
  'app/application-file-commands.c',
  'app/application-layer-shell-policy.c',
  'app/application-obex-adapter.c',
  'app/application-obex-notification-actions.c',
  'app/application-obex-policy-resolver.c',
  'app/application-obex-policy.c',
  'app/application-obex-runtime.c',
  'app/application-obex-status-section.c',
  'app/application-overview-actions.c',
  'app/application-overview-model.c',
  'app/application-overview-page-model.c',
  'app/application-overview-page.c',
  'app/application-overview-presentation.c',
  'app/application-overview-presenter.c',
  'app/application-overview-view.c',
  'app/application-page-controller.c',
  'app/application-page-renderer.c',
  'app/application-page-status.c',
  'app/application-page-utils.c',
  'app/application-pages.c',
  'app/application-pairing-adapter.c',
  'app/application-pairing-device-resolver.c',
  'app/application-pairing-presenter.c',
  'app/application-send-file-dialog.c',
  'app/application-service-accessors.c',
  'app/application-services.c',
  'app/application-settings-actions.c',
  'app/application-settings-bindings.c',
  'app/application-settings-commands.c',
  'app/application-settings-feedback.c',
  'app/application-settings-model.c',
  'app/application-settings-page.c',
  'app/application-settings-runtime.c',
  'app/application-settings-service.c',
  'app/application-sidebar-visibility.c',
  'app/application-styles.c',
  'app/application-window-access.c',
  'app/application-window-callbacks.c',
  'app/application-window-content.c',
  'app/application-window-initial-render.c',
  'app/application.c',
)

audio_sources = files(
  'audio/audio-profile-cache.c',
  'audio/audio-profile-load-manager.c',
  'audio/audio-profile-load-task.c',
  'audio/audio-profile-loader.c',
  'audio/audio-profile-messages.c',
  'audio/audio-profile-model.c',
  'audio/audio-profile-operations.c',
  'audio/audio-profile-parser.c',
  'audio/audio-profile-query.c',
  'audio/audio-profile-repository.c',
  'audio/audio-profile-set-manager.c',
  'audio/audio-profile-set-task.c',
  'audio/audio-profile-setter.c',
  'audio/audio-profiles.c',
  'audio/pactl-runner.c',
)

bluez_sources = files(
  'bluez/bluez-adapter-method-call.c',
  'bluez/bluez-adapter-policy.c',
  'bluez/bluez-adapter-power-recovery.c',
  'bluez/bluez-adapter-power-state.c',
  'bluez/bluez-adapter-power-workflow.c',
  'bluez/bluez-adapter-report.c',
  'bluez/bluez-adapter.c',
  'bluez/bluez-battery-cache.c',
  'bluez/bluez-cache-applier.c',
  'bluez/bluez-cache-query.c',
  'bluez/bluez-cache-selection.c',
  'bluez/bluez-cache-selector.c',
  'bluez/bluez-cache-status.c',
  'bluez/bluez-cache-updater.c',
  'bluez/bluez-cache.c',
  'bluez/bluez-client-adapter-service.c',
  'bluez/bluez-client-adapter.c',
  'bluez/bluez-client-device-service.c',
  'bluez/bluez-client-device.c',
  'bluez/bluez-client-discovery.c',
  'bluez/bluez-client-pairing-service.c',
  'bluez/bluez-client-pairing.c',
  'bluez/bluez-client-query-service.c',
  'bluez/bluez-client-query.c',
  'bluez/bluez-client-services.c',
  'bluez/bluez-client.c',
  'bluez/bluez-dbus-variants.c',
  'bluez/bluez-device-activation.c',
  'bluez/bluez-device-connection-commands.c',
  'bluez/bluez-device-connection-power.c',
  'bluez/bluez-device-connection-report.c',
  'bluez/bluez-device-connection-tracker.c',
  'bluez/bluez-device-connection.c',
  'bluez/bluez-device-display.c',
  'bluez/bluez-device-method-call.c',
  'bluez/bluez-device-pair-step.c',
  'bluez/bluez-device-pair-trust-connect-report.c',
  'bluez/bluez-device-pair-trust-connect.c',
  'bluez/bluez-device-power-adapter.c',
  'bluez/bluez-device-power-pending.c',
  'bluez/bluez-device-power-report.c',
  'bluez/bluez-device-power-state.c',
  'bluez/bluez-device-power.c',
  'bluez/bluez-device-properties.c',
  'bluez/bluez-device-property-operation.c',
  'bluez/bluez-device-property-report.c',
  'bluez/bluez-device-removal-report.c',
  'bluez/bluez-device-removal.c',
  'bluez/bluez-discovery-reporting.c',
  'bluez/bluez-discovery-state.c',
  'bluez/bluez-discovery-timer.c',
  'bluez/bluez-discovery.c',
  'bluez/bluez-messages.c',
  'bluez/bluez-object-manager-signals.c',
  'bluez/bluez-object-manager.c',
  'bluez/bluez-object-path.c',
  'bluez/bluez-object-store.c',
  'bluez/bluez-operation-call.c',
  'bluez/bluez-operation-messages.c',
  'bluez/bluez-operation-reporter.c',
  'bluez/bluez-operation-result.c',
  'bluez/bluez-pairing-agent-contract.c',
  'bluez/bluez-pairing-agent-dbus.c',
  'bluez/bluez-pairing-agent-manager.c',
  'bluez/bluez-pairing-agent-object.c',
  'bluez/bluez-pairing-agent-prompts.c',
  'bluez/bluez-pairing-agent-registration.c',
  'bluez/bluez-pairing-agent-response.c',
  'bluez/bluez-pairing-agent-session.c',
  'bluez/bluez-pairing-agent.c',
  'bluez/bluez-pairing-client-adapter.c',
  'bluez/bluez-pairing-prompts.c',
  'bluez/bluez-pairing-response-dbus.c',
  'bluez/bluez-pairing-response-format.c',
  'bluez/bluez-pairing-response-validation.c',
  'bluez/bluez-properties.c',
  'bluez/bluez-property-action-runner.c',
  'bluez/bluez-property-mutation.c',
  'bluez/bluez-property-set-call.c',
  'bluez/bluez-rfkill.c',
  'bluez/bluez-state-errors.c',
  'bluez/bluez-state-object-events.c',
  'bluez/bluez-state.c',
)

command_client_sources = files(
  'command/command-client.c',
  'command/command-codec.c',
  'command/command-socket-path.c',
  'command/sidebar-command.c',
  'command/startup-command-router.c',
)

command_server_sources = files(
  'command/command-ipc.c',
  'command/command-socket.c',
)

command_sources = command_client_sources + command_server_sources

core_cli_sources = files(
  'core/runtime-paths.c',
  'core/target-output.c',
)

core_sources = files(
  'core/asset-paths.c',
  'core/config-paths.c',
  'core/display.c',
  'core/runtime-paths.c',
  'core/target-output.c',
  'core/timed-subprocess.c',
  'core/user-paths.c',
)

obex_sources = files(
  'obex/obex-agent-dispatch.c',
  'obex/obex-agent-handler.c',
  'obex/obex-agent-lifecycle.c',
  'obex/obex-agent.c',
  'obex/obex-incoming-active-authorization.c',
  'obex/obex-incoming-authorization-acceptance.c',
  'obex/obex-incoming-authorization-flow.c',
  'obex/obex-incoming-authorization.c',
  'obex/obex-incoming-controller.c',
  'obex/obex-incoming-notifications.c',
  'obex/obex-incoming-storage.c',
  'obex/obex-incoming-transfer.c',
  'obex/obex-notifications.c',
  'obex/obex-notifier.c',
  'obex/obex-outgoing-controller.c',
  'obex/obex-outgoing-dbus.c',
  'obex/obex-outgoing-notifications.c',
  'obex/obex-outgoing-request.c',
  'obex/obex-outgoing-session-events.c',
  'obex/obex-outgoing-session.c',
  'obex/obex-outgoing-status.c',
  'obex/obex-outgoing-transfer-registration.c',
  'obex/obex-policy.c',
  'obex/obex-received-directory.c',
  'obex/obex-receiver.c',
  'obex/obex-sender.c',
  'obex/obex-service-agent.c',
  'obex/obex-service-authorization.c',
  'obex/obex-service-components.c',
  'obex/obex-service-context.c',
  'obex/obex-service-lifecycle-steps.c',
  'obex/obex-service-lifecycle.c',
  'obex/obex-service-monitor.c',
  'obex/obex-service-notification.c',
  'obex/obex-service-registry.c',
  'obex/obex-service-session.c',
  'obex/obex-service-status.c',
  'obex/obex-service-transfer.c',
  'obex/obex-service.c',
  'obex/obex-transfer-details.c',
  'obex/obex-transfer-monitor.c',
  'obex/obex-transfer-paths.c',
  'obex/obex-transfer-registry.c',
  'obex/obex-transfer-storage.c',
)

pairing_sources = files(
  'pairing/pairing-prompt-presenter.c',
  'pairing/pairing-prompt-response.c',
  'pairing/pairing-prompt-session.c',
)

settings_sources = files(
  'settings/settings-json-codec.c',
  'settings/settings-store.c',
  'settings/settings.c',
)

sidebar_sources = files(
  'sidebar/sidebar-chrome.c',
  'sidebar/sidebar-window-chrome-proxy.c',
  'sidebar/sidebar-window-panel.c',
  'sidebar/sidebar-window-shell.c',
  'sidebar/sidebar-window-state.c',
  'sidebar/sidebar-window-toast.c',
  'sidebar/sidebar-window.c',
)

bm_sidebar_sources = (
  cli_sources +
  command_client_sources +
  core_cli_sources
)

bm_sidebar_gui_sources = (
  files('app/main.c') +
  app_sources +
  audio_sources +
  bluez_sources +
  command_sources +
  core_sources +
  obex_sources +
  pairing_sources +
  settings_sources +
  sidebar_sources
)

executable(
  'bm-sidebar',
  bm_sidebar_sources,
  dependencies: [glib_dep],
  c_args: native_args,
  include_directories: include_directories('.'),
  install: true,
  install_dir: get_option('bindir'),
)

executable(
  'bm-sidebar-gui',
  bm_sidebar_gui_sources,
  dependencies: [
    layer_shell_dep,
    glib_dep,
    gio_dep,
    gio_unix_dep,
    gtk_dep,
    adw_dep,
    json_glib_dep,
  ],
  c_args: native_args,
  include_directories: include_directories('.'),
  install: true,
  install_dir: join_paths(get_option('libexecdir'), 'bm-sidebar'),
)

install_data(
  '../bm-sidebar.css',
  install_dir: join_paths(get_option('datadir'), 'bm-sidebar'),
)
