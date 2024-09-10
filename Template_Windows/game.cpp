#include "stdafx.h"
#include "game.h"

using namespace wi;

namespace game
{
	GameApplication* GameApplication::AppRef = nullptr;

	Entity GameApplication::entPlaySurface = INVALID_ENTITY;
	Entity GameApplication::entPinBall = INVALID_ENTITY;
	Vector3 GameApplication::locBallStart = Vector3(0.0f, 0.0f, 0.0f);

	void GameRenderPath::Load()
	{
		RenderPath3D::Load();

		//wi::physics::SetDebugDrawEnabled(true);

		MapMeta mapmeta;
		MapLoader::Load("assets/env.wiscene", mapmeta);
		if (mapmeta.entCamera) {
			Camera::SetEntity(mapmeta.entCamera);
		}

		// Load in the static walls
		Scene src;
		LoadModel(src, "assets/static-walls.wiscene");

		// Supply a collision shape for each object
		std::vector<Entity> objects = src.objects.GetEntityArray();
		for (Entity e : objects) {
			RigidBodyPhysicsComponent& body = src.rigidbodies.Create(e);

			// todo: append object name hints like "_coltri" or "_colconvex" as needed
			body.shape = RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH;
			body.SetKinematic(true);
		}
		GetScene().Merge(src);

		// Store the entity representing the playfield
		GameApplication::entPlaySurface = GetScene().Entity_FindByName("static-walls.glb");

		// Lets rotate the play surface
		TransformComponent* transPlayField = GetScene().transforms.GetComponent(GameApplication::entPlaySurface);
		transPlayField->RotateRollPitchYaw(Vector3(-0.4f, 0.0f, 0.0f));

	}

	void GameRenderPath::Update(float deltaTime)
	{
		Camera::Update(deltaTime);

		if (tick == 1) {
			// Load the pinball and place it at its start location
			LoadModel("assets/pinball.wiscene");
			GameApplication::entPinBall = GetScene().Entity_FindByName("PinBall");

			// Determine ball start location
			Entity entBallStart = GetScene().Entity_FindByName("ballstart");
			TransformComponent* transBallStart = GetScene().transforms.GetComponent(entBallStart);
			GameApplication::locBallStart = transBallStart->GetPosition();

			TransformComponent* transBall = GetScene().transforms.GetComponent(GameApplication::entPinBall);
			transBall->translation_local = GameApplication::locBallStart;
			transBall->UpdateTransform();
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
