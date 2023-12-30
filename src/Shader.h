#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// 着色器程序类
class Shader {
public:
    // 程序ID
    GLuint ID;
    // 构造器读取并构建着色器
    Shader() { this->ID = glCreateProgram(); }
    // 构造器读取并构建着色器
    Shader& Use();
    // 编译着色器
    void    Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);
    // 程序中设置uniform变量
    void    SetFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
    void    SetInteger(const GLchar *name, GLint value, GLboolean useShader = false);
    void    SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void    SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void    SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void    SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void    SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void    SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void    SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

    void    SetVector2fv(const GLchar *name, const GLfloat* data, GLuint num, GLboolean useShader = false);
    void    SetVector1iv(const GLchar *name, const GLint* data, GLuint num, GLboolean useShader = false);
    void    SetVector1fv(const GLchar *name, const GLfloat* data, GLuint num, GLboolean useShader = false);
private:
    // 检查编译/连接是否错误的辅助函数
    void    checkCompileErrors(GLuint object, std::string type);
};

#endif