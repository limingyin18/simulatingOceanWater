#include "OceanRender.hpp"

using namespace Eigen;

OceanRender::OceanRender(Ocean& o, Camera& cam) :
	ocean{o}, camera{cam}
{
 	// vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// vertex buffer
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshBase::Vertex) * ocean.plane.data.size(), ocean.plane.data.data(), GL_DYNAMIC_DRAW);

	// index buffer
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * ocean.plane.indices.size(), ocean.plane.indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBase::Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshBase::Vertex), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// color attribute
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MeshBase::Vertex), (GLvoid*)(8 * sizeof(float)));
	glEnableVertexAttribArray(2);

   // shader
    render.addShader("ocean.vert", GL_VERTEX_SHADER);
    render.addShader("ocean.frag", GL_FRAGMENT_SHADER);
    render.link();
	render.addUniform("projection");
	render.addUniform("view");
    render.addUniform("lambda");
   	render.addUniform("projection");
	render.addUniform("view");
	render.addUniform("viewPos");
	glProgramUniform1i(render.Program, render.uniforms.at("lambda"), ocean.lambda);
}

void OceanRender::draw()
{
	glUseProgram(render.Program);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ocean.ssboAmpl);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ocean.ssboNormalX);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ocean.ssboNormalZ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ocean.ssboDx);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ocean.ssboDz);
	glUniformMatrix4fv(render.uniforms.at("projection"), 1, GL_FALSE, camera.projection.data());
	glUniformMatrix4fv(render.uniforms.at("view"), 1, GL_FALSE, camera.view.data());
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)ocean.plane.indices.size(), GL_UNSIGNED_INT, 0);
}