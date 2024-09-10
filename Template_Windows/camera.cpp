#include "stdafx.h"
#include "camera.h"

Entity Camera::entCurrentCamera = wi::ecs::INVALID_ENTITY;
bool Camera::mouseCaptured = true;
float Camera::mouseSensitivity = 0.005f;
float Camera::pitch = 0.0f;
float Camera::yaw = 0.0f;
float Camera::speed = 2.0f;
Vector3 Camera::cam_move(0, 0, 0);
Vector4 Camera::mousePos(0, 0, 0, 0);

void Camera::SetEntity(Entity entCamera) {
	if (entCamera) {
		entCurrentCamera = entCamera;
		game::GameApplication::AppRef->renderPath.camera = GetScene().cameras.GetComponent(entCamera);
	} else {
		// Useful any time a camera is added as current camera becomes invalidated.
		game::GameApplication::AppRef->renderPath.camera = GetScene().cameras.GetComponent(entCurrentCamera);
	}
}

void Camera::Update(float dt) {
	if (!entCurrentCamera) return; // bail if camera is not defined

	float thisSpeed = speed;

	if (wi::input::Press(wi::input::KEYBOARD_BUTTON_ESCAPE)) {
		mouseCaptured = !mouseCaptured;

		if (mouseCaptured) {
			mousePos = wi::input::GetPointer();
		}
		wi::input::HidePointer(mouseCaptured);
	}
	if (mouseCaptured) {
		wi::input::SetPointer(mousePos);
	}


	if (wi::input::Down(wi::input::KEYBOARD_BUTTON_LSHIFT)) {
		thisSpeed = speed * 3.0f;
	}

	float cdt = std::min(dt, 0.1f);
	float dMouseX = 0, dMouseY = 0;

	if (mouseCaptured) {
		dMouseX = wi::input::GetMouseState().delta_position.x * mouseSensitivity;
		dMouseY = wi::input::GetMouseState().delta_position.y * mouseSensitivity;
	}

	Vector3 move = cam_move;
	Vector3 moveNew(0.0f, 0.0f, 0.0f);

	if (wi::input::Down((wi::input::BUTTON)'W')) {
		moveNew += Vector3(0.0f, 0.0f, 1.0f);
	}
	if (wi::input::Down((wi::input::BUTTON)'S')) {
		moveNew += Vector3(0.0f, 0.0f, -1.0f);
	}
	if (wi::input::Down((wi::input::BUTTON)'A')) {
		moveNew += Vector3(-1.0f, 0.0f, 0.0f);
	}
	if (wi::input::Down((wi::input::BUTTON)'D')) {
		moveNew += Vector3(1.0f, 0.0f, 0.0f);
	}
	if (wi::input::Down((wi::input::BUTTON)'Q')) {
		moveNew += Vector3(0.0f, -1.0f, 0.0f);
	}
	if (wi::input::Down((wi::input::BUTTON)'E')) {
		moveNew += Vector3(0.0f, 1.0f, 0.0f);
	}

	moveNew.Normalize();
	moveNew *= thisSpeed * cdt;

	move = Vector3::Lerp(move, moveNew, std::clamp(0.18f * cdt * 60.0f, 0.0f, 1.0f));

	float moveLength = move.Length();
	if (moveLength < 0.0001f) {
		move = Vector3(0, 0, 0);
	}

	pitch += dMouseY;
	yaw += dMouseX;

	TransformComponent* transcomp = GetScene().transforms.GetComponent(entCurrentCamera);
	SimpleMath::Matrix camRot = SimpleMath::Matrix::CreateFromQuaternion(transcomp->rotation_local);
	Vector3 move_rot = Vector3::TransformNormal(move, camRot);
	transcomp->Translate(move_rot);
	transcomp->RotateRollPitchYaw(Vector3(dMouseY, dMouseX, 0.0f));
	transcomp->UpdateTransform();

	cam_move = move;
}
