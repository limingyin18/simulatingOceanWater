#include "MeshBasicRender.hpp"

using namespace Eigen;

PlaneRender::PlaneRender(Plane& o, Matrix4f& m, Camera& cam) : plane{o}, camera{cam}, model{m}
{
 	// vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshBase::Vertex) * plane.data.size(), plane.data.data(), GL_DYNAMIC_DRAW);

	// index buffer
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * plane.indices.size(), plane.indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBase::Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBase::Vertex), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// color attribute
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MeshBase::Vertex), (GLvoid*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

   // shader
    render.addShader("Solid.vert", GL_VERTEX_SHADER);
    render.addShader("Solid.frag", GL_FRAGMENT_SHADER);
    render.link();
	render.addUniform("projection");
	render.addUniform("view");
	render.addUniform("model");
}

void PlaneRender::draw()
{
	glUseProgram(render.Program);
	glUniformMatrix4fv(render.uniforms.at("projection"), 1, GL_FALSE, camera.projection.data());
	glUniformMatrix4fv(render.uniforms.at("view"), 1, GL_FALSE, camera.view.data());
	glUniformMatrix4fv(render.uniforms.at("model"), 1, GL_FALSE, model.data());
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)plane.indices.size(), GL_UNSIGNED_INT, 0);
}