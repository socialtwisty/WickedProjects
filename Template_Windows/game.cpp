#include "stdafx.h"
#include "game.h"

using namespace wi;

namespace game
{
	GameApplication* GameApplication::AppRef = nullptr;

	void GameRenderPath::Load()
	{
		RenderPath3D::Load();

		MapMeta mapmeta;
		MapLoader::Load("assets/env.wiscene", mapmeta);
		if (mapmeta.entCamera) {
			Camera::SetEntity(mapmeta.entCamera);
		}

		// modify loader so each object is rigid body
		LoadModel("assets/static-walls.wiscene");

	}

	void GameRenderPath::Update(float deltaTime)
	{
		Camera::Update(deltaTime);

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
