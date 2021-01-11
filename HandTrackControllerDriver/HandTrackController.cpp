#include "HandTrackController.h"

#pragma region ITrackedDeviceServerDriver
vr::EVRInitError HandTrackController::Activate(uint32_t unObjectId)
{
	vr::VRDriverLog()->Log("HandTrackController::Activate: Enter.");
	vr::EVRInitError l_resultError = vr::VRInitError_Driver_Failed;
	if (m_trackedDeviceObjectId == vr::k_unTrackedDeviceIndexInvalid)
	{
		m_trackedDeviceObjectId = unObjectId;
		m_propertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_trackedDeviceObjectId);

		vr::VRProperties()->SetUint64Property(m_propertyContainer, vr::Prop_CurrentUniverseId_Uint64, 2);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, "handtrack_controller");

		switch (m_hand)
		{
		case HandControllerHand::HCH_Left:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			break;
		case HandControllerHand::HCH_Right:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
			break;
		default:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_OptOut);
		}

		std::string controller_handedness_str = m_hand == HandControllerHand::HCH_Left  ? "left" : "right";
		std::string controller_ready_file = "{handtrack}/icons/controller_ready_" + controller_handedness_str + ".png";
		std::string controller_not_ready_file = "{handtrack}/icons/controller_not_ready_" + controller_handedness_str + ".png";
		const char* controller_ready_file_cstr = controller_ready_file.c_str();
		const char* controller_not_ready_file_cstr = controller_not_ready_file.c_str();

		//TODO: Should use the icon resource file here
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceOff_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, controller_not_ready_file_cstr); 
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandbyAlert_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, controller_not_ready_file_cstr);
		
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReady_String, controller_ready_file_cstr);

		l_resultError = vr::VRInitError_None;
	}
	vr::VRDriverLog()->Log("HandTrackController::Activate: Exit.");
	return l_resultError;
}

void HandTrackController::Deactivate()
{
	vr::VRDriverLog()->Log("HandTrackController::Deactivate: Enter.");
	m_trackedDeviceObjectId = vr::k_unTrackedDeviceIndexInvalid;
	vr::VRDriverLog()->Log("HandTrackController::Deactivate: Exit.");
}

void HandTrackController::EnterStandby()
{
	vr::VRDriverLog()->Log("HandTrackController::EnterStandby: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::EnterStandby: Exit.");
}

void* HandTrackController::GetComponent(const char* pchComponentNameAndVersion)
{
	vr::VRDriverLog()->Log("HandTrackController::GetComponent: Enter.");
	void* l_result = nullptr;
	if (0 == strcmp(pchComponentNameAndVersion, vr::ITrackedDeviceServerDriver_Version))
	{
		l_result = dynamic_cast<vr::ITrackedDeviceServerDriver*>(this);
	}
	vr::VRDriverLog()->Log("HandTrackController::GetComponent: Exit.");
	return l_result;
}

void HandTrackController::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	vr::VRDriverLog()->Log("HandTrackController::DebugRequest: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::DebugRequest: Exit.");
}

vr::DriverPose_t HandTrackController::GetPose()
{
	vr::VRDriverLog()->Log("HandTrackController::GetPose: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::GetPose: Exit.");
	return m_pose;
}
#pragma endregion

#pragma region HandTrackController
HandTrackController::HandTrackController(HandControllerHand hand)
	:	m_hand(hand), 
		m_trackedDeviceObjectId(vr::k_unTrackedDeviceIndexInvalid),
		m_propertyContainer(vr::k_ulInvalidPropertyContainer),
		m_pose({0})
{
}

void HandTrackController::UpdatePose()
{
	m_pose.deviceIsConnected = true;
	m_pose.poseIsValid = true;
	m_pose.result = vr::TrackingResult_Running_OK;

	vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDeviceObjectId, m_pose, sizeof(vr::DriverPose_t));
}
#pragma endregion
