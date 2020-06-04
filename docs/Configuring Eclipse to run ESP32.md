This document is about configuring Eclipse to build and run ESP32 firmware. Can be used to configure almost any arduino-like hardware, same way as you can do it in Arduino IDE.

### Prerequirements
I'm using Eclipse 4.15.0, but any with 'Eclipse Marketplace' should work.  
Also i assume your IDE already has C/C++ support, if not, you can easily upgrade it by installing **'Eclipse C/C++ IDE CDT'** plugin from market place

### Steps to run ESP application
1. Open marketplace by clicking Help->Eclipse Marketplace

2. Find and install plugin 'The Arduino Eclipse plugin named Sloeber V4'.  
![arduino plugin](images/eclipse0.jpeg)

3. After installation new menu option 'Arduino' will be added on the top pane. To work with ESP32 you need to open 'Arduino->Preferences->Arduino->Third party index url' and add external url  
<https://adafruit.github.io/arduino-board-index/package_adafruit_index.json>  
<https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json>  

4. Then you can enable platform under 'Arduino->Preferences->Arduino->Platforms and Boards' and libraries 'Arduino->Preferences->Arduino->Library Manager'.  
![sdk lsit](images/eclipse2.jpeg)  
There is a useful option 'Arduino->Reattach libraries' which should search through the source code and enable all libraries in Library Manager, though sometimes it misses few   of them.

5. After that you just need to import existing project from src/firmware  
If you create a project from scratch, choose File->New->Arduino sketch. In project`s Preferences you want set ESP settings as depicted below
![new target](images/eclipse3.jpeg)  

6. Make sure that eclipse didn't override project files after import (.settings folder and .cproject .project files), otherwise you'll experience weird compilation errors and platform warnings.

**Note**

You may see following errors during compilation
```
platform.h:39:10: error: #include expects "FILENAME" or FILENAME
#include MBEDTLS_CONFIG_FILE
```
to fix this you need to escape quotes for few predefines.  
You need to change definitions in YOUR_ECLIPSE_LOCATION/eclipse/arduinoPlugin/packages/esp32/hardware/esp32/1.0.4/platform.txt  
Substitute  
```
-DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"
-DARDUINO_VARIANT="{build.variant}" 
```
with  
```
-DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"'  
-DARDUINO_VARIANT='"{build.variant}"'
```


Now you are all set!
