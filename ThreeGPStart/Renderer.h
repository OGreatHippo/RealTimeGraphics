#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"
#include "Model.h"
#include <string>

struct PointLight
{
	glm::vec3 position;
	glm::vec3 colour;
	float intensity;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
};

struct Material
{
	glm::vec3 specular;
};

class Model;

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	GLuint m_lights{ 0 };

	GLuint m_FXAA{ 0 };
	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };

	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };

	std::vector<Model> models;

	bool m_wireframe{ false };

	bool m_FXAAB{ false };

	GLuint fbo;
	GLuint renderedTexture;
	unsigned int rectVAO, rectVBO;

	bool CreateProgram();
	bool CreateFBO();
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

