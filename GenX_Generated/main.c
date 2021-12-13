/* Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 *   DISCLAIMER
 *
 *   The Azure Sphere DevX library is a community drive library
 *
 *	   1. are built from the Azure Sphere SDK Samples at https://github.com/Azure/azure-sphere-samples
 *	   2. are not intended as a substitute for understanding the Azure Sphere SDK Samples.
 *	   3. aim to follow best practices as demonstrated by the Azure Sphere SDK Samples.

 *
 *   DEVELOPER BOARD SELECTION
 *
 *   The following developer boards are supported.
 *
 *	   1. AVNET Azure Sphere Starter Kit.
 *     2. AVNET Azure Sphere Starter Kit Revision 2.
 *	   3. Seeed Studio Azure Sphere MT3620 Development Kit aka Reference Design Board or rdb.
 *	   4. Seeed Studio Seeed Studio MT3620 Mini Dev Board.
 *
 *   ENABLE YOUR DEVELOPER BOARD
 *
 *   Each Azure Sphere developer board manufacturer maps pins differently. You need to select the
 *      configuration that matches your board.
 *
 *   Follow these steps:
 *
 *	   1. Open CMakeLists.txt.
 *	   2. Uncomment the set command that matches your developer board.
 *	   3. Click File, then Save to save the CMakeLists.txt file which will auto generate the
 *          CMake Cache.
 *
 ************************************************************************************************/

#include "main.h"
#include "app_exit_codes.h"

/// <summary>
///  Initialize gpios, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralAndHandlers(void)
{
	dx_Log_Debug_Init(Log_Debug_buffer, sizeof(Log_Debug_buffer));
	gx_initPeripheralAndHandlers();

	// Uncomment the StartWatchdog when ready for production
	StartWatchdog();
}

/// <summary>
///     Close Timers, GPIOs, Device Twins, Direct Methods
/// </summary>
static void ClosePeripheralAndHandlers(void)
{
	dx_azureToDeviceStop();
	gx_closePeripheralAndHandlers();
	dx_timerEventLoopStop();
}

/// <summary>
///  Main event loop for the app
/// </summary>
int main(int argc, char *argv[])
{
	dx_registerTerminationHandler();
	if (!dx_configParseCmdLineArguments(argc, argv, &dx_config))
	{
		return dx_getTerminationExitCode();
	}

	InitPeripheralAndHandlers();

	// Run the main event loop. This is a blocking call until termination requested
	dx_eventLoopRun();

	ClosePeripheralAndHandlers();
	return dx_getTerminationExitCode();
}

// Main code blocks


/// GENX_BEGIN ID:FactoryAlertButtonA MD5:93b32861f782c6da06dcf38446f5c2b3
/// <summary>
/// Implement your GPIO input timer function
/// </summary>
static DX_DEFINE_TIMER_HANDLER(FactoryAlertButtonA_gx_handler)
{
    static GPIO_Value_Type gpio_FactoryAlertButtonA_NewState;

    if (dx_gpioStateGet(&gpio_FactoryAlertButtonA, &gpio_FactoryAlertButtonA_NewState)) {
        Log_Debug("gpio_FactoryAlertButtonA: %d\n", gpio_FactoryAlertButtonA_NewState);
    }
}
DX_END_TIMER_HANDLER
/// GENX_END ID:FactoryAlertButtonA


/// GENX_BEGIN ID:FactoryAlertButtonB MD5:b82126b992672c2e4fcff99266144f09
/// <summary>
/// Implement your GPIO input timer function
/// </summary>
static DX_DEFINE_TIMER_HANDLER(FactoryAlertButtonB_gx_handler)
{
    static GPIO_Value_Type gpio_FactoryAlertButtonB_NewState;

    if (dx_gpioStateGet(&gpio_FactoryAlertButtonB, &gpio_FactoryAlertButtonB_NewState)) {
        Log_Debug("gpio_FactoryAlertButtonB: %d\n", gpio_FactoryAlertButtonB_NewState);
    }
}
DX_END_TIMER_HANDLER
/// GENX_END ID:FactoryAlertButtonB


