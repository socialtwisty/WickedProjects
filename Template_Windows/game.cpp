#include "stdafx.h"
#include "game.h"

using namespace wi;

namespace game
{
	GameApplication* GameApplication::AppRef = nullptr;
	bool moveBall = false;
	long ballMoveFrame = 0;

	void GameRenderPath::Load()
	{
		RenderPath3D::Load();

		//wi::physics::SetDebugDrawEnabled(true);
		this->setBloomEnabled(false);

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
			NameComponent* name = src.names.GetComponent(e);
			if (name) { // assign respective body shapes if "_colconvex" or "_coltri" is found in name
				if (name->name.find("_colconvex") != std::string::npos) {
					body.shape = RigidBodyPhysicsComponent::CollisionShape::CONVEX_HULL;
				} else {
					body.shape = RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH;
				}
			}
			body.mass = 0;
		}
		GetScene().Merge(src);

		// Store the entity representing the playfield
		entPlaySurface = GetScene().Entity_FindByName("static-walls.glb");

		// Bring in the flippers		
		LoadModel("assets/flippers.wiscene");
		HierarchyComponent* hier;

		flipperUpperLeft = GetScene().Entity_FindByName("upper-left");
		RigidBodyPhysicsComponent& b_ul = GetScene().rigidbodies.Create(flipperUpperLeft);
		b_ul.shape = RigidBodyPhysicsComponent::CollisionShape::CONVEX_HULL;
		b_ul.SetKinematic(true);
		hier = GetScene().hierarchy.GetComponent(flipperUpperLeft);
		hier->parentID = entPlaySurface;

		flipperLowerLeft = GetScene().Entity_FindByName("lower-left");
		RigidBodyPhysicsComponent& b_ll = GetScene().rigidbodies.Create(flipperLowerLeft);
		b_ll.shape = RigidBodyPhysicsComponent::CollisionShape::CONVEX_HULL;
		b_ll.SetKinematic(true);
		hier = GetScene().hierarchy.GetComponent(flipperLowerLeft);
		hier->parentID = entPlaySurface;

		flipperUpperRight = GetScene().Entity_FindByName("upper-right");
		RigidBodyPhysicsComponent& b_ur = GetScene().rigidbodies.Create(flipperUpperRight);
		b_ur.shape = RigidBodyPhysicsComponent::CollisionShape::CONVEX_HULL;
		b_ur.SetKinematic(true);
		hier = GetScene().hierarchy.GetComponent(flipperUpperRight);
		hier->parentID = entPlaySurface;

		flipperLowerRight = GetScene().Entity_FindByName("lower-right");
		RigidBodyPhysicsComponent& b_lr = GetScene().rigidbodies.Create(flipperLowerRight);
		b_lr.shape = RigidBodyPhysicsComponent::CollisionShape::CONVEX_HULL;
		b_lr.SetKinematic(true);
		hier = GetScene().hierarchy.GetComponent(flipperLowerRight);
		hier->parentID = entPlaySurface;

		// Now the bell bumpers
		LoadModel("assets/bell-bumpers.wiscene");
		entBumper1 = GetScene().Entity_FindByName("bumper1");
		entBumper2 = GetScene().Entity_FindByName("bumper2");
		entBumper3 = GetScene().Entity_FindByName("bumper3");
		entBumper4 = GetScene().Entity_FindByName("bumper4");
		entBumper1_light = GetScene().Entity_FindByName("bumper1_light");
		entBumper2_light = GetScene().Entity_FindByName("bumper2_light");
		entBumper3_light = GetScene().Entity_FindByName("bumper3_light");
		entBumper4_light = GetScene().Entity_FindByName("bumper4_light");

		(GetScene().hierarchy.GetComponent(entBumper1))->parentID = entPlaySurface;
		(GetScene().hierarchy.GetComponent(entBumper2))->parentID = entPlaySurface;
		(GetScene().hierarchy.GetComponent(entBumper3))->parentID = entPlaySurface;
		(GetScene().hierarchy.GetComponent(entBumper4))->parentID = entPlaySurface;

