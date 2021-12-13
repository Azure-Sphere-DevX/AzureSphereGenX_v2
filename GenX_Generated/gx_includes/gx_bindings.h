#pragma once

#include "dx_azure_iot.h"
#include "dx_config.h"
#include "dx_exit_codes.h"
#include "dx_gpio.h"
#include "dx_json_serializer.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_version.h"
#include "dx_deferred_update.h"

#include "applibs_versions.h"
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "gx_includes.h"

extern DX_USER_CONFIG dx_config;
extern const char PNP_MODEL_ID[];
extern const char NETWORK_INTERFACE[];


/****************************************************************************************
 * Forward declarations
 ****************************************************************************************/
static DX_DECLARE_DIRECT_METHOD_HANDLER(Counter_gx_handler);
static DX_DECLARE_DIRECT_METHOD_HANDLER(FactoryLightOff_gx_handler);
static DX_DECLARE_TIMER_HANDLER(FactoryAlertButtonA_gx_handler);


/****************************************************************************************
* Binding declarations
****************************************************************************************/
extern int count;
static DX_GPIO_BINDING gpio_FactoryAlertButtonA = { .pin = BUTTON_A, .name = "FactoryAlertButtonA", .direction = DX_INPUT, .detect = DX_GPIO_DETECT_LOW };
static DX_GPIO_BINDING gpio_FactoryLight = { .pin = STATUS_LED, .name = "FactoryLight", .direction = DX_OUTPUT, .initialState = GPIO_Value_Low, .invertPin = false };
static DX_DIRECT_METHOD_BINDING dm_FactoryLightOff = { .methodName = "FactoryLightOff", .handler = FactoryLightOff_gx_handler, .context=&gpio_FactoryLight };
static DX_DIRECT_METHOD_BINDING dm_Counter = { .methodName = "Counter", .handler = Counter_gx_handler, .context=&count };
static DX_TIMER_BINDING tmr_FactoryAlertButtonA = { .repeat = &(struct timespec){ 0, 200 * ONE_MS }, .delay = NULL, .name = "FactoryAlertButtonA", .handler = FactoryAlertButtonA_gx_handler };


// All GPIOs referenced in gpio_bindings with be opened in the gx_initPeripheralAndHandlers function
static DX_GPIO_BINDING *gpio_bindings[] = { &gpio_FactoryAlertButtonA, &gpio_FactoryLight };

// All direct methods referenced in direct_method_bindings will be subscribed to in the gx_initPeripheralAndHandlers function
static DX_DEVICE_TWIN_BINDING* device_twin_bindings[] = {  };

// All direct methods referenced in direct_method_bindings will be subscribed to in the gx_initPeripheralAndHandlers function
static DX_DIRECT_METHOD_BINDING *direct_method_bindings[] = { &dm_FactoryLightOff, &dm_Counter };

// All timers referenced in timer_bindings will be opened in the gx_initPeripheralAndHandlers function
#define DECLARE_DX_TIMER_BINDINGS
static DX_TIMER_BINDING *timer_bindings[] = { &tmr_FactoryAlertButtonA };


/****************************************************************************************
* Initialise bindings
****************************************************************************************/

static void gx_initPeripheralAndHandlers(void)
{

#ifdef GX_AZURE_IOT
    dx_azureConnect(&dx_config, NETWORK_INTERFACE, PNP_MODEL_ID);
#else
    if (NELEMS(device_twin_bindings) > 0 || NELEMS(direct_method_bindings) > 0) {
        dx_azureConnect(&dx_config, NETWORK_INTERFACE, PNP_MODEL_ID);
    }
#endif // GX_AZURE_IOT

    dx_gpioSetOpen(gpio_bindings, NELEMS(gpio_bindings));
    dx_deviceTwinSubscribe(device_twin_bindings, NELEMS(device_twin_bindings));
    dx_directMethodSubscribe(direct_method_bindings, NELEMS(direct_method_bindings));
    dx_timerSetStart(timer_bindings, NELEMS(timer_bindings));

#ifdef GX_DEFERRED_UPDATE
       dx_deferredUpdateRegistration(DeferredUpdateCalculate_gx_handler, NULL);
#endif

}

static void gx_closePeripheralAndHandlers(void){
    dx_timerSetStop(timer_bindings, NELEMS(timer_bindings));
    dx_deviceTwinUnsubscribe();
    dx_directMethodUnsubscribe();
    dx_gpioSetClose(gpio_bindings, NELEMS(gpio_bindings));
    dx_azureToDeviceStop();
}
