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

static DX_TIMER_HANDLER(msg_handler)
{
	static int charId = 0;

	Log_Debug("Hello world\n");

	gfx_load_character((charId++ % 10) + 48, panel8x8_b.bitmap);
	gfx_reverse_panel(panel8x8_b.bitmap);
	gfx_rotate_counterclockwise(panel8x8_b.bitmap, 1, 1, panel8x8_b.bitmap);
	gfx_reverse_panel(panel8x8_b.bitmap);
	max7219_panel_write(&panel8x8_b);
}
DX_TIMER_HANDLER_END

/// <summary>
///  Initialize gpios, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralAndHandlers(void)
{
	dx_Log_Debug_Init(Log_Debug_buffer, sizeof(Log_Debug_buffer));

	max7219_init(&panel8x8_b, 0);
	max7219_clear(&panel8x8_b);

	gx_initPeripheralAndHandlers();

	// Uncomment the StartWatchdog when ready for production
	// StartWatchdog();
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
