#include "Renderer.hpp"

void RenderState::SetActiveVao(GLuint Vao) {
	glBindVertexArray(Vao);
	ActiveVao = Vao;
}

void RenderState::SetActiveShader(GLuint Shader) {
	if (Shader != ActiveShader) {
		glUseProgram(Shader);
		ActiveShader = Shader;
	}
}