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

	GLuint m_DOF{ 0 };

	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };

	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };

	std::vector<Model> models;

	bool m_wireframe{ false };

	bool m_FXAAB{ false };

	bool m_DOFB{ false };

	GLuint fbo;
	GLuint renderedTexture;
	GLuint dofDepthTexture;
	GLuint dofColourTexture;
	GLuint depthrenderbuffer;

	GLfloat nearPlane;
	GLfloat farPlane;

	const float sliderStep = 1.0f;

	GLfloat nearV = 1.0f;
	GLfloat farV = 3000.0f;

	bool CreateProgram();
	bool CreateFBO();

	float distance(glm::vec3 models, glm::vec3 camera);
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

