#pragma once

#include "hw/azure_sphere_remotex.h" // Hardware definition

#include "dx_azure_iot.h"
#include "dx_config.h"
#include "dx_deferred_update.h"
#include "dx_exit_codes.h"
#include "dx_gpio.h"
#include "dx_i2c.h"
#include "dx_json_serializer.h"
#include "dx_pwm.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_version.h"

#include "gx_bindings.h"
#include "gx_includes.h"

int count;


#define APPLICATION_VERSION "1.5"

DX_USER_CONFIG dx_config; // cmd args from app_manifest.json
const char PNP_MODEL_ID[] = "dtmi:com:example:application;1";
const char NETWORK_INTERFACE[] = "wlan0";

#define Log_Debug(f_, ...) dx_Log_Debug((f_), ##__VA_ARGS__)
static char Log_Debug_buffer[128];
