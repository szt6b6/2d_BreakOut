#ifndef SPIRIT_RENDERER_H
#define SPIRIT_RENDERER_H


#include "Shader.h"
#include "Texture2d.h"


class SpriteRenderer
{
public:
    SpriteRenderer(Shader shader);
    void drawSprite(Texture2D &texture, glm::vec2 position,     \
                    glm::vec2 size = glm::vec2(10.0f, 10.0f),   \
                    GLfloat rotate = 0.0f,                      \
                    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f));
private:
    Shader shader;
    GLuint quadVAO;
    void initRenderData();
};


#endif // !SPIRIT_RENDERER_H