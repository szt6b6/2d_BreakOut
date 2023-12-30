#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include "ResourceManager.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include <vector>


class GameLevel
{
public:
    // 关卡状态
    std::vector<GameObject> Bricks;
    // 构造函数
    GameLevel() { }
    // 加载关卡
    void      Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    // 渲染关卡
    void      Draw(SpriteRenderer &renderer);
    // 检查关卡是否完成（所有非坚硬砖块都被销毁）
    GLboolean IsCompleted();
private:
    // 初始化关卡从砖块数据
    void      init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif