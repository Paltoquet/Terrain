#pragma once

#include "d3dclass.h"

#include <Controls/Input.h>
#include <ShaderCode/ShaderManager.h>
#include <ImageProcessing/TextureManager.h>
#include <UI/UserInterface.h>
#include <Camera/Camera.h>
#include <Camera/Position.h>
#include <Scene/Drawable/Land.h>
#include <Scene/Drawable/Skydome.h>
#include <Scene/Light.h>

class World
{
public:
	World();
	World(const World&);
	~World();

	bool Initialize(D3DClass*, HWND, int, int, float);
	void Shutdown();
	bool Frame(D3DClass*, Input*, ShaderManager*, TextureManager*, float, int);

private:
	void HandleMovementInput(Input*, float);
	bool Render(D3DClass*, ShaderManager*, TextureManager*);

private:
	UserInterface* m_UserInterface;
	Camera* m_Camera;
	Position* m_Position;
	Land* m_Terrain;
	SkyDome* m_SkyDome;
	Light* m_Light;
	bool m_displayUI, m_wireFrame;
};