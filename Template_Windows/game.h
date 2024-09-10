#pragma once

#include "stdafx.h"
#include "maploader.h"
#include "camera.h"
#include "pathfinder.h"

namespace game
{
	static long tick = 0;
	static wi::VoxelGrid grid;
	static bool debugNavdraw = true;

	static Entity entStart, entGoal, entPos;
	static Vector3 wpStartPos, wpGoalPos;

	static wi::PathQuery path;
	static int wpIndex = 0;

	static wi::graphics::Texture tex;
	static uint8_t texData[400 * 300 * 4];


	class GameRenderPath : public wi::RenderPath3D
	{
	public:
		void Load() override;
		void Update(float deltaTime) override;
		void ResizeLayout() override;
		void Render() const override;
	};

	class GameApplication : public wi::Application
	{

	public:
		static GameApplication* AppRef;
		GameRenderPath renderPath; // temporary public

		GameRenderPath renderPath2;

		void Initialize() override;
		void Compose(wi::graphics::CommandList cmd) override;
	};

}
