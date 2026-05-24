#include "pairing/pairing-prompt-presenter.h"

#include "core/display.h"
#include "pairing/pairing-prompt-response.h"

typedef struct {
  guint ref_count;
  AdwAlertDialog *dialog;
  GtkEntry *entry;
  gboolean closed;
  PairingPromptResponse *response;
} PairingPrompt;

static PairingPrompt *
pairing_prompt_ref(PairingPrompt *prompt)
{
  prompt->ref_count++;
  return prompt;
}

static void
pairing_prompt_unref(PairingPrompt *prompt)
{
  if (!prompt)
    return;

  prompt->ref_count--;
  if (prompt->ref_count != 0)
    return;

  g_clear_pointer(&prompt->response, pairing_prompt_response_unref);
  g_clear_object(&prompt->dialog);
  g_free(prompt);
}

static void
pairing_prompt_close(gpointer handle_data)
{
  PairingPrompt *prompt = handle_data;

  if (!prompt || prompt->closed)
    return;

  prompt->closed = TRUE;
  if (prompt->dialog)
    adw_dialog_force_close(ADW_DIALOG(prompt->dialog));
}

static void
on_pairing_prompt_chosen(GObject *source_object, GAsyncResult *result, gpointer user_data)
{
  PairingPrompt *prompt = user_data;
  const char *response;

  if (prompt->closed) {
    pairing_prompt_unref(prompt);
    return;
  }

  prompt->closed = TRUE;
  response = adw_alert_dialog_choose_finish(ADW_ALERT_DIALOG(source_object), result);

  if (g_strcmp0(response, "accept") == 0) {
    const char *text = prompt->entry ? gtk_editable_get_text(GTK_EDITABLE(prompt->entry)) : "";

    pairing_prompt_response_accept(prompt->response, text);
  } else {
    pairing_prompt_response_reject(prompt->response);
  }

  pairing_prompt_unref(prompt);
}

PairingPromptHandle *
pairing_prompt_present(GtkWindow *window,
                           const char *heading,
                           const char *body,
                           GtkWidget *extra_child,
                           PairingAccept accept,
                           PairingEntryAccept entry_accept,
                           PairingReject reject,
                           gpointer response_data,
                           GDestroyNotify response_destroy)
{
  PairingPrompt *prompt;
  g_autofree char *safe_heading = NULL;
  g_autofree char *safe_body = NULL;

  if (!window)
    return NULL;

  gtk_window_present(window);

  prompt = g_new0(PairingPrompt, 1);
  prompt->ref_count = 1;
  safe_heading = display_dup_safe_text(heading, "Bluetooth pairing request");
  safe_body = display_dup_safe_text(body, "Confirm the Bluetooth pairing request.");
  prompt->dialog = ADW_ALERT_DIALOG(g_object_ref_sink(adw_alert_dialog_new(
    safe_heading ? safe_heading : "Bluetooth pairing request",
    safe_body ? safe_body : "Confirm the Bluetooth pairing request.")));
  gtk_widget_add_css_class(GTK_WIDGET(prompt->dialog), "bm-sidebar-dialog");
  prompt->entry = GTK_ENTRY(extra_child);
  prompt->response = pairing_prompt_response_new(accept,
                                                 entry_accept,
                                                 reject,
                                                 response_data,
                                                 response_destroy);

  adw_alert_dialog_add_response(prompt->dialog, "cancel", "Cancel");
  adw_alert_dialog_add_response(prompt->dialog, "accept", "Accept");
  adw_alert_dialog_set_close_response(prompt->dialog, "cancel");
  adw_alert_dialog_set_default_response(prompt->dialog, "accept");
  adw_alert_dialog_set_response_appearance(prompt->dialog, "accept", ADW_RESPONSE_SUGGESTED);
  if (extra_child)
    adw_alert_dialog_set_extra_child(prompt->dialog, extra_child);

  adw_alert_dialog_choose(prompt->dialog,
                          GTK_WIDGET(window),
                          NULL,
                          on_pairing_prompt_chosen,
                          pairing_prompt_ref(prompt));
  if (extra_child)
    gtk_widget_grab_focus(extra_child);

  return pairing_prompt_handle_new(prompt,
                                        pairing_prompt_close,
                                        (GDestroyNotify)pairing_prompt_unref);
}

PairingPromptHandle *
pairing_prompt_present_entry(GtkWindow *window,
                             const char *heading,
                             const char *body,
                             gboolean passkey,
                             PairingEntryAccept accept,
                             PairingReject reject,
                             gpointer response_data,
                             GDestroyNotify response_destroy)
{
  GtkWidget *entry = gtk_entry_new();
  PairingPromptHandle *handle;

  gtk_editable_set_text(GTK_EDITABLE(entry), "");
  gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry), passkey ? "000000" : "PIN code");
  if (passkey)
    gtk_entry_set_input_purpose(GTK_ENTRY(entry), GTK_INPUT_PURPOSE_DIGITS);

  handle = pairing_prompt_present(window,
                                  heading,
                                  body,
                                  entry,
                                  NULL,
                                  accept,
                                  reject,
                                  response_data,
                                  response_destroy);
  if (!handle)
    g_object_unref(entry);

  return handle;
}
