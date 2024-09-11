#include "stdafx.h"
#include "game.h"

using namespace wi;

namespace game
{
	GameApplication* GameApplication::AppRef = nullptr;

	Entity GameApplication::entPlaySurface = INVALID_ENTITY;
	Entity GameApplication::entPinBall = INVALID_ENTITY;
	Vector3 GameApplication::locBallStart = Vector3(0.0f, 0.0f, 0.0f);

	float GameApplication::trigL = 0.0f;
	float GameApplication::trigR = 0.0f;
	Quaternion GameApplication::flipperLeft = Quaternion(0, 0, 0, 0);
	Quaternion GameApplication::flipperRight = Quaternion(0, 0, 0, 0);

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

		// Bring in the flippers
		
		LoadModel("assets/flippers.wiscene");
		HierarchyComponent* p;

		game::flipperUpperLeft = GetScene().Entity_FindByName("upper-left");
		RigidBodyPhysicsComponent& b_ul = GetScene().rigidbodies.Create(game::flipperUpperLeft);
		b_ul.shape = RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH;
		b_ul.SetKinematic(true);
		p = GetScene().hierarchy.GetComponent(game::flipperUpperLeft);
		p->parentID = GameApplication::entPlaySurface;

		game::flipperLowerLeft = GetScene().Entity_FindByName("lower-left");
		RigidBodyPhysicsComponent& b_ll = GetScene().rigidbodies.Create(game::flipperLowerLeft);
		b_ll.shape = RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH;
		b_ll.SetKinematic(true);
		p = GetScene().hierarchy.GetComponent(game::flipperLowerLeft);
		p->parentID = GameApplication::entPlaySurface;

		game::flipperUpperRight = GetScene().Entity_FindByName("upper-right");
		RigidBodyPhysicsComponent& b_ur = GetScene().rigidbodies.Create(game::flipperUpperRight);
		b_ur.shape = RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH;
		b_ur.SetKinematic(true);
		p = GetScene().hierarchy.GetComponent(game::flipperUpperRight);
		p->parentID = GameApplication::entPlaySurface;

		game::flipperLowerRight = GetScene().Entity_FindByName("lower-right");
		RigidBodyPhysicsComponent& b_lr = GetScene().rigidbodies.Create(game::flipperLowerRight);
		b_lr.shape = RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH;
		b_lr.SetKinematic(true);
		p = GetScene().hierarchy.GetComponent(game::flipperLowerRight);
		p->parentID = GameApplication::entPlaySurface;

		// Lets rotate the play surface
		TransformComponent* transPlayField = GetScene().transforms.GetComponent(GameApplication::entPlaySurface);
		transPlayField->RotateRollPitchYaw(Vector3(-0.3f, 0.0f, 0.0f));

		
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

		if (tick > 1) {
			Vector4 meas;
			meas = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG::GAMEPAD_ANALOG_TRIGGER_L, 0);
			GameApplication::trigL = meas.x;
			meas = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG::GAMEPAD_ANALOG_TRIGGER_R, 0);
			GameApplication::trigR = meas.x;

			TransformComponent* t_ll = GetScene().transforms.GetComponent(game::flipperLowerLeft);
			TransformComponent* t_ul = GetScene().transforms.GetComponent(game::flipperUpperLeft);
			TransformComponent* t_lr = GetScene().transforms.GetComponent(game::flipperLowerRight);
			TransformComponent* t_ur = GetScene().transforms.GetComponent(game::flipperUpperRight);

			float l = GameApplication::trigL * (3.14159f / -3.0f);
			float r = GameApplication::trigR * (3.14159f / 3.0f);
			Quaternion rot_l = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), l);
			Quaternion rot_r = Quaternion::CreateFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), r);

			t_ll->rotation_local = rot_l;
			t_ul->rotation_local = rot_l;
			t_lr->rotation_local = rot_r;
			t_ur->rotation_local = rot_r;

			t_ll->UpdateTransform();
			t_ul->UpdateTransform();
			t_lr->UpdateTransform();
			t_ur->UpdateTransform();

			if (wi::input::Press(wi::input::KEYBOARD_BUTTON_SPACE)) {
				TransformComponent* transBall = GetScene().transforms.GetComponent(GameApplication::entPinBall);
				transBall->translation_local = GameApplication::locBallStart;
				transBall->UpdateTransform();


				GetScene().Clear();
				GameRenderPath::Load();
				tick = 0;
			}

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
