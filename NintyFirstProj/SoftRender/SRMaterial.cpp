﻿#include "SRMaterial.h"

#include "SRTexture.h"
#include "SRShader.h"

Material::Material(Shader* sha, Texture* tex, const int& _gloss) {
	color = glm::vec4(255, 255, 255, 255);
	specular = glm::vec4(255, 255, 255, 255);
	gloss = _gloss;
	texture = tex;
	shader = sha;
}

Material::Material(Shader* sha, const glm::vec4& _color, const int& _gloss) {
	texture = nullptr;
	color = _color;
	specular = glm::vec4(255, 255, 255, 255);
	shader = sha;
	gloss = _gloss;
}

void Material::setShader(Shader* sha) {
	shader = sha;
}

void Material::setTexture(Texture* tex) {
	texture = tex;
}