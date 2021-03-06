#pragma once
#include "dx_azure_iot.h"

/****************************************************************************************
 * Telemetry message templates and property sets
 ****************************************************************************************/
#define GX_AZURE_IOT true

// Number of bytes to allocate for the JSON telemetry message for IoT Hub/Central
#define PublishTelemetry_JSON_MESSAGE_LENGTH 128

static char gx_PublishTelemetryBuffer[PublishTelemetry_JSON_MESSAGE_LENGTH] = {0};

static DX_MESSAGE_PROPERTY *gx_PublishTelemetryMessageProperties[] = {
    &(DX_MESSAGE_PROPERTY){.key = "appid", .value = "hvac"},
    &(DX_MESSAGE_PROPERTY){.key = "type", .value = "telemetry"},
    &(DX_MESSAGE_PROPERTY){.key = "schema", .value = "1"}};

static DX_MESSAGE_CONTENT_PROPERTIES gx_PublishTelemetryContentProperties = {.contentEncoding = "utf-8", .contentType = "application/json"};