#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

struct Mesh
{
	GLuint tex{ 0 };
	GLuint vao{ 0 };
	GLuint numElements{ 0 };
};

struct Model
{
	std::vector<Mesh> mesh;
};

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	bool m_wireframe{ false };

	std::vector<Model> models;

	bool CreateProgram();
	
public:
	Renderer();
	~Renderer();
	void DefineGUI();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	bool CreateTerrain(int size);

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

