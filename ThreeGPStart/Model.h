#pragma once
#include "ExternalLibraryHeaders.h"
#include "Mesh.h"
#include "ImageLoader.h"
#include "Renderer.h"
#include <glm/glm.hpp>

class Model
{
public :
		std::vector<Helpers::Mesh> mesh;

		glm::mat4 modelMatrix = glm::mat4(1);

		Model(std::string modelPath, std::string texturePath);

		void CreateModel(std::string modelPath, std::string texturePath);
		Model();

		void transformModel(glm::vec3 position);
		void scaleModel(glm::vec3 scale);
};

