#pragma once

#include "stdafx.h"
#include "maploader.h"
#include "camera.h"

namespace game
{
	static long tick = 0;
	static Entity flipperUpperLeft, flipperLowerLeft;
	static Entity flipperUpperRight, flipperLowerRight;
	static Entity entPlaySurface;
	static Entity entPinBall;
	static Vector3 locBallStart;
	static Entity entBumper1, entBumper1_light;
	static Entity entBumper2, entBumper2_light;
	static Entity entBumper3, entBumper3_light;
	static Entity entBumper4, entBumper4_light;
	static float trigL;
	static float trigR;
	static Quaternion flipperLeft;
	static Quaternion flipperRight;

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
		GameRenderPath renderPath;

		void Initialize() override;
		void Compose(wi::graphics::CommandList cmd) override;
	};

}