/// GENX_BEGIN ID:PressureAlertLevel MD5:2e044e96133d6928d5a8253827145f18
/// <summary>
/// What is the purpose of this device twin handler function?
/// </summary>
/// <param name="deviceTwinBinding"></param>
static DX_DEFINE_DEVICE_TWIN_HANDLER(PressureAlertLevel_gx_handler, deviceTwinBinding)
{
    DX_GPIO_BINDING *context = (DX_GPIO_BINDING *)deviceTwinBinding->context;
    
    Log_Debug("Device Twin Property %s called\n", deviceTwinBinding->propertyName);
    void *value = deviceTwinBinding->propertyValue;

    // Test device twin float is within range
    if (!IN_RANGE(*(float*)value, 1600, 2000))
    {
        dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_ERROR);
    }

    dx_gpioOn(context);

    // implement your device twin logic

    dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_COMPLETED);
}
DX_END_DEVICE_TWIN_HANDLER
/// GENX_END ID:PressureAlertLevel


/// GENX_BEGIN ID:OfficeTargetTemperature MD5:5d99fde994fa81954166fd31f2d74ee7
/// <summary>
/// What is the purpose of this device twin handler function?
/// </summary>
/// <param name="deviceTwinBinding"></param>
static DX_DEFINE_DEVICE_TWIN_HANDLER(OfficeTargetTemperature_gx_handler, deviceTwinBinding)
{
    Log_Debug("Device Twin Property %s called\n", deviceTwinBinding->propertyName);
    void *value = deviceTwinBinding->propertyValue;
    
    if (!IN_RANGE(*(int*)value, 0, 80))
    {
        dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_ERROR);
    }

    // implement your device twin logic

    dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_COMPLETED);
}
DX_END_DEVICE_TWIN_HANDLER
/// GENX_END ID:OfficeTargetTemperature


/// GENX_BEGIN ID:FactoryLightOn MD5:5c6c5a0cef720aaed67300adadca53e0
/// <summary>
/// FactoryLightOn direct method purpose
/// </summary>
static DX_DEFINE_DIRECT_METHOD_HANDLER(FactoryLightOn_gx_handler, json, directMethodBinding, responseMsg)
{
    DX_GPIO_BINDING *context = (DX_GPIO_BINDING *)directMethodBinding->context;
    dx_gpioOn(context);
    return DX_METHOD_SUCCEEDED;
}
DX_END_DIRECT_METHOD_HANDLER
/// GENX_END ID:FactoryLightOn


/// GENX_BEGIN ID:FactoryLightOff MD5:4da1a07c971b4ee6137219638a8e17d7
/// <summary>
/// FactoryLightOff direct method purpose
/// </summary>
static DX_DEFINE_DIRECT_METHOD_HANDLER(FactoryLightOff_gx_handler, json, directMethodBinding, responseMsg)
{
    DX_GPIO_BINDING *context = (DX_GPIO_BINDING *)directMethodBinding->context;
    dx_gpioOff(context);
    return DX_METHOD_SUCCEEDED;
}
DX_END_DIRECT_METHOD_HANDLER
/// GENX_END ID:FactoryLightOff


/// GENX_BEGIN ID:LightControl MD5:2d30ddb9e1709cec386bc9783f109f72
/// <summary>
/// LightControl direct method purpose
/// </summary>
static DX_DEFINE_DIRECT_METHOD_HANDLER(LightControl_gx_handler, json, directMethodBinding, responseMsg) 
{
    // Direct method JSON payload example {{"State":true}}:
    char state_property[] = "State";
    JSON_Object *jsonObject = json_value_get_object(json);
    if (jsonObject == NULL || !json_object_has_value_of_type(jsonObject, state_property, JSONBoolean)) {{
        return DX_METHOD_FAILED;
    }}
    bool state = (int)json_object_get_boolean(jsonObject, state_property);
    if (state){
      Log_Debug("State is true\n");
    } else {
      Log_Debug("State is false\n");
    }
   
    // Example direct method with no payload
    // Implement your logic here

    return DX_METHOD_SUCCEEDED;
}
DX_END_DIRECT_METHOD_HANDLER
/// GENX_END ID:LightControl


/// GENX_BEGIN ID:FanOff MD5:5a6d1a3e082f3260447fd0cde3ea8702
/// <summary>
/// FanOff direct method purpose
/// </summary>
static DX_DEFINE_DIRECT_METHOD_HANDLER(FanOff_gx_handler, json, directMethodBinding, responseMsg)
{
    DX_GPIO_BINDING *context = (DX_GPIO_BINDING *)directMethodBinding->context;
    dx_gpioOff(context);
    return DX_METHOD_SUCCEEDED;
}
DX_END_DIRECT_METHOD_HANDLER
/// GENX_END ID:FanOff


