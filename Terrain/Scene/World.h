#pragma once

#include "d3dclass.h"

#include <Controls/Input.h>
#include <ShaderCode/ShaderManager.h>
#include <UI/UserInterface.h>
#include <Camera/Camera.h>
#include <Camera/Position.h>
#include <Scene/Drawable\Land.h>

class World
{
public:
	World();
	World(const World&);
	~World();

	bool Initialize(D3DClass*, HWND, int, int, float);
	void Shutdown();
	bool Frame(D3DClass*, Input*, ShaderManager*, float, int);

private:
	void HandleMovementInput(Input*, float);
	bool Render(D3DClass*, ShaderManager*);

private:
	UserInterface* m_UserInterface;
	Camera* m_Camera;
	Position* m_Position;
	Land* m_Terrain;
	bool m_displayUI, m_wireFrame;
};