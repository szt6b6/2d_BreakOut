#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2d.h"
#include "SpriteRenderer.h"


class PostProcessor
{
public:
    Shader PostProcessingShader;
    Texture2D Texture;
    GLuint Width, Height;
    // options
    GLboolean Confuse, Chaos, Shake;
    // constructor
    PostProcessor(Shader shader, GLuint width, GLuint height);
    // prepares the postprocessor's framebuffer operations before rendering the game
    void BeginRender();
    // should be called after rendering the game, so it stores all the rendered data into a texture object
    void EndRender();
    // renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void Render(GLfloat time);
private:
    // state
    GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    GLuint RBO; // RBO is used for multisampled color buffer
    GLuint VAO;
    // initialize quad for rendering postprocessing texture
    void initRenderData();
};

#endif /* POST_PROCESS_H */