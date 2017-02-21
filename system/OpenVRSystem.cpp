#include "OpenVRSystem.h"
#include "../component/Renderer.h"
#include <openvr.h>
#include <iostream>

using namespace omen;
using namespace ecs;

OpenVRSystem::OpenVRSystem()
{
	init();
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

	// Initialize the compositor
	m_Compositor = vr::VRCompositor();

	setupStereoRenderTargets();
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

void OpenVRSystem::renderScene(vr::EVREye eye)
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (auto c : m_components) {
		Renderer* r = dynamic_cast<Renderer*>(c);
		if (r != nullptr)
			//renderers.push_back(r);
			r->render(nullptr);
	}
}

void OpenVRSystem::render(omen::Shader* shader)
{
	if (m_VRSystem->IsInputFocusCapturedByAnotherProcess())
	{
		std::cout << "VR Input focus captured by another process, cannot render!" << std::endl;
		return;
	}

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_MULTISAMPLE);

	// Left Eye
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_renderFramebufferId);
	glViewport(0, 0, m_renderWidth, m_renderHeight);
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
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_renderFramebufferId);
	glViewport(0, 0, m_renderWidth, m_renderHeight);
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

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFlush();
	glFinish();




	// Update HMD Poses
	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	m_iValidPoseCount = 0;
	m_strPoseClasses = "";
	for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
		{
			m_iValidPoseCount++;
			//m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
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
}