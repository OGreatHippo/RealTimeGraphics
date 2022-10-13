#pragma once
#include "ExternalLibraryHeaders.h"
#include "Mesh.h"
#include "ImageLoader.h"
#include "Renderer.h"

class Model
{
	std::string modelFilePath;
	std::string textureFilePath;
	std::vector<Helpers::Mesh> mesh;

	Model(std::string modelPath, std::string texturePath)
	{
		modelPath = modelFilePath;
		texturePath = textureFilePath;
	}

	Model();

public :
		void CreateModel(std::string modelPath, std::string texturePath);
		void UpdateModel();
};

