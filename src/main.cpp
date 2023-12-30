#include "Game.h"
#include "ResourceManager.h"

// GLFW 函数声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// 屏幕宽度
const GLuint SCREEN_WIDTH = 800;
// 屏幕高度
const GLuint SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[])
{
    // 初始化 GLFW
    glfwInit();
    // 配置 GLFW
    // 第一个参数代表选项的名称，我们可以从很多以GLFW_开头的枚举值中选择
    // 第二个参数接受一个整型，用来设置这个选项的值
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 设置我们使用的是核心模式(Core-profile)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 设置窗口不可改变
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // 创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    
    // 初始化 GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();

    // 设置键盘回调函数
    glfwSetKeyCallback(window, key_callback);
    
    // 设置视口(Viewport)，告诉OpenGL渲染窗口的尺寸大小 seam to be nothing with coordination transformer
    // 前两个参数控制窗口左下角的位置，第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE); // cut the back face
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 初始化游戏
    Breakout.Init();
    Breakout.State = GAME_MENU;
    
    // 游戏循环
    GLfloat lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        // 计算帧数
        GLfloat currentFrame = glfwGetTime();
        // 计算两帧之间的时间差
        GLfloat deltaTime = currentFrame - lastFrame;
        // 记录上一帧的时间
        lastFrame = currentFrame;
        // 检查并调用事件
        glfwPollEvents();
        // 处理输入
        Breakout.ProcessInput(deltaTime);
        // 更新
        Breakout.Update(deltaTime);
        // 渲染
        Breakout.Render();

        // 交换缓冲
        glfwSwapBuffers(window);
    }
    // 释放所有的资源
    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // 当用户按下 ESC 键，我们设置 WindowShouldClose 属性为 true，关闭应用程序
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if(key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS) // if pressed all the time, run all the time
        {
            Breakout.Keys[key] = GL_TRUE;
        }
        else if(action == GLFW_RELEASE)
        {
            Breakout.Keys[key] = GL_FALSE;
            Breakout.Keys_hold[key] = GL_FALSE; // hold state reset
        }
    }
}