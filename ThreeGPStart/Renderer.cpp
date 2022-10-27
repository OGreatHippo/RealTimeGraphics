#include "Renderer.h"

class Mesh;

Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	//glDeleteBuffers(1, &m_VAO);
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	{
		ImGui::Begin("RTG");                    // Create a window called "RTG" and append into it.

		ImGui::Text("Visibility.");             // Display some text (you can use a format strings too)	

		ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::End();
	}
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	// Helpers has an object for loading 3D geometry, supports most types
	
	CreateTerrain(10000);

	Model Jeep("Data\\Models\\Jeep\\jeep.obj", "Data\\Models\\Jeep\\jeep_army.jpg");
	Model Apple("Data\\Models\\Apple\\apple.obj", "Data\\Models\\Apple\\2.jpg");
	Model Apple2("Data\\Models\\Apple\\apple.obj", "Data\\Models\\Bones\\bones.bmp");
	Model Mummy("Data\\Models\\Mummy\\mummy.x", "Data\\Models\\Mummy\\mummy.bmp");
	Model Mummy2("Data\\Models\\Mummy\\mummy.x", "Data\\Models\\AquaPig\\aqua_pig_1k.png");

	models.push_back(Jeep);
	models.push_back(Apple);
	models.push_back(Apple2);
	models.push_back(Mummy);
	models.push_back(Mummy2);

	// Good idea to check for an error now:	
	Helpers::CheckForGLError();
	
	return true;
}

bool Renderer::CreateTerrain(int size)
{
	int numCellsX{ 64 };
	int numCellsZ{ 64 };

	int numVerticesX{ numCellsX + 1 };
	int numVerticesZ{ numCellsZ + 1 };

	int numTrisX{ numCellsX * 2 };
	int numTrisZ{ numCellsZ };

	float cellSizeX{ size / (float)numCellsX };
	float cellSizeZ{ size / (float)numCellsZ };

	float tiles{ 10.0f };

	glm::vec3 startPos{ -size / 2.0f, 0,  size / 2.0f };

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvCoords;

	Model terrain;

	Helpers::ImageLoader heightmap;
	if (!heightmap.Load("Data\\Heightmaps\\curvy.gif"))
		return false;

	unsigned char* texels = (unsigned char*)heightmap.GetData();

	for (int z = 0; z < numVerticesZ; z++)
	{
		for (int x = 0; x < numVerticesX; x++)
		{
			glm::vec3 pos{ startPos };
			pos.x += cellSizeX * x;
			pos.z -= cellSizeZ * z;

			float u{ (float)x / (numVerticesX - 1) };
			float v{ (float)z / (numVerticesZ - 1) };

			int heightMapX = (int)(u * (heightmap.Width() - 1));
			int heightMapY = (int)(v * (heightmap.Height() - 1));

			int offset = (heightMapX + heightMapY * heightmap.Width()) * 4;

			pos.y = texels[offset];

			vertices.push_back(pos);

			u *= tiles;
			v *= tiles;

			uvCoords.push_back(glm::vec2(u, v));
		}
	}

	std::vector<GLuint> elements;

	bool toggle{ true };

	for (GLuint z = 0; z < numCellsZ; z++)
	{
		for (GLuint x = 0; x < numCellsX; x++)
		{
			GLuint startVertex = z * numVerticesX + x;

			if (toggle)
			{
				//Triangle 1
				elements.push_back(startVertex);
				elements.push_back(startVertex + 1);
				elements.push_back(startVertex + 1 + numVerticesX);

				//Triangle 2
				elements.push_back(startVertex);
				elements.push_back(startVertex + 1 + numVerticesX);
				elements.push_back(startVertex + numVerticesX);
			}
			else
			{
				//Triangle 1
				GLuint startVertex = z * numVerticesX + x;
				elements.push_back(startVertex);
				elements.push_back(startVertex + 1);
				elements.push_back(startVertex + numVerticesX);

				//Triangle 2
				elements.push_back(startVertex + 1);
				elements.push_back(startVertex + 1 + numVerticesX);
				elements.push_back(startVertex + numVerticesX);
			}
			toggle = !toggle;
		}
		toggle = !toggle;
	}

	std::vector<glm::vec3> normals(vertices.size());
	std::fill(normals.begin(), normals.end(), glm::vec3(0, 0, 0));

	for (size_t index = 0; index < elements.size(); index += 3)
	{
		int index1 = elements[index];
		int index2 = elements[index + 1];
		int index3 = elements[index + 2];

		glm::vec3 v0 = vertices[index1];
		glm::vec3 v1 = vertices[index2];
		glm::vec3 v2 = vertices[index3];

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		normals[index1] += normal;
		normals[index2] += normal;
		normals[index3] += normal;
	}

	for (glm::vec3& n : normals)
		n = glm::normalize(n);

	GLuint vertexPosVBO;
	glGenBuffers(1, &vertexPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint normalsVBO;
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint texcoordsVBO;
	glGenBuffers(1, &texcoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvCoords.size(), uvCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint elementsVBO;
	glGenBuffers(1, &elementsVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Helpers::Mesh terrainMesh;
	terrainMesh.numElements = elements.size();

	glGenVertexArrays(1, &terrainMesh.vao);
	glBindVertexArray(terrainMesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexPosVBO);
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

	Helpers::ImageLoader grass;

	if (grass.Load("Data\\Textures\\dirt_earth-n-moss_df_.dds"))
	{
		glGenTextures(1, &terrainMesh.tex);
		glBindTexture(GL_TEXTURE_2D, terrainMesh.tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grass.Width(), grass.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, grass.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		terrain.mesh.push_back(terrainMesh);

		models.push_back(terrain);

		return true;
	}

	return false;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{			
	Helpers::CheckForGLError();
	// Configure normal pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 10000.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;
	Helpers::CheckForGLError();
	glUseProgram(m_program);

	// Send the combined matrix to the shader in a uniform

	Helpers::CheckForGLError();

	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	glm::mat4 model_xform = glm::mat4(1);
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	// Send the model matrix to the shader in a uniform

	for (Model& mod : models)
	{
		for (Helpers::Mesh& mesh : mod.mesh)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.tex);
			glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

			glBindVertexArray(mesh.vao);
			glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, (void*)0);
		}
	}

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();
}