		// Rotate the play surface so the ball rolls downward
		TransformComponent* transPlayField = GetScene().transforms.GetComponent(entPlaySurface);
		transPlayField->RotateRollPitchYaw(Vector3(-0.35f, 0.0f, 0.0f));		
	}

	void GameRenderPath::Update(float deltaTime)
	{
		Camera::Update(deltaTime);

		if (tick == 1) {
			// Load the pinball and place it at its start location
			LoadModel("assets/pinball.wiscene");
			entPinBall = GetScene().Entity_FindByName("PinBall");

			// Determine ball start location
			Entity entBallStart = GetScene().Entity_FindByName("ballstart");
			TransformComponent* transBallStart = GetScene().transforms.GetComponent(entBallStart);
			locBallStart = transBallStart->GetPosition();

			TransformComponent* transBall = GetScene().transforms.GetComponent(entPinBall);
			transBall->translation_local = locBallStart;
			transBall->UpdateTransform();

			RigidBodyPhysicsComponent* body = GetScene().rigidbodies.GetComponent(entPinBall);
			body->SetStartDeactivated(false);
		}

		if (tick > 1) {
			Vector4 meas;
			meas = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG::GAMEPAD_ANALOG_TRIGGER_L, 0);
			trigL = meas.x;
			meas = wi::input::GetAnalog(wi::input::GAMEPAD_ANALOG::GAMEPAD_ANALOG_TRIGGER_R, 0);
			trigR = meas.x;

			TransformComponent* t_ll = GetScene().transforms.GetComponent(flipperLowerLeft);
			TransformComponent* t_ul = GetScene().transforms.GetComponent(flipperUpperLeft);
			TransformComponent* t_lr = GetScene().transforms.GetComponent(flipperLowerRight);
			TransformComponent* t_ur = GetScene().transforms.GetComponent(flipperUpperRight);

			float l = trigL * (3.14159f / -3.0f);
			float r = trigR * (3.14159f / 3.0f);
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

			TransformComponent* transPinball = GetScene().transforms.GetComponent(entPinBall);
			TransformComponent* transBumper1 = GetScene().transforms.GetComponent(entBumper1);
			TransformComponent* transBumper2 = GetScene().transforms.GetComponent(entBumper2);
			TransformComponent* transBumper3 = GetScene().transforms.GetComponent(entBumper3);
			TransformComponent* transBumper4 = GetScene().transforms.GetComponent(entBumper4);

			// Check if the ball is in contact with a bumper
			wi::primitive::Sphere colPinball, colBumper1, colBumper2, colBumper3, colBumper4;
			
			colPinball.center = transPinball->GetPosition(); colPinball.radius = 0.2f;
			colBumper1.center = transBumper1->GetPosition(); colBumper1.radius = 0.35f;
			colBumper2.center = transBumper2->GetPosition(); colBumper2.radius = 0.35f;
			colBumper3.center = transBumper3->GetPosition(); colBumper3.radius = 0.35f;
			colBumper4.center = transBumper4->GetPosition(); colBumper4.radius = 0.35f;

			LightComponent* lightBumper1 = GetScene().lights.GetComponent(entBumper1_light);
			LightComponent* lightBumper2 = GetScene().lights.GetComponent(entBumper2_light);
			LightComponent* lightBumper3 = GetScene().lights.GetComponent(entBumper3_light);
			LightComponent* lightBumper4 = GetScene().lights.GetComponent(entBumper4_light);
			if (colPinball.intersects(colBumper1)) { lightBumper1->intensity = 8; } else { lightBumper1->intensity = 0; }
			if (colPinball.intersects(colBumper2)) { lightBumper2->intensity = 8; }	else { lightBumper2->intensity = 0; }
			if (colPinball.intersects(colBumper3)) { lightBumper3->intensity = 8; }	else { lightBumper3->intensity = 0; }
			if (colPinball.intersects(colBumper4)) { lightBumper4->intensity = 8; } else { lightBumper4->intensity = 0; }
		}

		if (wi::input::Press(wi::input::KEYBOARD_BUTTON_SPACE)) {
			// todo: Reset the ball's position
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
