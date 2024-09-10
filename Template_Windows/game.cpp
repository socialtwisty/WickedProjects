#include "stdafx.h"
#include "game.h"

using namespace wi;

namespace game
{
	GameApplication* GameApplication::AppRef = nullptr;

	void GameRenderPath::Load()
	{
		RenderPath3D::Load();

		LoadModel("content/pathfinding/pathtest.wiscene");
		MapMeta mapmeta;
		MapLoader::Load("content/level/testmap1.wiscene", mapmeta);
		if (mapmeta.entCamera) {
			//camera = GetScene().cameras.GetComponent(mapmeta.entCamera);
			Camera::SetEntity(mapmeta.entCamera);
		}

		PathFinder::LoadGridFromScene(grid, "content/pathfinding/pathtest.wiscene");

		// Load in the markers
		MapLoader::LoadAndFind(entStart, "start", "content/pathfinding/start-marker.wiscene");
		MapLoader::LoadAndFind(entGoal, "goal", "content/pathfinding/goal-marker.wiscene");
		MapLoader::LoadAndFind(entPos, "pos", "content/pathfinding/pos-marker.wiscene");

		// Place them
		Entity wpStart = GetScene().Entity_FindByName("wp_start");
		TransformComponent* wpStartTrans = GetScene().transforms.GetComponent(wpStart);
		wpStartPos = wpStartTrans->GetPosition();
		TransformComponent* markerStartTrans = GetScene().transforms.GetComponent(entStart);
		markerStartTrans->translation_local = wpStartPos;
		markerStartTrans->UpdateTransform();

		Entity wpGoal = GetScene().Entity_FindByName("wp_goal");
		TransformComponent* wpGoalTrans = GetScene().transforms.GetComponent(wpGoal);
		wpGoalPos = wpGoalTrans->GetPosition();
		TransformComponent* markerGoalTrans = GetScene().transforms.GetComponent(entGoal);
		markerGoalTrans->translation_local = wpGoalPos;
		markerGoalTrans->UpdateTransform();

	}

	void GameRenderPath::Update(float deltaTime)
	{
		Camera::Update(deltaTime);

		if (tick == 1) {
			path.agent_height = 1;
			path.agent_width = 1;
			path.debug_waypoints = true;

			path.process(wpStartPos, wpGoalPos, grid);
			bool success = path.is_succesful();

			wi::texturehelper::Initialize();
			bool result = wi::texturehelper::CreateTexture(tex, texData, 400, 300);
			int dummy = 0;

		}

		if (tick > 1) {			
			size_t numWaypoints = path.get_waypoint_count();
			if (wpIndex >= numWaypoints) {
				wpIndex = 0;
			}

			Vector3 pos = path.get_waypoint(wpIndex);
			TransformComponent* t = GetScene().transforms.GetComponent(entPos);
			t->translation_local = pos;
			t->UpdateTransform();
			++wpIndex;
		}


		if (debugNavdraw) {
			wi::renderer::DrawVoxelGrid(&grid);
		}

		++tick;
		RenderPath3D::Update(deltaTime);
	}

	void GameRenderPath::ResizeLayout()
	{
		RenderPath3D::ResizeLayout();
	}

	void GameRenderPath::Render() const
	{
		RenderPath3D::Render();
	}

	void GameApplication::Initialize()
	{
		AppRef = this;
		Application::Initialize();

		infoDisplay.active = true;
		infoDisplay.watermark = true;
		infoDisplay.fpsinfo = true;

		renderPath.init(canvas);
		renderPath.Load();

		ActivatePath(&renderPath);

	}
	void GameApplication::Compose(wi::graphics::CommandList cmd)
	{
		Application::Compose(cmd);
	}
}
