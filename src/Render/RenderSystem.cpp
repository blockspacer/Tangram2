#include "RenderSystem.hpp"

#include "Instance.hpp"

#include "Components/Vertices.hpp"

#include <glm/gtc/matrix_access.hpp>

#include "Debugging/glException.hpp"
#include <glad/glad.h>

unsigned int RenderSystem::m1to1QuadVBOid;
unsigned int RenderSystem::m1to1QuadVAOid;
Shader RenderSystem::s_shaderTest         ("res/shaders/default.vert", "res/shaders/test.frag", false);
Shader RenderSystem::s_shaderDrawingBoard ("res/shaders/default.vert", "res/shaders/drawingBoard.frag", false);
Shader RenderSystem::s_shaderPoint        ("res/shaders/default.vert", "res/shaders/point.frag", false);
Shader RenderSystem::s_shaderPolygon      ("res/shaders/default.vert", "res/shaders/polygon.frag", false);
Shader RenderSystem::s_shaderTexture      ("res/shaders/default.vert", "res/shaders/texture.frag", false);

RenderSystem::RenderSystem(Instance& instance)
	: ISystem(instance)
{}

#include <imgui/imgui.h>

void RenderSystem::render() {
	// Drawing Board
	renderQuad({ I.drawingBoardId() }, s_shaderDrawingBoard);
	// Layers
	//renderQuad(I.layersManager().m_layersOrdered, s_shaderTest);
	renderPreviewTexture(I.layersManager().m_layersOrdered);
	// Polygons
	static float smoothMin = 32.0f;
	ImGui::Begin("Test");
	ImGui::SliderFloat("SmoothMin", &smoothMin, 0.0f, 40.0f);
	ImGui::End();
	I.registry().view<entt::tag<"Polygon"_hs>, Cmp::Vertices>().each([this](auto entity, auto& tag, auto& vertices) {
		s_shaderPolygon.bind();
		s_shaderPolygon.setUniform1f("u_SmoothMin", smoothMin);
		int k = 0;
		for (entt::entity vertex : vertices.list) {
			s_shaderPolygon.setUniform2f("u_vertices[" + std::to_string(k) + "]", glm::vec2(glm::column(I.getMatrix(vertex), 2)));
			s_shaderPolygon.setUniformMat3f("u_mat", glm::mat3(1.0f));
			glBindVertexArray(m1to1QuadVAOid);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			k++;
		}

	});
	// Points 2D
	I.registry().view<entt::tag<"Point2D"_hs>>().each([this](auto entity, auto& tag) {
		renderSquare({ entity }, s_shaderPoint);
	});
}


void RenderSystem::_renderQuad(entt::entity e, Shader& shader, std::function<glm::mat3(entt::entity)> getMatrix) {
	shader.bind();
	shader.setUniformMat3f("u_mat", getMatrix(e));
	glBindVertexArray(m1to1QuadVAOid);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderSystem::renderQuad(const std::vector<entt::entity>& list, Shader& shader) {
	for (const entt::entity& entity : list)
		_renderQuad(entity, shader, [this](entt::entity e) { return I.getMatrixPlusAspectRatio(e);  });
}

void RenderSystem::renderSquare(const std::vector<entt::entity>& list, Shader& shader) {
	for (const entt::entity& entity : list)
		_renderQuad(entity, shader, [this](entt::entity e) { return I.getMatrix(e); });
}

void RenderSystem::renderPreviewTexture(const std::vector<entt::entity>& list) {
	s_shaderTexture.bind();
	s_shaderTexture.setUniformMat3f("u_mat", I.getMatrixPlusAspectRatio(I.drawingBoardId()));
	GLCall(glActiveTexture(GL_TEXTURE0));
	s_shaderTexture.setUniform1i("u_TextureSlot", 0);
	// Loop
	for (entt::entity e : list) {
		// Texture
		Cmp::Texture& tex = I.registry().get<Cmp::Texture>(e);
		GLCall(glBindTexture(GL_TEXTURE_2D, tex.id));
		// Draw
		glBindVertexArray(m1to1QuadVAOid);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	// Unbind
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void RenderSystem::Initialize() {
	s_shaderTest.compile();
	s_shaderDrawingBoard.compile();
	s_shaderPoint.compile();
	s_shaderPolygon.compile();
	s_shaderTexture.compile();
	GLCall(glGenVertexArrays(1, &m1to1QuadVAOid));
	GLCall(glGenBuffers(1, &m1to1QuadVBOid));
	// Vertices data
	float vertices[] = {
		//    pos      tex coords
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,

		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m1to1QuadVBOid));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	// Layout
	GLCall(glBindVertexArray(m1to1QuadVAOid));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m1to1QuadVBOid));
	// Positions
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0));
	// Texture coordinates
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

void RenderSystem::ShutDown() {
	glDeleteBuffers(1, &m1to1QuadVBOid);
	glDeleteVertexArrays(1, &m1to1QuadVAOid);
}