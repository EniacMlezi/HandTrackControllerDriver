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

		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_ModelNumber_String, "handtrack_controller");
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_TrackingSystemName_String, "handtrack");

		switch (m_hand)
		{
		case HandControllerHand::HCH_Left:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "oculus_rifts_controller_left");
			break;
		case HandControllerHand::HCH_Right:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
			vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "oculus_rifts_controller_right");
			break;
		default:
			vr::VRProperties()->SetInt32Property(m_propertyContainer, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_OptOut);
			vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_RenderModelName_String, "generic_controller");
		}

		// Set controller profile
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_InputProfilePath_String, "{handtrack}/input/handtrack_controller_bindings.json");

		std::string controller_handedness_str = m_hand == HandControllerHand::HCH_Left  ? "left" : "right";
		std::string controller_ready_file = "{handtrack}/icons/controller_ready_" + controller_handedness_str + ".png";
		std::string controller_not_ready_file = "{handtrack}/icons/controller_not_ready_" + controller_handedness_str + ".png";
		const char* controller_ready_file_cstr = controller_ready_file.c_str();
		const char* controller_not_ready_file_cstr = controller_not_ready_file.c_str();

		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceOff_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, controller_not_ready_file_cstr); 
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceStandbyAlert_String, controller_not_ready_file_cstr);
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, controller_not_ready_file_cstr);
		
		vr::VRProperties()->SetStringProperty(m_propertyContainer, vr::Prop_NamedIconPathDeviceReady_String, controller_ready_file_cstr);

		vr::EVRInputError error;
		error = vr::VRDriverInput()->CreateBooleanComponent(m_propertyContainer, "/input/trigger/click", &m_triggerClick);
		if (error)
		{
			vr::VRDriverLog()->Log("HandTrackController::Activate: VRDriverInputError!");
		}
		vr::VRDriverInput()->CreateScalarComponent(m_propertyContainer, "/input/trigger/value", &m_triggerValue, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedOneSided);
		if (error)
		{
			vr::VRDriverLog()->Log("HandTrackController::Activate: VRDriverInputError!");
		}

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

void CalculateRotation(vr::HmdMatrix34_t& m, vr::HmdQuaternionf_t& q)
{
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];
	if (trace >= 0) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (m.m[2][1] - m.m[1][2]) * s;
		q.y = (m.m[0][2] - m.m[2][0]) * s;
		q.z = (m.m[1][0] - m.m[0][1]) * s;
	}
	else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2])
	{
		float s = 2.0f * sqrtf(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]);
		q.w = (m.m[2][1] - m.m[1][2]) / s;
		q.x = 0.25f * s;
		q.y = (m.m[0][1] + m.m[1][0]) / s;
		q.z = (m.m[0][2] + m.m[2][0]) / s;
	}
	else if (m.m[1][1] > m.m[2][2]) {
		float s = 2.0f * sqrtf(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]);
		q.w = (m.m[0][2] - m.m[2][0]) / s;
		q.x = (m.m[0][1] + m.m[1][0]) / s;
		q.y = 0.25f * s;
		q.z = (m.m[1][2] + m.m[2][1]) / s;
	}
	else {
		float s = 2.0f * sqrtf(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]);
		q.w = (m.m[1][0] - m.m[0][1]) / s;
		q.x = (m.m[0][2] + m.m[2][0]) / s;
		q.y = (m.m[1][2] + m.m[2][1]) / s;
		q.z = 0.25f * s;
	}
}

void HandTrackController::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	vr::VRDriverLog()->Log("HandTrackController::DebugRequest: Enter.");
	
	std::istringstream l_chaperoneDataSS(pchRequest);
	vr::HmdMatrix34_t m;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			l_chaperoneDataSS >> m.m[i][j];
		}
	}

	m_pose.vecWorldFromDriverTranslation[0] = m.m[0][3];
	m_pose.vecWorldFromDriverTranslation[1] = m.m[1][3];
	m_pose.vecWorldFromDriverTranslation[2] = m.m[2][3];

	vr::HmdQuaternionf_t q;
	CalculateRotation(m, q);

	m_pose.qWorldFromDriverRotation.w = q.w;
	m_pose.qWorldFromDriverRotation.x = q.x;
	m_pose.qWorldFromDriverRotation.y = q.y;
	m_pose.qWorldFromDriverRotation.z = q.z;

	vr::VRDriverLog()->Log("HandTrackController::DebugRequest: Exit.");
}

vr::DriverPose_t HandTrackController::GetPose()
{
	vr::VRDriverLog()->Log("HandTrackController::GetPose: Enter.");
	vr::VRDriverLog()->Log("HandTrackController::GetPose: Exit.");
	return m_last_pose;
}
#pragma endregion

#pragma region HandTrackController
HandTrackController::HandTrackController(HandControllerHand hand)
:	m_hand(hand),
	m_pose({0}),
	m_trackedDeviceObjectId(vr::k_unTrackedDeviceIndexInvalid),
	m_propertyContainer(vr::k_ulInvalidPropertyContainer),
	m_last_pose({0}),
	m_triggerClick(0),
	m_triggerValue(0)
{	
}

void HandTrackController::UpdatePose(const GestureResult *gesture)
{
	m_pose.deviceIsConnected = true;
	if (nullptr == gesture)
	{
		m_pose.poseIsValid = false;
		m_pose.result = vr::TrackingResult_Running_OutOfRange;
	}
	else
	{
		m_pose.poseIsValid = true;
		m_pose.result = vr::TrackingResult_Running_OK;

		//Rotate the controller slightly forward for easier point and click.
		m_pose.qDriverFromHeadRotation.w = 0.843;
		m_pose.qDriverFromHeadRotation.x = 0.537;
		m_pose.qDriverFromHeadRotation.y = 0;
		m_pose.qDriverFromHeadRotation.z = -0.10;
		
		//Actual controller position
		m_pose.vecPosition[0] = gesture->position.x;
		m_pose.vecPosition[1] = gesture->position.y;
		m_pose.vecPosition[2] = -gesture->position.z;

		//Rotation of the wrist joint (joint 0)
		m_pose.qRotation.w = -gesture->rotations[0].w;
		m_pose.qRotation.x = gesture->rotations[0].x;
		m_pose.qRotation.y = gesture->rotations[0].y;
		m_pose.qRotation.z = -gesture->rotations[0].z;

		vr::VRDriverInput()->UpdateBooleanComponent(m_triggerClick, gesture->pinch.isPinching(), 0);
		vr::VRDriverInput()->UpdateScalarComponent(m_triggerValue, gesture->pinch.pinchLevel, 0);
	}

	vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDeviceObjectId, m_pose, sizeof(vr::DriverPose_t));
	m_last_pose = m_pose;
}
#pragma endregion
