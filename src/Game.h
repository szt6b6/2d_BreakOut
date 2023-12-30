#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "GameLevel.h"
#include "BallObject.h"
#include "Particle.h"
#include "PostProcess.h"
#include "PowerUp.h"
#include "irrKlang/irrKlang.h"
#include "TextureRender.h"

#include <algorithm>

// init player board size
const glm::vec2 PLAYER_SIZE(100, 20);
// init player board speed
const GLfloat PLAYER_SPEED(500.0f);
// ball init radius and speed
const GLfloat BALL_RADIUS(12.5f);
const glm::vec2 INI_BALL_VELOCITY(100.0f, -350.0f);

// 代表了游戏的当前状态
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// direction 
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// check collision function
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;
GLboolean CheckCollisionRec2Rec(GameObject &one, GameObject &two);
Collision CheckCollisionCircle2Rec(BallObject &ball, GameObject &rec);

// get direction fucntion
Direction VectorDirection(glm::vec2 target);

class Game
{
    public:
        SpriteRenderer *Renderer;
        // game levels
        std::vector<GameLevel> Levels;
        // 0-
        GLuint Level;
        // player board
        GameObject *Player;
        // ball
        BallObject *Ball;
        // ParticleGenerator
        ParticleGenerator *Particles;
        // PostProcessor
        PostProcessor *Effects;
        // PowerUps
        std::vector<PowerUp> PowerUps;
        // sound engine
        irrklang::ISoundEngine *SoundEngine;
        // text render
        TextRender *Text;

        // 游戏状态
        GameState  State;   
        GLboolean  Keys[1024]; // key pressed state
        GLboolean  Keys_hold[1024]; // key hold state
        GLuint     Width, Height;
        GLuint     Lives; // player lives
        // 构造函数/析构函数
        Game(GLuint width, GLuint height);
        ~Game() {delete Renderer; delete Player; delete Ball; delete Particles; delete Effects; delete SoundEngine;}
        // 初始化游戏状态（加载所有的着色器/纹理/关卡）
        void Init();
        // 游戏循环
        void ProcessInput(GLfloat dt);
        void Update(GLfloat dt);
        void Render();
        // collision detect, if happen then do action
        void DoCollision();
        // generate powerups
        void GeneratePowerUps(GameObject &block);
        // activate powerups
        void ActivatePowerUp(PowerUp &powerUp);
        // update powerups
        void UpdatePowerUps(GLfloat dt);
        // reset game
        void ResetLevel();
        void ResetPlayer();
};