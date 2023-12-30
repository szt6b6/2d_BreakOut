#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <SOIL/SOIL.h>

#include "Texture2d.h"
#include "Shader.h"

// 单例模式 资源管理器
class ResourceManager{
public:
    // 资源存储
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    // 加载着色器
    static Shader   LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
    // 获取一个着色器
    static Shader   GetShader(std::string name);
    // 加载纹理
    static Texture2D LoadTexture(const GLchar *file, GLboolean alpha, std::string name);
    // 获取一个纹理
    static Texture2D GetTexture(std::string name);
    // 清理
    static void      Clear();

private:
    // 构造函数
    ResourceManager() { };
    // 加载并生成着色器
    static Shader    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    // 加载并生成纹理
    static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

#endif