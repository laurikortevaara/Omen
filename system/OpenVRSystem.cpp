#include "OpenVRSystem.h"
#include "../component/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <openvr.h>
#include <iostream>
#include "../MathUtils.h"
#include "../component/KeyboardInput.h"
#include "../component/MouseInput.h"
#include "../component/CameraController.h"
#include "../Camera.h"

using namespace omen;
using namespace ecs;

static const float m_fNearClip = 0.01f;
static const float m_fFarClip = 1000.0f;

OpenVRSystem::OpenVRSystem() : m_renderVR(false)
{
	init();
	memset(m_rDevClassChar, 0, sizeof(m_rDevClassChar));

	m_camera = new Camera("VR Camera", { 0, 3.0, -10 }, { 0, 0, 0 }, 45.0f);
}

//-----------------------------------------------------------------------------
// Purpose: Helper to get a string from a tracked device property and turn it
//			into a std::string
//-----------------------------------------------------------------------------
std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

bool OpenVRSystem::init()
{
	vr::EVRInitError eError = vr::VRInitError_None;
	m_VRSystem = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None)
	{
		m_VRSystem = nullptr;
		std::cout << "Unable to init VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
		return false;
	}


	m_RenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
	if (!m_RenderModels)
	{
		m_VRSystem = nullptr;
		vr::VR_Shutdown();

		std::cout << "Unable to get render model interface: " << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
		return false;
	}

	m_strDriver = GetTrackedDeviceString(m_VRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	m_strModelLabel = GetTrackedDeviceString(m_VRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModeLabel_String);
	m_strModelNumber = GetTrackedDeviceString(m_VRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ModelNumber_String);
	m_strDisplay = GetTrackedDeviceString(m_VRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
	m_strManufacturerName = GetTrackedDeviceString(m_VRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_ManufacturerName_String);

	setupCameras(); 
	setupStereoRenderTargets();

	// Initialize the compositor
	m_Compositor = vr::VRCompositor();


	// Get the Mouse coordinates
	MouseInput *mi = Engine::instance()->findComponent<MouseInput>();
	if (mi != nullptr) {
		mi->signal_cursorpos_changed.connect([&](omen::floatprec x, omen::floatprec y) {
			CameraController* ctr = Engine::instance()->findComponent<CameraController>();
			if (!ctr->enabled())
				return;
			static omen::floatprec old_x = x;
			static omen::floatprec old_y = y;
			omen::floatprec dx = x - old_x;
			omen::floatprec dy = y - old_y;
			old_x = x;
			old_y = y;

			if (m_camera >= nullptr) {
				m_camera->yaw() += -dx*Engine::instance()->CameraSensitivity;
				m_camera->pitch() += dy*Engine::instance()->CameraSensitivity;
			}

		});
	}

	Engine::instance()->signal_engine_update.connect([this](omen::floatprec time, omen::floatprec deltaTime) {
		Engine* e = Engine::instance();
		KeyboardInput* ki = e->findComponent<KeyboardInput>();

		// velocity = velocity + accelleration
		// velo = m/s
		// acceleration = m/s^2
		if (m_camera != nullptr && !ki->keyModifierPressed(GLFW_MOD_SHIFT)) {
			m_camera->acceleration() = glm::vec3(10.35f);


			if (ki->keyPressed(GLFW_KEY_W)) {
				m_camera->velocity().z += m_camera->acceleration().z * deltaTime;
			}
			if (ki->keyPressed(GLFW_KEY_S)) {
				m_camera->velocity().z -= m_camera->acceleration().z * deltaTime;
			}

			if (ki->keyPressed(GLFW_KEY_A)) {
				m_camera->velocity().x -= m_camera->acceleration().x * deltaTime;
			}
			if (ki->keyPressed(GLFW_KEY_D)) {
				m_camera->velocity().x += m_camera->acceleration().x * deltaTime;
			}

			if (ki->keyPressed(GLFW_KEY_E)) {
				m_camera->velocity().y += m_camera->acceleration().y * deltaTime;
			}
			if (ki->keyPressed(GLFW_KEY_C)) {
				m_camera->velocity().y -= m_camera->acceleration().y * deltaTime;
			}
		}
	});

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool OpenVRSystem::setupStereoRenderTargets()
{
	if (m_VRSystem == nullptr)
		return false;

	m_VRSystem->GetRecommendedRenderTargetSize(&m_renderWidth, &m_renderHeight);

	createFrameBuffer(m_renderWidth, m_renderHeight, leftEyeDesc);
	createFrameBuffer(m_renderWidth, m_renderHeight, rightEyeDesc);

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Creates a frame buffer. Returns true if the buffer was set up.
//          Returns false if the setup failed.
//-----------------------------------------------------------------------------
bool OpenVRSystem::createFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
{
	glGenFramebuffers(1, &framebufferDesc.m_renderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_renderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_depthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_depthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_depthBufferId);

	glGenTextures(1, &framebufferDesc.m_renderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_renderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_renderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_resolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_resolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_resolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_resolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_resolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void OpenVRSystem::setupCameras()
{
	m_mat4ProjectionLeft = getHMDMatrixProjectionEye(vr::Eye_Left);
	m_mat4ProjectionRight = getHMDMatrixProjectionEye(vr::Eye_Right);
	m_mat4eyePosLeft = getHMDMatrixPoseEye(vr::Eye_Left);
	m_mat4eyePosRight = getHMDMatrixPoseEye(vr::Eye_Right);
}

//-----------------------------------------------------------------------------
// Purpose: Gets a Matrix Projection Eye with respect to nEye.
//-----------------------------------------------------------------------------
glm::mat4 OpenVRSystem::getHMDMatrixProjectionEye(vr::EVREye nEye)
{
	if (!m_VRSystem)
		return glm::mat4();

	vr::HmdMatrix44_t mat = m_VRSystem->GetProjectionMatrix(nEye, m_fNearClip, m_fFarClip);

	return glm::mat4(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
	);
}

glm::mat4 OpenVRSystem::getHMDMatrixPoseEye(vr::EVREye nEye)
{
	if (!m_VRSystem)
		return glm::mat4();

	vr::HmdMatrix34_t matEyeRight = m_VRSystem->GetEyeToHeadTransform(nEye);
	glm::mat4 matrixObj(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
	);

	return glm::inverse(matrixObj);
}

//-----------------------------------------------------------------------------
// Purpose: Gets a Current View Projection Matrix with respect to nEye,
//          which may be an Eye_Left or an Eye_Right.
//-----------------------------------------------------------------------------
glm::mat4 OpenVRSystem::getCurrentViewProjectionMatrix(vr::EVREye nEye)
{
	omen::Engine* e = Engine::instance();
	omen::Camera* c = e->camera();

	glm::mat4 pos(1);
	pos = glm::translate(pos, c->position());

	glm::mat4 matMVP;
	if (nEye == vr::Eye_Left)
	{
		matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft * m_mat4HMDPose * m_camera->view();
	}
	else if (nEye == vr::Eye_Right)
	{
		matMVP = m_mat4ProjectionRight * m_mat4eyePosRight * m_mat4HMDPose * m_camera->view();
	}

	//matMVP = glm::rotate(matMVP, static_cast<float>(M_PI), glm::vec3(0, 1, 0));
	return matMVP;
}

glm::mat4 OpenVRSystem::getCurrentViewMatrix(vr::EVREye eye)
{
	if (eye == vr::Eye_Left)
		return m_mat4eyePosLeft * glm::mat4(glm::mat3(m_mat4HMDPose));
	if (eye == vr::Eye_Right)
		return m_mat4eyePosRight * glm::mat4(glm::mat3(m_mat4HMDPose));
}

void OpenVRSystem::renderScene(vr::EVREye eye)
{
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();
	m_currentEye = eye;
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	gs->render(0);
	/*
	for (uint32_t unTrackedDevice = 0; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++)
	{
		if (!m_rTrackedDeviceToRenderModel[unTrackedDevice] || !m_rbShowTrackedDevice[unTrackedDevice])
			continue;

		const vr::TrackedDevicePose_t & pose = m_rTrackedDevicePose[unTrackedDevice];
		if (!pose.bPoseIsValid)
			continue;

		if (bIsInputCapturedByAnotherProcess && m_pHMD->GetTrackedDeviceClass(unTrackedDevice) == vr::TrackedDeviceClass_Controller)
			continue;

		const Matrix4 & matDeviceToTracking = m_rmat4DevicePose[unTrackedDevice];
		Matrix4 matMVP = GetCurrentViewProjectionMatrix(nEye) * matDeviceToTracking;
		glUniformMatrix4fv(m_nRenderModelMatrixLocation, 1, GL_FALSE, matMVP.get());

		gs->render(nullptr, -2);
		m_rTrackedDeviceToRenderModel[unTrackedDevice]->Draw();
	}
	*/
}

//-----------------------------------------------------------------------------
// Purpose: Converts a SteamVR matrix to our local matrix class
//-----------------------------------------------------------------------------
glm::mat4 OpenVRSystem::convertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	glm::mat4 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void OpenVRSystem::updateHMDMatrixPose()
{
	if (!m_VRSystem)
		return;

	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	m_iValidPoseCount = 0;
	m_strPoseClasses = "";
	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
		{
			m_iValidPoseCount++;
			m_rmat4DevicePose[nDevice] = convertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			if (m_rDevClassChar[nDevice] == 0)
			{
				switch (m_VRSystem->GetTrackedDeviceClass(nDevice))
				{
				case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
				case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
				case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
				case vr::TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
				case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
				default:                                       m_rDevClassChar[nDevice] = '?'; break;
				}
			}
			m_strPoseClasses += m_rDevClassChar[nDevice];
		}
	}

	if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
		m_mat4HMDPose = glm::inverse(m_mat4HMDPose);
	}
}


void OpenVRSystem::render(omen::Shader* shader, int layer)
{
	m_renderVR = true;
	if (m_VRSystem->IsInputFocusCapturedByAnotherProcess())
	{
		std::cout << "VR Input focus captured by another process, cannot render!" << std::endl;
		return;
	}

	glClearColor(0.33f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_MULTISAMPLE);

	// Left Eye
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_renderFramebufferId);
	Engine::instance()->setViewport(0, 0, m_renderWidth, m_renderHeight);
	renderScene(vr::Eye_Left);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_renderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_resolveFramebufferId);

	glBlitFramebuffer(0, 0, m_renderWidth, m_renderHeight, 0, 0, m_renderWidth, m_renderHeight,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glEnable(GL_MULTISAMPLE);

	// Right Eye
	glClearColor(0.33f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_renderFramebufferId);
	Engine::instance()->setViewport(0, 0, m_renderWidth, m_renderHeight);
	renderScene(vr::Eye_Right);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_renderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_resolveFramebufferId);

	glBlitFramebuffer(0, 0, m_renderWidth, m_renderHeight, 0, 0, m_renderWidth, m_renderHeight,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		

	// Send the buffers to VR headset
	vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_resolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_resolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

	glFinish();

	glClearColor(0.33f, 0.33f, 0.33f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFlush();
	glFinish();
	
	// Update HMD Poses
	updateHMDMatrixPose();

	m_renderVR = false;
	Engine::instance()->setViewport(0, 0, 1920, 1080);
}