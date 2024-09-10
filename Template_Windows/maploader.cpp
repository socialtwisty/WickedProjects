#include "stdafx.h"
#include "maploader.h"

void MapLoader::Load(std::string scenePath, MapMeta& mapmeta) {
	Scene src;
	LoadModel(src, scenePath);


	// Have a look for entities with metadata and a name that is "start"
	// Store its location and rotation for camera start.
	std::vector<Entity> metaEntities = src.metadatas.GetEntityArray();
	mapmeta.cameraStartLocation = Vector3(0, 0, 0);
	mapmeta.cameraStartRotation = Quaternion(0, 0, 0, 0);
	for (Entity e : metaEntities) {
		NameComponent* nameComp = src.names.GetComponent(e);
		if (nameComp) {
			if (nameComp->name == "start") {
				TransformComponent* transComp = src.transforms.GetComponent(e);
				if (transComp) {
					mapmeta.cameraStartLocation = transComp->translation_local;
					mapmeta.cameraStartRotation = transComp->rotation_local;
				}
			}
		}
	}

	// Check if a camera is in the scene. Use the first one we find as the main camera.
	std::vector<Entity> cameraEntities = src.cameras.GetEntityArray();
	mapmeta.entCamera = wi::ecs::INVALID_ENTITY;
	if (cameraEntities.size() >= 1) {
		mapmeta.entCamera = cameraEntities.at(0);
	}

	// Other things..

	// Finally merge source scene into 'main' scene
	GetScene().Merge(src);
}

bool MapLoader::LoadAndFind(Entity& ent, std::string entName, std::string scenePath) {
	Scene src;

	LoadModel(src, scenePath);
	Entity found = src.Entity_FindByName(entName);
	if (found) {
		ent = found;
		GetScene().Merge(src);
		return true;
	}
	return false;
}
