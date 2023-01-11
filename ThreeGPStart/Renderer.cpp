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
	glDeleteProgram(m_lights);
	glDeleteProgram(m_ShadowM);
	glDeleteProgram(m_FXAA);
	glDeleteProgram(m_DOF);
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

		ImGui::Checkbox("FXAA", &m_FXAAB);

		ImGui::Checkbox("DOF", &m_DOFB);

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

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	m_lights = glCreateProgram();

	GLuint LightVS{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/LightVS.glsl") };
	GLuint LightFS{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/LightFS.glsl") };
	if (LightVS == 0 || LightFS == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_lights, LightVS);

	// Attach the fragment shader (copies it)
	glAttachShader(m_lights, LightFS);

	// Done with the originals of these as we have made copies
	glDeleteShader(LightVS);
	glDeleteShader(LightFS);

	if (!Helpers::LinkProgramShaders(m_lights))
		return false;

	//m_ShadowM = glCreateProgram();

	//GLuint ShadowMVS{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/ShadowMappingVS.glsl") };
	//GLuint ShadowMFS{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/ShadowMappingFS.glsl") };
	//GLuint ShadowMGS{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/ShadowMappingGS.glsl") };
	//if (ShadowMVS == 0 || ShadowMFS == 0 || ShadowMGS == 0)
	//	return false;

	//// Attach the vertex shader to this program (copies it)
	//glAttachShader(m_ShadowM, ShadowMVS);

	//// Attach the fragment shader (copies it)
	//glAttachShader(m_ShadowM, ShadowMFS);

	//// Attach the geometry shader (copies it)
	//glAttachShader(m_ShadowM, ShadowMGS);

	//// Done with the originals of these as we have made copies
	//glDeleteShader(ShadowMVS);
	//glDeleteShader(ShadowMFS);
	//glDeleteShader(ShadowMGS);

	//if (!Helpers::LinkProgramShaders(m_ShadowM))
	//	return false;

	m_FXAA = glCreateProgram();

	GLuint FXAAVS{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/FXAAVS.glsl") };
	GLuint FXAAFS{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/FXAAFS.glsl") };
	if (FXAAVS == 0 || FXAAFS == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_FXAA, FXAAVS);

	// Attach the fragment shader (copies it)
	glAttachShader(m_FXAA, FXAAFS);

	// Done with the originals of these as we have made copies
	glDeleteShader(FXAAVS);
	glDeleteShader(FXAAFS);

	if (!Helpers::LinkProgramShaders(m_FXAA))
		return false;

	m_DOF = glCreateProgram();

	GLuint DOFVS{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/DOFVS.glsl") };
	GLuint DOFFS{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/DOFFS.glsl") };
	if (DOFVS == 0 || DOFFS == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_DOF, DOFVS);

	// Attach the fragment shader (copies it)
	glAttachShader(m_DOF, DOFFS);

	// Done with the originals of these as we have made copies
	glDeleteShader(DOFVS);
	glDeleteShader(DOFFS);

	if (!Helpers::LinkProgramShaders(m_DOF))
		return false;

	return !Helpers::CheckForGLError();
}

bool Renderer::CreateFBO()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &renderedTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glGenTextures(1, &dofDepthTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dofDepthTexture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dofDepthTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glGenTextures(1, &dofColourTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dofColourTexture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, dofColourTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 1280, 720, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	// The depth buffer
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(1, drawBuffers);

	return !Helpers::CheckForGLError();
}

float Renderer::distance(glm::vec3 models, glm::vec3 camera)
{
	float d = sqrt(pow(camera.x - models.x, 2) + pow(camera.y - models.y, 2) + pow(camera.z - models.z, 2) * 1.0);
	return d;
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	CreateFBO();

	// Helpers has an object for loading 3D geometry, supports most types
	
	CreateTerrain(10000);

	Model Jeep("Data\\Models\\Jeep\\jeep.obj", "Data\\Models\\Jeep\\jeep_army.jpg");
	Model Apple("Data\\Models\\Apple\\apple.obj", "Data\\Models\\Apple\\2.jpg");
	Model Apple2("Data\\Models\\Apple\\apple.obj", "Data\\Models\\Bones\\bones.bmp");
	Model Mummy("Data\\Models\\Mummy\\mummy.x", "Data\\Models\\Mummy\\mummy.bmp");
	Model Mummy2("Data\\Models\\Mummy\\mummy.x", "Data\\Models\\AquaPig\\aqua_pig_1k.png");

	Mummy.transformModel(glm::vec3(4000, 75, 400));
	Mummy2.transformModel(glm::vec3(4000, 75, -400));

	glm::vec3 mummyScale = glm::vec3(80);

	Mummy.scaleModel(mummyScale);
	Mummy2.scaleModel(mummyScale);

	Apple.transformModel(glm::vec3(0, 200, 1700));
	Apple2.transformModel(glm::vec3(0, 200, -1700));

	models.push_back(Jeep);
	models.push_back(Apple);
	models.push_back(Apple2);
	models.push_back(Mummy);
	models.push_back(Mummy2);

	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	return !Helpers::CheckForGLError();
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

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glActiveTexture(GL_TEXTURE0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	// Configure normal pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 20000.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;

	Helpers::CheckForGLError();

	glUseProgram(m_program);

	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	glm::mat4 model_xform = glm::mat4(1);
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");

	for (Model& mod : models)
	{
		model_xform = mod.modelMatrix;

		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

		for (Helpers::Mesh& mesh : mod.mesh)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.tex);
			glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

			glBindVertexArray(mesh.vao);
			glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, (void*)0);
		}
	}
	Helpers::CheckForGLError();

	// Send the combined matrix to the shader in a uniform

	glEnable(GL_DEPTH_TEST);

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_EQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	Helpers::CheckForGLError();

	glm::vec3 lightPositions[] =
	{
		glm::vec3(0, 500, 1700),
		glm::vec3(-1000, 500, 0),
		glm::vec3(0, 500, -1700),
		glm::vec3(4000, 500, 0)
	};

	glm::vec3 lightColours[] =
	{
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 1, 1)
	};

	glUseProgram(m_lights);

	const int numLights = 4;

	for (int i = 0; i < numLights; i++)
	{
		glm::vec3 cameraPos = camera.GetPosition();
		GLuint cameraPositionID = glGetUniformLocation(m_lights, "viewPos");
		glUniform3fv(cameraPositionID, 1, glm::value_ptr(cameraPos));

		glm::vec3 lightPos = glm::vec3(lightPositions[i]);
		GLuint lightPosID = glGetUniformLocation(m_lights, "lightposition");
		glUniform3fv(lightPosID, 1, glm::value_ptr(lightPos));

		glm::vec3 lightColour = glm::vec3(lightColours[i]);
		GLuint lightColourID = glGetUniformLocation(m_lights, "lightcolour");
		glUniform3fv(lightColourID, 1, glm::value_ptr(lightColour));

		GLfloat lightConstant = 1.0f;
		GLuint lightConstantID = glGetUniformLocation(m_lights, "lightconstant");
		glUniform1f(lightConstantID, lightConstant);

		GLfloat lightLinear = 0.0000014f;
		GLuint lightLinearID = glGetUniformLocation(m_lights, "lightlinear");
		glUniform1f(lightLinearID, lightLinear);

		GLfloat lightquadratic = 0.0000000007f;
		GLuint lightquadraticID = glGetUniformLocation(m_lights, "lightquadratic");
		glUniform1f(lightquadraticID, lightquadratic);

		glm::vec3 lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		GLuint lightAmbientID = glGetUniformLocation(m_lights, "lightambient");
		glUniform3fv(lightAmbientID, 1, glm::value_ptr(lightAmbient));

		glm::vec3 lightDiffuse = lightColour;
		GLuint lightDiffuseID = glGetUniformLocation(m_lights, "lightdiffuse");
		glUniform3fv(lightDiffuseID, 1, glm::value_ptr(lightDiffuse));

		glm::vec3 lightSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
		GLuint lightSpecularID = glGetUniformLocation(m_lights, "lightspecular");
		glUniform3fv(lightSpecularID, 1, glm::value_ptr(lightSpecular));

		combined_xform_id = glGetUniformLocation(m_lights, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
		model_xform = glm::mat4(1);
		model_xform_id = glGetUniformLocation(m_lights, "model_xform");

		// Send the combined matrix to the shader in a uniform

		Helpers::CheckForGLError();

		// Send the model matrix to the shader in a uniform

		for (Model& mod : models)
		{
			model_xform = mod.modelMatrix;

			glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

			for (Helpers::Mesh& mesh : mod.mesh)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh.tex);
				glUniform1i(glGetUniformLocation(m_lights, "sampler_tex"), 0);

				glBindVertexArray(mesh.vao);
				glDrawElements(GL_TRIANGLES, mesh.numElements, GL_UNSIGNED_INT, (void*)0);
			}
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();

	//Shadow Mapping Stuff Start

	/*glUseProgram(m_ShadowM);

	farPlane = glGetUniformLocation(m_ShadowM, "far");
	glUniform1f(farPlane, farV);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGetUniformLocation(m_ShadowM, "")
	
*/


	//Shadow Mapping Stuff End

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(m_FXAA);

	glm::vec2 texelStep = glm::vec2(1.0f / 1280, 1.0f / 720);
	GLuint texelStepID = glGetUniformLocation(m_FXAA, "u_texelStep");
	glUniform2fv(texelStepID, 1, glm::value_ptr(texelStep));

	GLfloat lumaThreshold = glGetUniformLocation(m_FXAA, "u_lumaThreshold");
	glUniform1f(lumaThreshold, 0.5f);

	GLfloat mulReduce = glGetUniformLocation(m_FXAA, "u_mulReduce");
	glUniform1f(mulReduce, 1.0f / 8.0f);

	GLfloat minReduce = glGetUniformLocation(m_FXAA, "u_minReduce");
	glUniform1f(minReduce, 1.0f / 128.0f);

	GLfloat maxSpan = glGetUniformLocation(m_FXAA, "u_maxSpan");
	glUniform1f(maxSpan, 8.0f);

	if (m_FXAAB)
	{
		GLuint FXAAON = glGetUniformLocation(m_FXAA, "u_fxaaOn");
		glUniform1i(FXAAON, 1);
	}

	else
	{
		GLuint FXAAON = glGetUniformLocation(m_FXAA, "u_fxaaOn");
		glUniform1i(FXAAON, 0);
	}

	Helpers::CheckForGLError();

	 glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(m_DOF);

	GLuint model_distance = glGetUniformLocation(m_DOF, "model_distance");
	glUniform1f(model_distance, distance(glm::vec3(0, 0, 0), camera.GetPosition()));

	nearPlane = glGetUniformLocation(m_DOF, "near");
	glUniform1f(nearPlane, nearV);

	farPlane = glGetUniformLocation(m_DOF, "far");
	glUniform1f(farPlane, farV);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(m_DOF, "depth_tex"), 0);

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(m_DOF, "colour_tex"), 0);

	if (m_DOFB)
	{
		GLuint DOFON = glGetUniformLocation(m_DOF, "u_DOFOn");
		glUniform1i(DOFON, 1);
	}

	else
	{
		GLuint DOFON = glGetUniformLocation(m_DOF, "u_DOFOn");
		glUniform1i(DOFON, 0);
	}

	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

