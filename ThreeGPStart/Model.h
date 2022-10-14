#pragma once
#include "ExternalLibraryHeaders.h"
#include "Mesh.h"
#include "ImageLoader.h"
#include "Renderer.h"

class Model
{
	std::string modelFilePath;
	std::string textureFilePath;

	Model(std::string modelPath, std::string texturePath)
	{
		modelPath = modelFilePath;
		texturePath = textureFilePath;
	}

public :
		std::vector<Helpers::Mesh> mesh;

		void CreateModel(std::string modelPath, std::string texturePath);
		void UpdateModel();
		Model();
};

