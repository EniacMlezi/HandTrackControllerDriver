// HandTrackControllerMonitor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <set>
#include <chrono>
#include <thread>
#include <openvr.h>

void FindInitialHandTrackControllers();
bool IsHandTrackController(vr::TrackedDeviceIndex_t deviceIndex);
void TryAddHandTrackController(vr::TrackedDeviceIndex_t deviceIndex);
void UpdateAllHandTrackControllerChaperones();
void UpdateHandTrackControllerChaperone(vr::TrackedDeviceIndex_t deviceIndex, vr::HmdMatrix34_t& m);

std::set<vr::TrackedDeviceIndex_t> m_setHandTrackDevices;

const std::chrono::milliseconds k_MonitorInterval(50);

int main()
{
	// Start as "background" application.  This prevents vrserver from being started
	// on our behalf, and prevents us from keeping vrserver alive when everything else
	// exits.  This is very important because we're spawning from a driver, and any
	// class besides "background" would keep vrserver running forever
	while (true)
	{
		vr::EVRInitError eVRInitError;
		vr::VR_Init(&eVRInitError, vr::VRApplication_Background);
		if (vr::VRSystem() && eVRInitError == vr::VRInitError_None)
		{
			break;
		}
		std::this_thread::sleep_for(k_MonitorInterval);
	}

	FindInitialHandTrackControllers();
	UpdateAllHandTrackControllerChaperones();

	while (true)
	{
		vr::VREvent_t l_event;
		while (vr::VRSystem()->PollNextEvent(&l_event, sizeof(l_event)))
		{
			switch (l_event.eventType)
			{
			case vr::VREvent_Quit:
				exit(0);
				// NOTREAHED

			case vr::VREvent_TrackedDeviceActivated:
			case vr::VREvent_TrackedDeviceUpdated:
				// a possible HandTrackController is connected. Try to subscribe it to Chaperone data.
				if (IsHandTrackController(l_event.trackedDeviceIndex))
				{
					TryAddHandTrackController(l_event.trackedDeviceIndex);
					vr::HmdMatrix34_t m;
					vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&m);

					UpdateHandTrackControllerChaperone(l_event.trackedDeviceIndex, m);
				}
				break;

			case vr::VREvent_ChaperoneUniverseHasChanged:
			case vr::VREvent_ChaperoneSettingsHaveChanged:
			case vr::VREvent_SeatedZeroPoseReset:
			case vr::VREvent_StandingZeroPoseReset:
				UpdateAllHandTrackControllerChaperones();
				break;
			}
		}

		std::this_thread::sleep_for(k_MonitorInterval);
	}
}

/// <summary>
/// Searches all TrackedDevices in OpenVR and adds HandTrack devices to the set of devices to update.
/// </summary>
void FindInitialHandTrackControllers()
{
	for (int td = 0; td < vr::k_unMaxTrackedDeviceCount; td++)
	{
		if (IsHandTrackController(td))
		{
			TryAddHandTrackController(td);
		}
	}
}

/// <summary>
/// Checks whether te device's TrackingSystemName string is 'HandTrack'. 
/// </summary>
/// <param name="deviceIndex">The device to check.</param>
/// <returns>Whether the device is a HandTrack controller</returns>
bool IsHandTrackController(vr::TrackedDeviceIndex_t deviceIndex)
{
	char l_trackingSystemName[10]; // "HandTrack\0"
	vr::ETrackedPropertyError l_error;
	vr::VRSystem()->GetStringTrackedDeviceProperty(deviceIndex, vr::Prop_TrackingSystemName_String, l_trackingSystemName, 10, &l_error);

	if (l_error != vr::TrackedProp_Success)
	{
		return false;
	}
	if (strcmp(l_trackingSystemName, "handtrack") != 0)
	{
		return false;
	}
	return true;
}

/// <summary>
/// Adds the device to HandTrackControllers to update about chaperone changes.
/// </summary>
/// <param name="deviceIndex"></param>
void TryAddHandTrackController(vr::TrackedDeviceIndex_t deviceIndex)
{
	if (m_setHandTrackDevices.count(deviceIndex) == 0)
	{
		m_setHandTrackDevices.insert(deviceIndex);
	}
}

/// <summary>
/// Updates chaperone info for all HandTrack devices in the list of previously added devices using OpenVR DebugRequests.
/// </summary>
void UpdateAllHandTrackControllerChaperones()
{
	vr::HmdMatrix34_t m;
	vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&m);
	
	for (auto it = m_setHandTrackDevices.begin(); it != m_setHandTrackDevices.end(); ++it)
	{
		UpdateHandTrackControllerChaperone(*it, m);
	}
}

/// <summary>
/// Updates the chaperone info of a single HandTrack device using an OpenVR DebugRequest.
/// </summary>
/// <param name="deviceIndex">The device to update.</param>
/// <param name="m">The chaperone info.</param>
void UpdateHandTrackControllerChaperone(vr::TrackedDeviceIndex_t deviceIndex, vr::HmdMatrix34_t& m)
{
	std::ostringstream l_chaperoneDataSS;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			l_chaperoneDataSS << " " << m.m[i][j];
		}
	}

	vr::VRDebug()->DriverDebugRequest(deviceIndex, l_chaperoneDataSS.str().c_str(), NULL, 0);
}