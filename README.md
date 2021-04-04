# HandTrackControllerDriver

This project creates a "virtual" controller driver under SteamVR using the [Vive Hand Tracking SDK](https://developer.vive.com/resources/vive-sense/sdk/vive-hand-tracking-sdk/).
Using this virtual controller, it will be possible to interact with UI's without wearing controllers. This is great for simulator environments (MSFS, DCS, Project Cars, etc.).

Currently supported:
- Tracking
- Trigger Axis (Pinching)
- Trigger Clicking (Pinching > 0.7)

Roadmap for the future:
- System Button Clicking (Wrist Tap)
- Skeleton tracking!

For a quick demo: https://streamable.com/8uod3l


## Install from Release
Create a 'handtrack' folder in your SteamVR drivers path (usually 'C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers'). Extract the release contents in this new folder. Now you're all set! 

## Compile and Run
This project uses [microsoft/vcpkg](https://github.com/microsoft/vcpkg) for easier package management. 
Because of [legal uncertainties](https://forum.vive.com/topic/9445-redistribute-the-sdk-through-vcpkg/) I have not yet created a PR for the Handtrack SDK.
So for now: 

```
git clone https://github.com/EniacMlezi/vcpkg 
cd vcpkg
./bootstrap-vcpkg
./vcpkg install openvr:x64-windows
./vcpkg install vive-handtrack-sdk:x64-windows
./vcpkg integrate install
```

### Configure Paths
Under "Property Manager" in Visual Studio, expand any of the configurations and find "Paths". Right click and select "Properties" and then "User Macros". Modify the paths to match your setup. InstallDir will be created, and will be configured as an external driver path for SteamVR.

### Build and Run
Starting the debugger will automatically install the driver, launch SteamVR and debug the application. 

### Clean
Cleaning the solution will uninstall the driver.
