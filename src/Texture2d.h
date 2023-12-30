#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class Texture2D {
public:
    // 纹理对象的ID
    GLuint ID;
    // 纹理的尺寸
    GLuint Width, Height;
    // 纹理的格式
    GLuint Internal_Format;
    GLuint Image_Format;
    // 纹理的配置选项
    GLuint Wrap_S;
    GLuint Wrap_T;
    GLuint Filter_Min;
    GLuint Filter_Max;
    // 构造函数
    Texture2D();
    // 生成纹理
    void Generate(GLuint width, GLuint height, unsigned char* data);
    // 绑定纹理
    void Bind() const;
};

#endif