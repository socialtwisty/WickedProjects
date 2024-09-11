#pragma once

#include "stdafx.h"
#include "maploader.h"
#include "camera.h"

namespace game
{
	static long tick = 0;
	static wi::VoxelGrid grid;
	static bool debugNavdraw = true;

	static Entity entStart, entGoal, entPos;
	static Vector3 wpStartPos, wpGoalPos;

	static wi::PathQuery path;
	static int wpIndex = 0;

	static Entity flipperUpperLeft, flipperLowerLeft;
	static Entity flipperUpperRight, flipperLowerRight;

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
		static float trigL;
		static float trigR;

		static Quaternion flipperLeft;
		static Quaternion flipperRight;

		static GameApplication* AppRef;
		GameRenderPath renderPath;

		static Entity entPlaySurface;
		static Entity entPinBall;
		static Vector3 locBallStart;

		void Initialize() override;
		void Compose(wi::graphics::CommandList cmd) override;
	};

}
