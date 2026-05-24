#include "bluez/bluez-device-pair-trust-connect.h"

#include "bluez/bluez-device-pair-step.h"
#include "bluez/bluez-device-pair-trust-connect-report.h"
#include "bluez/bluez-device-properties.h"

struct _BluezDevicePairTrustConnectWorkflow {
  BluezState *state;
  BluezOperationReporter *reporter;
  BluezDevicePowerGate *power_gate;
  BluezDevicePropertiesController *properties;
  BluezDeviceConnect connect_device;
  gpointer connect_user_data;
  GDestroyNotify connect_destroy;
  guint ref_count;
  gboolean disposed;
};

static void bluez_device_pair_trust_connect_workflow_unref(BluezDevicePairTrustConnectWorkflow *workflow);
static void bluez_device_pair_trust_connect_start_pair_step(BluezDevicePairTrustConnectWorkflow *workflow,
                                                            const char *device_path);

static BluezDevicePairTrustConnectWorkflow *
bluez_device_pair_trust_connect_workflow_ref(BluezDevicePairTrustConnectWorkflow *workflow)
{
  workflow->ref_count++;
  return workflow;
}

static void
bluez_device_pair_trust_connect_workflow_unref(BluezDevicePairTrustConnectWorkflow *workflow)
{
  workflow->ref_count--;
  if (workflow->ref_count == 0)
    g_free(workflow);
}

static void
bluez_device_pair_trust_connect_workflow_dispose(BluezDevicePairTrustConnectWorkflow *workflow)
{
  if (!workflow || workflow->disposed)
    return;

  workflow->disposed = TRUE;
  if (workflow->connect_destroy && workflow->connect_user_data)
    workflow->connect_destroy(workflow->connect_user_data);
  workflow->state = NULL;
  workflow->reporter = NULL;
  workflow->power_gate = NULL;
  workflow->properties = NULL;
  workflow->connect_device = NULL;
  workflow->connect_user_data = NULL;
  workflow->connect_destroy = NULL;
}

static void
bluez_device_pair_trust_connect_resume_pair(const char *device_path, gpointer user_data)
{
  BluezDevicePairTrustConnectWorkflow *workflow = user_data;

  bluez_device_pair_trust_connect_start_pair_step(workflow, device_path);
}

static void
bluez_device_pair_trust_connect_report_power_failure(BluezDevicePairTrustConnectWorkflow *workflow,
                                                     const char *device_path,
                                                     BluezPowerGateStatus status)
{
  if (!workflow || !device_path)
    return;

  bluez_pair_trust_connect_report_power_failure(workflow->reporter, device_path, status);
}

static void
bluez_device_pair_trust_connect_on_power_pending_finished(const BluezPowerGateResumeResult *result,
                                                         gpointer user_data)
{
  BluezDevicePairTrustConnectWorkflow *workflow = user_data;

  if (result)
    bluez_device_pair_trust_connect_report_power_failure(workflow, result->device_path, result->status);
}

static void
bluez_device_pair_trust_connect_start_connect_step(BluezDevicePairTrustConnectWorkflow *workflow,
                                                   const char *device_path)
{
  if (workflow && !workflow->disposed && workflow->connect_device)
    workflow->connect_device(device_path, workflow->connect_user_data);
}

static void
bluez_device_pair_trust_connect_on_trust_finished(const char *device_path,
                                                  gboolean success,
                                                  gpointer user_data)
{
  BluezDevicePairTrustConnectWorkflow *workflow = user_data;

  if (success)
    bluez_device_pair_trust_connect_start_connect_step(workflow, device_path);
}

static void
bluez_device_pair_trust_connect_start_trust_step(BluezDevicePairTrustConnectWorkflow *workflow,
                                                 const char *device_path)
{
  if (!workflow || workflow->disposed)
    return;

  bluez_device_properties_controller_trust_with_callback(
    workflow->properties,
    device_path,
    bluez_device_pair_trust_connect_on_trust_finished,
    bluez_device_pair_trust_connect_workflow_ref(workflow),
    (GDestroyNotify)bluez_device_pair_trust_connect_workflow_unref);
}

static void
bluez_device_pair_trust_connect_handle_pair_failure(BluezDevicePairTrustConnectWorkflow *workflow,
                                                    const char *device_path,
                                                    const GError *error)
{
  if (!workflow || workflow->disposed)
    return;

  bluez_pair_trust_connect_report_pair_failure(workflow->reporter, device_path, error);
  bluez_state_refresh_if_active(workflow->state, workflow->disposed);
}

