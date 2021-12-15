##################################################################################################################
# IMPORTANT READ
#
# You must rebuild the CMakeLists cache if you change any options here.
# In Visual Studio Code, press F1, select Delete Cache and Reconfigure.
# Visual Studio will regenerate the CMakeLists when any file prefixed with .cmake is updated
#
##################################################################################################################

##################################################################################################################
# Select your developer board by removing the # tag from the beginning of the line
# If you are NOT using the AVNET Revision 1 board be sure to comment out the AVNET board
#
set(AVNET TRUE "AVNET Azure Sphere Starter Kit Revision 1")    
# set(AVNET_REV_2 TRUE "AVNET Azure Sphere Starter Kit Revision 2")             
# set(SEEED_STUDIO_RDB TRUE "Seeed Studio Azure Sphere MT3620 Development Kit (aka Reference Design Board or RDB)")
# set(SEEED_STUDIO_MINI TRUE "Seeed Studio Azure Sphere MT3620 Mini Dev Board")
#
##################################################################################################################

##################################################################################################################
# Uncomment if using AZURE SPHERE REMOTE remote peripheral over network access
add_compile_definitions(AZURE_SPHERE_REMOTEX_IP="192.168.10.118")
##################################################################################################################

##################################################################################################################
# Configure Azure IoT Hub conection string
add_compile_definitions(IOT_HUB_HOST_NAME="HostName=iot-hub-azure-sphere.azure-devices.net")
add_compile_definitions(IOT_HUB_DEVICE_ID="DeviceId=DesktopDevX")
add_compile_definitions(IOT_HUB_SHARED_ACCESS_KEY="SharedAccessKey=FrqMj/OivUWvyKlbIdtcodfx4IR0L1MutA6oPDPApPw=")
##################################################################################################################

##################################################################################################################
# Comment out to disable performance mode
add_compile_definitions(AZURE_SPHERE_REMOTEX_PERFORMANCE_MODE)
##################################################################################################################