#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

class Sprite
{
public:
	Sprite(Texture* m_texture, const glm::vec2& position, const glm::vec2& dimensions);
	~Sprite();

	void setTexture(Texture* _texture);
	Texture* getTexture() const;

	void setAngle(const float& _angle);
	float getAngle() const;

	void setPosition(const glm::vec2& _pos);
	glm::vec2 getPosition() const;

	void setDimensions(const glm::vec2& _dim);
	glm::vec2 getDimensions() const;

	void setFlip(const SDL_RendererFlip& _flip);
	SDL_RendererFlip getFlip() const;

	void setZOrder(const float& _zOrder);
	float getZOrder() const;

	virtual void update();
	virtual void render();
private:
	Texture* m_texture;
	glm::vec2 m_pos, m_dim;
	float m_angle;
	SDL_RendererFlip m_flip;

	float m_zOrder;
};

