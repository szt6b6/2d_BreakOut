#ifndef PARTICLE_H
#define PARTICLE_H

#include "GameObject.h"
#include "Shader.h"
#include "Texture2d.h"

#include <vector>
#include <glm/glm.hpp>

// single particle struct
struct Particle
{
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;

    Particle()
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

// particle generator class
class ParticleGenerator
{
public:
    ParticleGenerator(Shader shader, Texture2D texture, GLuint amount);
    void Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();
private:
    std::vector<Particle> particles;
    GLuint amount;
    Shader shader;
    Texture2D texture;
    GLuint VAO;
    void init(); // initializes buffer and vertex attributes
    GLuint firstUnusedParticle(); // find first unused particle in particles vector
    // respawns unused particle
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};


#endif // !PARTICLE_H