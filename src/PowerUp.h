#ifndef POWER_UP_H
#define POWER_UP_H

#include "GameObject.h"

// powerup size
static const glm::vec2 SIZE = glm::vec2(60, 20);
// powerup velocity
static const glm::vec2 VELOCITY = glm::vec2(0.0f, 150.0f);

class PowerUp : public GameObject
{
    public:
        // powerup state
        std::string Type;
        GLfloat     Duration;
        GLboolean   Activated;
        // constructor
        PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D texture)
            : GameObject(position, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated(GL_FALSE)
        { }
};

#endif