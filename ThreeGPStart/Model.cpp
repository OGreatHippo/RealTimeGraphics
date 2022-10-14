#include "Model.h"

Model::Model()
{

}

Model::Model(std::string modelPath, std::string texturePath)
{
	CreateModel(modelPath, texturePath);
}

void Model::CreateModel(std::string modelPath, std::string texturePath)
{
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile(modelPath))
	{
		std::cout << "File not found" << std::endl;
		return;
	}

	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		GLuint verticesVBO;
		glGenBuffers(1, &verticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint texcoordsVBO;
		glGenBuffers(1, &texcoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);

		GLuint elementsVBO;
		glGenBuffers(1, &elementsVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		Helpers::Mesh modelMesh;

		glGenVertexArrays(1, &modelMesh.vao);
		glBindVertexArray(modelMesh.vao);

		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);

		glBindVertexArray(0);

		modelMesh.numElements = mesh.elements.size();

		Helpers::CheckForGLError();

		Helpers::ImageLoader image;

		if (image.Load(texturePath))
		{
			glGenTextures(1, &modelMesh.tex);
			glBindTexture(GL_TEXTURE_2D, modelMesh.tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetData());

			glGenerateMipmap(GL_TEXTURE_2D);


			//FIX THIS

			Model::mesh.push_back(modelMesh);

			//models.push_back(jeep);

			//Renderer::AddModel();
		}
		return;
	}
}

void Model::UpdateModel()
{

}