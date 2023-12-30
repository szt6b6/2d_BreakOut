#include "Particle.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount)
{
    this->shader = shader;
    this->texture = texture;
    this->amount = amount;
    this->init();
}

void ParticleGenerator::Update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset)
{
    // add new particles
    for(GLuint i = 0; i < newParticles; i++) {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    // update all particles
    for(GLuint i = 0; i < this->amount; i++) {
        Particle& p = this->particles[i];
        p.Life -= dt; // reduce life
        if(p.Life > 0.0f) { // particle is alive, thus update
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt * 2.5;
        }
    }
}

void ParticleGenerator::Draw()
{
    // use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.Use();
    for(Particle& particle : this->particles) {
        if(particle.Life > 0.0f) {
            this->shader.SetVector2f("offset", particle.Position);
            this->shader.SetVector4f("color", particle.Color);
            this->texture.Bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // set back to default
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{   
    GLuint vbo;
    GLfloat particle_quad[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f, 
        1.0f, 1.0f, 1.0f, 1.0f, 
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &vbo);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
    
    glBindVertexArray(0);

    // create particle
    for(GLuint i = 0; i < this->amount; i++) {
        this->particles.push_back(Particle());
    }
}


GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
    for(GLuint i = lastUsedParticle; i < this->amount; i++) {
        if(this->particles[i].Life <= 0.0f) {
            return i;
        }
    }

    for(GLuint i = 0; i < lastUsedParticle; i++) {
        if(this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    
    // all are used, override the first one
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset)
{
    // update particle
    GLfloat random = ((rand() % 100) - 50) / 10.0f;
    GLfloat rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = object.Position + random + offset;
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = object.Velocity * 0.1f;
}
