#include "Tile.h"
#include <GLFW/glfw3.h>

bool Tile::drawFog = true;

float sign(float x) {
	if (x > 0.0f) return 1.0f;
	else if (x < 0.0f) { return -1.0f; }
	else return 0.0f;
}

Tile::Tile(glm::vec2 position, float scale, float dispFactor, TessellationShader * shad, Model * planeModel, unsigned int * textures) : dispFactor(dispFactor), scaleFactor(scale), shad(shad), position(position), planeModel(planeModel), textures(textures)
{
	glm::mat4 id;
	glm::mat4 scaleMatrix = glm::scale(id, glm::vec3(scale, 0.0, scale));
	glm::mat4 positionMatrix = glm::translate(id, glm::vec3(position.x, 0.0, position.y));
	modelMatrix = positionMatrix * scaleMatrix;

	octaves = 10;
	frequency = 10.0;
	grassCoverage = 0.75;
	tessMultiplier = 2.0;

}

void Tile::drawTile(Camera * camera, glm::mat4 proj, glm::vec3 lightPosition, glm::vec3 lightColor, glm::vec3 fogColor, float waterHeight, float up) {

	if (up != 0.0f) {
		glEnable(GL_CLIP_DISTANCE0);
	}
	glm::mat4 gWorld = modelMatrix;
	glm::mat4 gVP = proj * camera->GetViewMatrix();

	shad->use();
	shad->setVec3("gEyeWorldPos", camera->Position);
	shad->setMat4("gWorld", gWorld);
	shad->setMat4("gVP", gVP);
	shad->setFloat("gDispFactor", dispFactor);
	float correction = 0.0f;
	if (up < 0.0f) correction = 0.0025f * dispFactor;
	glm::vec4 clipPlane(0.0, 1.0, 0.0, -waterHeight -correction);
	shad->setVec4("clipPlane", clipPlane*up);
	shad->setVec3("u_LightColor", lightColor);
	shad->setVec3("u_LightPosition", lightPosition);
	shad->setVec3("u_ViewPosition", camera->Position);
	shad->setVec3("fogColor", fogColor);
	//shad->setFloat("tessLevel", 0.0f);
	shad->setInt("octaves", octaves);
	shad->setFloat("freq", frequency);
	shad->setFloat("u_grassCoverage", grassCoverage);
	shad->setFloat("waterHeight", waterHeight);
	shad->setFloat("tessMultiplier", tessMultiplier);


	shad->setBool("normals", true);
	shad->setBool("drawFog", Tile::drawFog);


	// set textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	shad->setInt("sand", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	shad->setInt("grass", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	shad->setInt("rock", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	shad->setInt("snow", 4);

	planeModel->Draw(*shad);
	glDisable(GL_CLIP_DISTANCE0);

}

void Tile::drawTile(Camera * camera, glm::mat4 proj, glm::vec3 lightPosition, glm::vec3 lightColor, glm::vec3 fogColor, float waterHeight, float up, float tessLevel) {
	shad->setFloat("tessLevel", tessLevel);
	shad->setBool("normals", true);
	this->drawTile(camera, proj, lightPosition, lightColor, fogColor, waterHeight, up);
	shad->setFloat("tessLevel", 0.0);
	shad->setBool("normals", true);
}

bool Tile::inTile(Camera camera) {
	float camX = camera.Position.x;
	float camY = camera.Position.z;

	float x = position.x;
	float y = position.y;

	bool inX = false;
	if ((camX < x + scaleFactor * tileW/2.0f) && (camX > x - scaleFactor * tileW/2.0f)) { inX = true; }
	bool inY = false;
	if ((camY < y + scaleFactor * tileW/2.0f) && (camY > y - scaleFactor * tileW/2.0f)) { inY = true; }

	bool result = inX && inY;

	if (result) {

		//std::cout << y << " :y" << std::endl;
		//std::cout << y << " :y" << std::endl;

		//std::cout << y + scaleFactor * tileW / 2.0f << ": y + scalefactor" << std::endl;
		//std::cout << y - scaleFactor * tileW / 2.0f << ": y - scalefactor" << std::endl;
	}

	return result;

}


Tile::~Tile()
{

}
