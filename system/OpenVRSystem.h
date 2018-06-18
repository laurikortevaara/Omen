#ifndef _OMEN_OPENVR_SYSTEM_
#define _OMEN_OPENVR_SYSTEM_

#include "GraphicsSystem.h"
#include <openvr.h>
#include <string>
#include <glm/glm.hpp>


typedef uint32_t GLuint;

namespace omen
{
	class Camera;

	namespace ecs
	{
		class OpenVRSystem : public ecs::GraphicsSystem
		{
		public:
			struct FramebufferDesc
			{
				GLuint m_depthBufferId;
				GLuint m_renderTextureId;
				GLuint m_renderFramebufferId;
				GLuint m_resolveTextureId;
				GLuint m_resolveFramebufferId;
			};

			OpenVRSystem();

			virtual void render(omen::Shader* shader = nullptr, int layer = 0);
			virtual void shutDown() { vr::VR_Shutdown(); };

			glm::mat4 getCurrentViewProjectionMatrix(vr::EVREye nEye);
			glm::mat4 getCurrentViewMatrix(vr::EVREye eye);

			bool renderVR() const { return m_renderVR; }
			vr::EVREye currentEye() const { return m_currentEye; }
		protected:
			bool setupStereoRenderTargets();
			void setupCameras();
		private:
			void renderScene(vr::EVREye eye);
			bool init();
			void updateHMDMatrixPose();

			glm::mat4 convertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);
			glm::mat4 getHMDMatrixProjectionEye(vr::EVREye eye);
			glm::mat4 getHMDMatrixPoseEye(vr::EVREye eye);
			

			int m_iValidPoseCount;
			std::string m_strPoseClasses;
			vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
			char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];   // for each device, a character representing its class
			glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

			vr::IVRSystem* m_VRSystem;
			vr::IVRRenderModels* m_RenderModels;
			vr::IVRCompositor* m_Compositor;
			vr::EVREye m_currentEye;
			bool m_renderVR;

			glm::mat4 m_mat4HMDPose;
			glm::mat4 m_mat4eyePosLeft;
			glm::mat4 m_mat4eyePosRight;

			glm::mat4 m_mat4ProjectionCenter;
			glm::mat4 m_mat4ProjectionLeft;
			glm::mat4 m_mat4ProjectionRight;

			FramebufferDesc leftEyeDesc;
			FramebufferDesc rightEyeDesc;

			bool createFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);

			uint32_t m_renderWidth;
			uint32_t m_renderHeight;

			std::string m_strDriver;
			std::string m_strDisplay;
			std::string m_strModelLabel;
			std::string m_strModelNumber;
			std::string m_strManufacturerName;

			Camera* m_camera;
		};
	}
}

#endif //_OMEN_OPENVR_SYSTEM_