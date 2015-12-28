#include "OpenGL.h"

std::fstream * debugstream;

void util::initShaderHandler(std::fstream * ds)
{
	debugstream = ds;
}

bool util::shaderFromFile(GLuint shader, std::string filename, std::string shadername)
{
	std::fstream in;
	in.open(filename, std::ios_base::in);
	if(!in.is_open())
	{
		*debugstream << "File " << filename << " could not be opened, and consequently <<" << shadername << ">> couldn't be compiled." << std::endl;
		return false;
	}
	std::string line, answer;
	while(std::getline(in, line)) answer += line + "\n";
	in.close();
	const char * c_str = answer.c_str();
	GLint size = answer.size();
	glShaderSource(shader, 1, &(c_str), &(size));
	glCompileShader(shader);
	//Check for proper compilation:
	GLint _compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &_compiled);
	if(!_compiled)
	{
		GLint length;
		GLchar* log;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = new GLchar[length];
		glGetShaderInfoLog(shader, length, &length, log);
		*debugstream << "There was a problem with shader <<" << shadername << ">>\n" << log << "----------------------------------------" << std::endl;
		delete log;
		return false;
	}
	return true;
}

GLuint shaderProgram;
FT_Library ft;
FT_Face face;
int screenWidth, screenHeight;

void util::setScreenSize(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
}

bool util::initText()
{
	shaderProgram = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	util::shaderFromFile(vs, "shaders/text/vertex.glsl", "Text Vertex Shader");
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	util::shaderFromFile(fs, "shaders/text/fragment.glsl", "Text Fragment Shader");
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);
	glDetachShader(shaderProgram, fs);
	glDetachShader(shaderProgram, vs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

	return FT_Init_FreeType(&ft) == 0;
}

bool util::setFont(std::string path)
{
	FT_Done_Face(face);
	return FT_New_Face(ft, path.c_str(), 0, &face) == 0;
}

#include <iostream>

char util::renderText(float x, float y, int size, const std::string & text)
{
	//Get currently bound program:
	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

	//Set size.
	FT_Set_Pixel_Sizes(face, size, size);

	//Create initial buffer:
	float points[] = { 0, 0, 0, 0, 0, 0, 0, 0 }; //Zero-initialize
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), points, GL_DYNAMIC_DRAW);

	float tpoints[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
	GLuint vbot = 0;
	glGenBuffers(1, &vbot);
	glBindBuffer(GL_ARRAY_BUFFER, vbot);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), tpoints, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vbot);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glActiveTexture(GL_TEXTURE0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_GlyphSlot g = face->glyph;

	//For each character, load the position:
	for(int a = 0; a < text.size(); a++)
	{
		if(FT_Load_Char(face, text[a], FT_LOAD_RENDER) != 0) return text[a];

		float left = x + float(g->bitmap_left) / screenWidth, top = y + float(g->bitmap_top) / screenHeight, right = left + (float(g->bitmap.width) / screenWidth), bottom = top - (float(g->bitmap.rows) / screenHeight);
		points[0] = left; points[1] = top; points[2] = right; points[3] = top; points[4] = left; points[5] = bottom; points[6] = right; points[7] = bottom;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), points, GL_DYNAMIC_DRAW);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g->bitmap.width, g->bitmap.rows, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += float(g->advance.x) / 64 / screenWidth;
		y += float(g->advance.y) / 64 / screenHeight;
	}

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbot);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &tex);

	//Set program back to previous program:
	glUseProgram(currentProgram);

	return 0;
}

std::map<std::pair<char, int>, std::vector< std::pair< std::pair<float, float>, std::pair<float, float> > > > textJobs;

void util::addText(float x, float y, int size, const std::string & text)
{
	FT_GlyphSlot g = face->glyph;
	for(auto a = text.begin(); a != text.end(); a++)
	{
		if(FT_Load_Char(face, *a, FT_LOAD_RENDER) != 0) continue; //Ignore characters which can't be loaded.
		float left = x + float(g->bitmap_left) / screenWidth, top = y + float(g->bitmap_top) / screenHeight, right = left + (float(g->bitmap.width) / screenWidth), bottom = top - (float(g->bitmap.rows) / screenHeight);
		if(!textJobs.count({ *a, size })) textJobs.insert({ { *a, size }, std::vector< std::pair< std::pair<float, float>, std::pair<float, float> > >() });
		textJobs[{ *a, size }].push_back({ { left, bottom }, { right, top } });
		x += float(g->advance.x) / 64 / screenWidth;
		y += float(g->advance.y) / 64 / screenHeight;
	}
}

char util::renderAllText()
{
	//Get currently bound program:
	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

	GLuint vbo[2];
	glGenBuffers(1, vbo);
	float textureVertices[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), textureVertices, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glActiveTexture(GL_TEXTURE0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	float points[8];

	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	FT_GlyphSlot g = face->glyph;

	for(auto a = textJobs.begin(); a != textJobs.end(); a++)
	{
		FT_Set_Pixel_Sizes(face, a->first.second, a->first.second);
		if(FT_Load_Char(face, a->first.first, FT_LOAD_RENDER) != 0) return a->first.first;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g->bitmap.width, g->bitmap.rows, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		for(auto b = a->second.begin(); b != a->second.end(); b++)
		{
			points[0] = b->first.first; points[1] = b->second.second; points[2] = b->second.first; points[3] = b->second.second; points[4] = b->first.first; points[5] = b->first.second; points[6] = b->second.first; points[7] = b->first.second;

			glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), points, GL_DYNAMIC_DRAW);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &tex);

	//Set program back to previous program:
	glUseProgram(currentProgram);

	textJobs.clear();

	return 0;
}

void util::cleanup()
{
	glDeleteProgram(shaderProgram);
}