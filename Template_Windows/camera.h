#pragma once

#include "SimpleMath.h"
#include "game.h"

using namespace wi::ecs;
using namespace wi::scene;
using namespace SimpleMath;

class Camera
{
	static bool mouseCaptured;
	static float mouseSensitivity;

	static float speed;
	static float pitch;
	static float yaw;
	static Vector4 mousePos;
	static Vector3 cam_move;

	static Entity entCurrentCamera;
public:
	static void SetEntity(Entity entCamera);
	static void Update(float dt);
};

