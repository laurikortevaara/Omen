#pragma once

#include "GraphicsSystem.h"
#include <openvr.h>
#include <string>


typedef uint32_t GLuint;

namespace omen
{
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

			virtual void render(omen::Shader* shader = nullptr);
		protected:
			bool setupStereoRenderTargets();
		private:
			void renderScene(vr::EVREye eye);
			bool init();

			int m_iValidPoseCount;
			std::string m_strPoseClasses;
			vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
			char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];   // for each device, a character representing its class

			vr::IVRSystem* m_VRSystem;
			vr::IVRRenderModels* m_RenderModels;
			vr::IVRCompositor* m_Compositor;

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
		};
	}
}