/// GENX_BEGIN ID:FanOn MD5:398dae856dda8e7fe7ac6734ae7ef558
/// <summary>
/// FanOn direct method purpose
/// </summary>
static DX_DEFINE_DIRECT_METHOD_HANDLER(FanOn_gx_handler, json, directMethodBinding, responseMsg)
{
    DX_GPIO_BINDING *context = (DX_GPIO_BINDING *)directMethodBinding->context;
    dx_gpioOn(context);
    return DX_METHOD_SUCCEEDED;
}
DX_END_DIRECT_METHOD_HANDLER
/// GENX_END ID:FanOn


/// GENX_BEGIN ID:MeasureTemperature MD5:a822377b049818a5f19b50f26a48884c
/// <summary>
/// MeasureTemperature_gx_handler purpose?
/// </summary>
static DX_DEFINE_TIMER_HANDLER(MeasureTemperature_gx_handler)
{
    Log_Debug("Periodic timer MeasureTemperature_handler called\n");

    // Implement your timer function
    
}
DX_END_TIMER_HANDLER
/// GENX_END ID:MeasureTemperature


/// GENX_BEGIN ID:Watchdog MD5:9430d1192e8f6ad3c73b1f6d1a2c469a
/// <summary>
/// This timer extends the app level lease watchdog timer
/// </summary>
/// <param name="eventLoopTimer"></param>
static void Watchdog_gx_handler(EventLoopTimer *eventLoopTimer) {
    if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0) {
        dx_terminate(DX_ExitCode_ConsumeEventLoopTimeEvent);
        return;
    }
    timer_settime(watchdogTimer, 0, &watchdogInterval, NULL);
}

/// <summary>
/// Set up watchdog timer - the lease is extended via the Watchdog_handler function
/// </summary>
/// <param name=""></param>
static void StartWatchdog(void) {
	struct sigevent alarmEvent;
	alarmEvent.sigev_notify = SIGEV_SIGNAL;
	alarmEvent.sigev_signo = SIGALRM;
	alarmEvent.sigev_value.sival_ptr = &watchdogTimer;

	if (timer_create(CLOCK_MONOTONIC, &alarmEvent, &watchdogTimer) == 0) {
		if (timer_settime(watchdogTimer, 0, &watchdogInterval, NULL) == -1) {
			Log_Debug("Issue setting watchdog timer. %s %d\n", strerror(errno), errno);
		}
	}
}
/// GENX_END ID:Watchdog


/// GENX_BEGIN ID:DeferredUpdate MD5:561d077d7dc609f3c9b457eb67dd20f6
/// <summary>
/// Determine whether or not to defer an update
/// </summary>
/// <param name="max_deferral_time_in_minutes">The maximum number of minutes you can defer</param>
/// <returns>Return 0 to start update, return greater than zero to defer</returns>
static uint32_t DeferredUpdateCalculate_gx_handler(uint32_t max_deferral_time_in_minutes, SysEvent_UpdateType type,
	SysEvent_Status status, const char* typeDescription,
	const char* statusDescription) {

	uint32_t requested_minutes = 0;
	time_t now = time(NULL);
	struct tm* t = gmtime(&now);
	char msgBuffer[128];
	char utc[40];

	// UTC +10 would work well for devices in Australia
	t->tm_hour += 10;
	t->tm_hour = t->tm_hour % 24;

	// Set requested_minutes to zero to allow update, greater than zero to defer the update
	requested_minutes = t->tm_hour >= 1 && t->tm_hour <= 5 ? 0 : 10;

	snprintf(msgBuffer, sizeof(msgBuffer), "Utc: %s, Type: %s, Status: %s, Max defer minutes: %i, Requested minutes: %i", 
		dx_getCurrentUtc(utc, sizeof(utc)), typeDescription, statusDescription, max_deferral_time_in_minutes, requested_minutes);

	dx_deviceTwinReportValue(&dt_DeferredUpdateRequest, msgBuffer);

	return requested_minutes;
}
/// GENX_END ID:DeferredUpdate

