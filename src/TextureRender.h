#ifndef TEXTURE_RENDER_H
#define TEXTURE_RENDER_H

#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture2d.h"
#include "Shader.h"
#include "ResourceManager.h"

#include "ft2build.h"
#include FT_FREETYPE_H

struct Character {
    GLuint TextureID;   // 字形纹理ID
    glm::ivec2 Size;    // 字形大小
    glm::ivec2 Bearing; // 从基准线到字形左部/顶部的偏移值
    GLuint Advance;     // 原点距下一个字形原点的距离
};

class TextRender {
public:
    // 字体state
    std::map<GLchar, Character> Characters;
    // 字体shader
    Shader TextShader;
    // 构造函数
    TextRender(GLuint width, GLuint height);
    // 预处理字体
    void Load(std::string font, GLuint fontSize);
    // 渲染文本
    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));
private:
    // 渲染状态
    GLuint VAO, VBO;
};

#endif