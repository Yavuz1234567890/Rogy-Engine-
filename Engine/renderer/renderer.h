#ifndef RENDER_ENGINE
#define RENDER_ENGINE

#include <iostream>
#include <string>
#include "SOIL.h"
#include "glm\glm.hpp"

#include "../camera/Camera.h"
#include "../camera/camera_frustum.h"
#include "../camera/frustum.h"

#include "../resources/resources_manger.h"
#include "../resources/material_library.h"

#include "render_buffer.h"
#include "shadow_map.h"
#include "reflection_probe.h"
#include "renderCache.h"
#include "billboardInfo.h"
#include "lightmapSettings.h"
#include "billboardComponent.h"
#include "cameraComponent.h"
#include "rendererComponent.h"
#include "GrassComponent.h"

#include "postprocessing.h"
#include "IBL.h"
#include "ParticleSystem.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	// Window Size Settings
	// ---------------------------------
	GLFWwindow* window;
	int SCR_weight, SCR_height;

	// Environment Settings
	// ---------------------------------
	bool skyTexChange = false;
	std::string new_skyPath;
	std::string SkyPath;
	GLuint envCubemap;
	PBRCapture m_SkyCapture;
	glm::vec3 ClearColor = glm::vec3(0.2f, 0.3f, 0.3f);
	glm::vec3 AmbientColor;
	int fps;
	std::string SceneName;

	float AmbientLevel = 1.0f;

	unsigned int m_FramebufferCubemap;
	unsigned int m_CubemapDepthRBO;

	// Post Processor, resourcesManager, ibl baker
	// ---------------------------------
	ResourcesManager* resManager;
	RPostProcessing postProc;
	IBL ibl;
	RCache m_cache;

	// Fog effect
	// ---------------------------------
	bool usefog = false;
	glm::vec3 fogColor = glm::vec3(1.0f);
	float fogNear = 40.0f;
	float fogFar = 300.0f;

	// Material Library
	// ---------------------------------
	MaterialLibrary materials;

	Material* CreateMaterial(const char* mName);
	bool CreateMaterialOnDisk(const char* mPath);
	Material* LoadMaterial(const char* mPath);
	bool SaveMaterial(Material* mat, const char* mPath);

	// Render commands queue
	// ---------------------------------
	ComponentArray<RendererComponent> m_renderers;
	RenderBuffer m_RenderBuffer;
	
	// Camera
	// ---------------------------------
	Camera MainCam;
	Frustum frustum;
	bool update_frustum = true;

	// Game Camera
	// ---------------------------------
	ComponentArray<CameraComponent> m_cameras;
	bool use_GameView = false;

	// Lightmapping
	// ---------------------------------
	LightmapSettings m_LightmapSettings;
	bool BakeLighting = false;

	// Lightmaps managment
	std::vector<Texture*> mlightmaps;
	Texture* Get_Lightmap(std::string ppath);
	void Clear_Lightmaps();

	// Lighting
	// ---------------------------------
	DirectionalLight*		 m_DirectionalLight = nullptr;
	std::vector<PointLight*> m_PointLights;
	std::vector<SpotLight*>  m_SpotLights;

	ShadowMapper      m_ShadowMapper;
	PointShadowMapper m_PointShadowMapper;
	SpotShadowMapper  m_SpotShadowMapper;

	PointLight* CreatePointLight(EnttID ent_id);
	bool        RemovePointLight(EnttID ent_id);
	PointLight* GetPointLight(EnttID ent_id);

	SpotLight* CreateSpotLight(EnttID ent_id);
	bool       RemoveSpotLight(EnttID ent_id);
	SpotLight* GetSpotLight(EnttID ent_id);

	DirectionalLight* CreateDirectionalLight();
	bool              RemoveDirectionalLight();

	// Reflection Probe
	// ---------------------------------
	std::vector<ReflectionProbe*> m_ReflectionProbes;

	ReflectionProbe* CreateReflectionProbe(EnttID ent_id);
	bool             RemoveReflectionProbe(EnttID ent_id);
	ReflectionProbe* GetReflectionProbe(EnttID ent_id);

	void UpdateReflectionProbes();
	void BakeReflectionProbes();
	GLuint RenderToCubemap(glm::vec3 position, float resolution, float nearPlane, float farPlane, bool static_only = false);

	// Billbroad Render queue
	// ---------------------------------
	std::vector<BillboardComponent*> r_billboards;
	BillboardComponent* CreateBillboard(EnttID ent);
	bool RemoveBillboard(EnttID ent);

	std::vector<BillboardInfo*> m_Billboards;
	void AddBillbroadInfo(glm::vec3 pos, glm::vec2 size, const char* texture_path, bool depth_test = true, glm::vec3 color = glm::vec3(1.0f), bool tex_mask = false);
	void RenderBillboards(Camera& cam);

	// Partical Systems
	// ---------------------------------
	ComponentArray<ParticleSystem> mParticals;
	void RenderParticals(Camera& cam, float dt);

	// Grass
	// ---------------------------------
	ComponentArray<GrassComponent> mGrass;
	void RenderGrass(Camera& cam, float dt);

	// Rendering Functions
	// ---------------------------------
	void Initialize(int weight, int height, GLFWwindow* wind, ResourcesManager* resM);

	void RemoveAllLights();

	void SetEnv_SkyCapture(std::string path);

	// Render a Render command directly; without pushing it to the renderer queue.
	void RenderMesh(Mesh* mesh, Material* material, glm::mat4 transform);

	// Push a render command to draw in the next frame.
	void PushRender(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows = true, glm::vec3 pos = glm::vec3(0), bool is_static = false, std::string lmPath = "");

	// Change what we need to change when screen size changed.
	void OnViewportResize(int pos, int top, int weight, int height);

	void UpdateGameCamera();

	// Render a Frame, Renders all the pushed render commands and environment map
	void RenderFrame(float dt);

	// End Render the frame, call this after RenderFrame(), 
	// may be you need to draw debug gizmos before rendering ends.
	void EndFrame();

	void RenderScene(Camera& cam, bool static_only = false, GLuint target_frambuffer = 0, int resolu = 0);

	void checkForChanges();

	// Clear All Data
	// ---------------------------------
	void Clear();

	// Lightmap Baking
	// ---------------------------------
	void BakeLightMaps(int meshIndex);
	bool BakeSceneLightmaps();
	void RenderForLighmap(float* view, float* proj);
	bool isLightBakingSucceed();
	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(string(SER_VERSION));
		ar(SkyPath);
		ar(postProc.Use);
		ar(postProc);
		ar(usefog);
		ar(fogColor.x, fogColor.y, fogColor.z);
		ar(fogNear);
		ar(fogFar);
		ar(AmbientLevel);
	}

private:
	void ReIndexPointLightsShadowMaps();
	void ReIndexSpotLightsShadowMaps();
	int last_used_id = 0;
	bool on_env_map_changed = false;
	int left_scr_pos;
	int lm_Count = 0;
	bool bakingSucceed = false;
	std::vector<size_t> not_visible;
	//GLuint spot_depthFBO;
	//GLuint spot_depth;
	//glm::mat4 spotMVP;
};


#endif