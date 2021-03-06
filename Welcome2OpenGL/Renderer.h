#pragma once
#include <GLEW/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char * function, const char * file, int line);

class VertexArray;
class IndexBuffer;
class Shader;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
	void Draw(const GLenum mode, GLint first, GLsizei count, GLsizei primcount);
};
