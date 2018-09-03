######################################################################
# omen Game engine project
#
# Lauri Kortevaara (2015-2018)
# lauri.kortevaara@gmail.com
#
# https://github.com/laurikortevaara/omen
#
######################################################################

######################################################################
# Define the source files and the executable
######################################################################
set(SOURCE_FILES
${SOURCE_FILES}
# Main sources and headers
${CMAKE_CURRENT_SOURCE_DIR}/editor/EditorWindow.cpp
${CMAKE_CURRENT_SOURCE_DIR}/editor/EditorCameraView.cpp
${CMAKE_CURRENT_SOURCE_DIR}/editor/GUILayout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/editor/EditorScene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/editor/WindowDivider.cpp

${CMAKE_CURRENT_SOURCE_DIR}/AudioSystem.cpp
${CMAKE_CURRENT_SOURCE_DIR}/BoundingBox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/BoundingVolume.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Engine.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp
${CMAKE_CURRENT_SOURCE_DIR}/FBXLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/GameObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Light.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
${CMAKE_CURRENT_SOURCE_DIR}/MD3Loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp
${CMAKE_CURRENT_SOURCE_DIR}/MeshProvider.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp
${CMAKE_CURRENT_SOURCE_DIR}/MultipassShader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Object.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Ocean.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Octree.cpp
${CMAKE_CURRENT_SOURCE_DIR}/OctreeRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/PointLight.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Renderable.cpp
${CMAKE_CURRENT_SOURCE_DIR}/RenderBuffer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ShadowMap.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sky.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SkyRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Texture.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Triangle.cpp
${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Vertex.cpp
${CMAKE_CURRENT_SOURCE_DIR}/VertexArrayObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/VertexBufferObject.cpp
${CMAKE_CURRENT_SOURCE_DIR}/WavefrontLoader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Property.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Signal.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Ocean.cpp
${CMAKE_CURRENT_SOURCE_DIR}/FFT.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Skybox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/SkyboxRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/StringTools.cpp

#/component/
${CMAKE_CURRENT_SOURCE_DIR}/component/BorderSprite.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/CameraController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Clickable.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Component.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Draggable.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/JoystickInput.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/KeyboardInput.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/MeshAnimator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/MeshController.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/MeshRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/MouseInput.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Picker.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/PlainColorRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Renderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Script.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Sprite.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/SpriteRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/TextRenderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Transform.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/Transformer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/component/EditorSceneRenderer.cpp

# /system/
${CMAKE_CURRENT_SOURCE_DIR}/system/CoreSystem.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/GraphicsSystem.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/InputSystem.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/PhysicsSystem.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/System.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/OpenVRSystem.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/

# /ui/
${CMAKE_CURRENT_SOURCE_DIR}/ui/Button.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/FrameLayout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/ImageView.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/Layout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/LinearLayout.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/Slider.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/TextView.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ui/View.cpp
PARENT_SCOPE)
