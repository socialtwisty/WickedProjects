#pragma once
#include "SimpleMath.h"

using namespace SimpleMath;
using namespace wi::scene;
using namespace wi::ecs;

typedef struct mapmeta_t {
	Vector3 cameraStartLocation;
	Quaternion cameraStartRotation;
	Entity entCamera;
} MapMeta;

class MapLoader
{


public:
	static void Load(std::string scenePath, MapMeta& mapmeta);

	// Load a scene and assign kinematic convex hull shapes to each object
	static void RigidBodyLoad(std::string scenePath);

	static bool LoadAndFind(Entity& ent, std::string entName, std::string scenePath);
};
