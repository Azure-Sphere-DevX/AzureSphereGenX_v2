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


/// GENX_BEGIN ID:Counter MD5:03a595a3c737182084c6746913b7e437
/// <summary>
/// Counter direct method purpose
/// </summary>
static DX_DEFINE_DIRECT_METHOD_HANDLER(Counter_gx_handler, json, directMethodBinding, responseMsg)
{
    int *context = (int *)directMethodBinding->context;
    context++;
    return DX_METHOD_SUCCEEDED;
}
DX_END_DIRECT_METHOD_HANDLER
/// GENX_END ID:Counter

