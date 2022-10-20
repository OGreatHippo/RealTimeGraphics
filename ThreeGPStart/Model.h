#pragma once
#include "ExternalLibraryHeaders.h"
#include "Mesh.h"
#include "ImageLoader.h"
#include "Renderer.h"

class Model
{
public :
		std::vector<Helpers::Mesh> mesh;

		Model(std::string modelPath, std::string texturePath);

		void CreateModel(std::string modelPath, std::string texturePath);
		void UpdateModel();
		Model();
};