static void
bluez_device_pair_trust_connect_handle_pair_success(BluezDevicePairTrustConnectWorkflow *workflow,
                                                    const char *device_path)
{
  if (!workflow || workflow->disposed)
    return;

  bluez_pair_trust_connect_report_pair_success(workflow->reporter, device_path);
  bluez_device_pair_trust_connect_start_trust_step(workflow, device_path);
}

static void
bluez_device_pair_trust_connect_on_pair_finished(const char *device_path,
                                                 gboolean success,
                                                 const GError *error,
                                                 gpointer user_data)
{
  BluezDevicePairTrustConnectWorkflow *workflow = user_data;

  if (!success) {
    bluez_device_pair_trust_connect_handle_pair_failure(workflow, device_path, error);
    return;
  }

  bluez_device_pair_trust_connect_handle_pair_success(workflow, device_path);
}

static void
bluez_device_pair_trust_connect_start_pair_step(BluezDevicePairTrustConnectWorkflow *workflow,
                                                const char *device_path)
{
  if (!workflow || workflow->disposed || !workflow->state || !device_path)
    return;

  bluez_pair_trust_connect_report_pairing(workflow->reporter);
  if (workflow->disposed || !workflow->state)
    return;

  if (!bluez_device_pair_step_start(workflow->state,
                                    device_path,
                                    bluez_device_pair_trust_connect_on_pair_finished,
                                    bluez_device_pair_trust_connect_workflow_ref(workflow),
                                    (GDestroyNotify)bluez_device_pair_trust_connect_workflow_unref))
    bluez_pair_trust_connect_report_system_bus_unavailable(workflow->reporter, device_path);
}

static BluezPowerGateDecision
bluez_device_pair_trust_connect_start_power_step(BluezDevicePairTrustConnectWorkflow *workflow,
                                                 const char *device_path)
{
  return bluez_device_power_gate_ensure_powered(workflow->power_gate,
                                                device_path,
                                                bluez_device_pair_trust_connect_resume_pair,
                                                bluez_device_pair_trust_connect_workflow_ref(workflow),
                                                (GDestroyNotify)bluez_device_pair_trust_connect_workflow_unref,
                                                bluez_device_pair_trust_connect_on_power_pending_finished,
                                                bluez_device_pair_trust_connect_workflow_ref(workflow),
                                                (GDestroyNotify)bluez_device_pair_trust_connect_workflow_unref);
}

static gboolean
bluez_device_pair_trust_connect_power_step_ready(BluezDevicePairTrustConnectWorkflow *workflow,
                                                 const char *device_path,
                                                 BluezPowerGateDecision decision)
{
  if (decision.pending) {
    bluez_pair_trust_connect_report_turning_power_on(workflow->reporter);
    return FALSE;
  }

  if (!decision.ready) {
    bluez_device_pair_trust_connect_report_power_failure(workflow, device_path, decision.status);
    return FALSE;
  }

  return TRUE;
}

BluezDevicePairTrustConnectWorkflow *
bluez_device_pair_trust_connect_workflow_new(BluezState *state,
                                              BluezOperationReporter *reporter,
                                              BluezDevicePowerGate *power_gate,
                                              BluezDevicePropertiesController *properties,
                                              BluezDeviceConnect connect_device,
                                              gpointer connect_user_data,
                                              GDestroyNotify connect_destroy)
{
  BluezDevicePairTrustConnectWorkflow *workflow = NULL;

  g_return_val_if_fail(state != NULL, NULL);
  g_return_val_if_fail(power_gate != NULL, NULL);
  g_return_val_if_fail(properties != NULL, NULL);

  workflow = g_new0(BluezDevicePairTrustConnectWorkflow, 1);
  workflow->state = state;
  workflow->reporter = reporter;
  workflow->power_gate = power_gate;
  workflow->properties = properties;
  workflow->connect_device = connect_device;
  workflow->connect_user_data = connect_user_data;
  workflow->connect_destroy = connect_destroy;
  workflow->ref_count = 1;

  return workflow;
}

void
bluez_device_pair_trust_connect_workflow_free(BluezDevicePairTrustConnectWorkflow *workflow)
{
  if (!workflow)
    return;

  bluez_device_pair_trust_connect_workflow_dispose(workflow);
  bluez_device_pair_trust_connect_workflow_unref(workflow);
}

void
bluez_device_pair_trust_connect_workflow_start(BluezDevicePairTrustConnectWorkflow *workflow,
                                               const char *device_path)
{
  BluezPowerGateDecision decision = { 0 };

  if (!workflow || workflow->disposed || !workflow->state || !device_path)
    return;

  decision = bluez_device_pair_trust_connect_start_power_step(workflow, device_path);
  if (!bluez_device_pair_trust_connect_power_step_ready(workflow, device_path, decision))
    return;

  bluez_device_pair_trust_connect_start_pair_step(workflow, device_path);
}
