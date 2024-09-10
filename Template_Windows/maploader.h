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

	static bool LoadAndFind(Entity& ent, std::string entName, std::string scenePath);
};
