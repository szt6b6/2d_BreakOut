#include "Game.h"
#include "utils.h"

GLboolean CheckCollisionRec2Rec(GameObject &one, GameObject &two) {
    // x axis
    GLboolean collisionX = one.Position.x + one.Size.x >= two.Position.x && \
                           two.Position.x + two.Size.x >= one.Position.x;
    // y axis
    GLboolean collisionY = one.Position.y + one.Size.y >= two.Position.y && \
                           two.Position.y + two.Size.y >= one.Position.y;
    return collisionX && collisionY;
}

Collision CheckCollisionCircle2Rec(BallObject &ball, GameObject &rec)
{
    // get circle center
    glm::vec2 center(ball.Position + ball.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(rec.Size.x / 2, rec.Size.y / 2);
    glm::vec2 aabb_center(rec.Position.x + aabb_half_extents.x, rec.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    // clamp difference vector to be between -half-extents and half-extents
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;

    if(glm::length(difference) < ball.Radius) {
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    } else {
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0.0f, 0.0f));
    }
}

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // 上
        glm::vec2(1.0f, 0.0f),  // 右
        glm::vec2(0.0f, -1.0f), // 下
        glm::vec2(-1.0f, 0.0f)  // 左
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++)
    {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
    for(const PowerUp& powerUp : powerUps) {
        if(powerUp.Activated && powerUp.Type == type) {
            return GL_TRUE;
        }
    }
    return GL_FALSE;
}

Game::Game(GLuint width, GLuint height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), Lives(3)
{
}

// 初始化游戏状态（加载所有的着色器/纹理/关卡）
void Game::Init() {
    // sound init
    this->SoundEngine = irrklang::createIrrKlangDevice();
    this->SoundEngine->play2D("resources/audio/breakout.mp3", GL_TRUE); // loop
    /*
        glm::ortho(0.0f, static_cast<GLfloat>(this->Width), \
                                      static_cast<GLfloat>(this->Height), 0.0f, \
                                      -1.0f, 1.0f);
        前两个参数指定了平截头体的左右坐标，
        第三和第四参数指定了平截头体的底部和顶部。
        通过这四个参数我们定义了近平面和远平面的大小，
        然后第五和第六个参数则定义了近平面和远平面的距离。
        这个投影矩阵会将处于这些x，y，z值范围内的坐标变换为标准化设备坐标。
    */
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), \
                                      static_cast<GLfloat>(this->Height), 0.0f, \
                                      -1.0f, 1.0f); // 将坐标放缩到-1 1之间
    ResourceManager::LoadShader("src/shaders/sprite.vs", "src/shaders/sprite.fs", nullptr, "sprite");
    ResourceManager::LoadShader("src/shaders/particle.vs", "src/shaders/particle.fs", nullptr, "particle");

    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("sprite").SetInteger("image", 0);
    ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").SetInteger("image", 0);
    // 设置渲染器
    this->Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // 加载纹理
    ResourceManager::LoadTexture("resources/textures/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture("resources/textures/background.jpg", GL_FALSE, "background");
    // powerups textures
    ResourceManager::LoadTexture("resources/textures/powerup_speed.png", GL_TRUE, "powerup_speed");
    ResourceManager::LoadTexture("resources/textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
    ResourceManager::LoadTexture("resources/textures/powerup_increase.png", GL_TRUE, "powerup_increase");
    ResourceManager::LoadTexture("resources/textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
    ResourceManager::LoadTexture("resources/textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");
    ResourceManager::LoadTexture("resources/textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");

    // 加载关卡
    GameLevel one; one.Load("resources/levels/one.lvl", this->Width, this->Height * 0.5);
    GameLevel two; two.Load("resources/levels/two.lvl", this->Width, this->Height * 0.5);
    GameLevel three; three.Load("resources/levels/three.lvl", this->Width, this->Height * 0.5);
    GameLevel four; four.Load("resources/levels/four.lvl", this->Width, this->Height * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    // load player board
    ResourceManager::LoadTexture("resources/textures/paddle.png", GL_TRUE, "paddle");
    glm::vec2 player_positon = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    this->Player = new GameObject(player_positon, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    // load ball
    ResourceManager::LoadTexture("resources/textures/awesomeface.png", GL_TRUE, "ball");
    glm::vec2 ball_position = player_positon + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    this->Ball = new BallObject(ball_position, BALL_RADIUS, INI_BALL_VELOCITY, ResourceManager::GetTexture("ball"));

    // load particle
    ResourceManager::LoadTexture("resources/textures/particle.png", GL_TRUE, "particle");
    this->Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

    // load postprocessor
    ResourceManager::LoadShader("src/shaders/frame_buffer.vs", "src/shaders/frame_buffer.fs", nullptr, "postprocessing");
    this->Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

    // load text
    this->Text = new TextRender(this->Width, this->Height);
    this->Text->Load("resources/fonts/OCRAEXT.TTF", 24);
}
// 游戏循环
void Game::ProcessInput(GLfloat dt) {
    if(this->State == GAME_ACTIVE) {
        // player board move
        GLfloat delta_s = PLAYER_SPEED * dt;
        if(this->Keys[GLFW_KEY_A]) {
            if(this->Player->Position.x >= 0) {
                this->Player->Position.x -= delta_s;
                if(this->Ball->Stuck) this->Ball->Position.x -= delta_s; // let stuck ball move with player board
            }
        }
        if(this->Keys[GLFW_KEY_D]) {
            if(this->Player->Position.x <= this->Width - this->Player->Size.x) {
                this->Player->Position.x += delta_s;
                if(this->Ball->Stuck) this->Ball->Position.x += delta_s;
            }
        }

        // ball stuck on player board
        if(this->Keys[GLFW_KEY_SPACE]) {
            this->Ball->Stuck = false;
        }
    }
    
    if(this->State == GAME_MENU) {
        if(this->Keys[GLFW_KEY_SPACE] && !this->Keys_hold[GLFW_KEY_SPACE]) {
            this->State = GAME_ACTIVE;
            this->Keys_hold[GLFW_KEY_SPACE] = GL_TRUE;
        } 
        if(this->Keys[GLFW_KEY_W] && !this->Keys_hold[GLFW_KEY_W]) {
            this->Level = (this->Level + 1) % 4;
            this->Keys_hold[GLFW_KEY_W] = GL_TRUE;
        }
        if(this->Keys[GLFW_KEY_S] && !this->Keys_hold[GLFW_KEY_S]) {
            if(this->Level > 0) {
                --this->Level;
            } else {
                this->Level = 3;
            }
            this->Keys_hold[GLFW_KEY_S] = GL_TRUE;
            
        }
    }

    if(this->State == GAME_WIN) {
        if(this->Keys[GLFW_KEY_SPACE]) {
            this->State = GAME_MENU;
            this->Effects->Chaos = GL_TRUE;
            this->Keys_hold[GLFW_KEY_SPACE] = GL_TRUE;
        }
    }
}


GLfloat ShakeTime = 0.0f;
void Game::Update(GLfloat dt) {
    // update ball move
    Ball->Move(dt, this->Width);
    // check collision
    this->DoCollision();
    // check ball out of bounds
    if(this->Ball->Position.y >= this->Height) {
        --this->Lives;
        if(this->Lives == 0) {
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }
    // particle update
    this->Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));

    // update powerups
    this->UpdatePowerUps(dt);

    // shake
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }

    // check win
    if(this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()) {
        this->ResetLevel();
        this->ResetPlayer();
        this->Effects->Chaos = GL_TRUE;
        this->State = GAME_WIN;
    }
}
void Game::Render(){
    if(this->State == GAME_ACTIVE || this->State == GAME_MENU) {

        // begin rendering to postprocessing framebuffer
        this->Effects->BeginRender();

        // draw background
        Texture2D texture = ResourceManager::GetTexture("background");
        Renderer->drawSprite(texture, glm::vec2(0, 0), glm::vec2(this->Width, this->Height), 0.0f);

        // draw level
        this->Levels[this->Level].Draw(*Renderer);

        // draw player
        this->Player->Draw(*Renderer);

        // draw ball
        this->Ball->Draw(*Renderer);

        // draw particle
        this->Particles->Draw();

        // draw powerups
        for(PowerUp& powerUp : this->PowerUps) {
            if(!powerUp.Destroyed) {
                powerUp.Draw(*Renderer);
            }
        }

        // end rendering to postprocessing framebuffer
        this->Effects->EndRender();
        this->Effects->Render(glfwGetTime());

        // draw left lives
        std::stringstream ss;
        ss << this->Lives;
        this->Text->RenderText("Lives: " + ss.str(), 0.0f, this->Height - 24.0f, 1.0f, glm::vec3(1.0f));
    }

    if(this->State == GAME_MENU) {
        this->Text->RenderText("Press SPACE to start", 250.0f, this->Height / 2, 1.0f, glm::vec3(1.0f));
        this->Text->RenderText("Press W or S to select level", 245.0f, this->Height / 2 + 20.0f, 0.75f, glm::vec3(1.0f));
    }

    if(this->State == GAME_WIN) {
        this->Text->RenderText("You WON!!!", 320.0f, this->Height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        this->Text->RenderText("Press SPACE to retry or ESC to quit", 130.0f, this->Height / 2, 1.0f, glm::vec3(1.0f));
    }
}


void Game::DoCollision()
{
    for(GameObject& box : this->Levels[this->Level].Bricks) {
        if(!box.Destroyed) {
            Collision collision = CheckCollisionCircle2Rec(*this->Ball, box);
            if(std::get<0>(collision)) { // collision happen
                if(!box.IsSolid) { // not solid then destroy
                    box.Destroyed = GL_TRUE;
                    // play sound
                    this->SoundEngine->play2D("resources/audio/bleep.mp3", GL_FALSE);
                    // generate powerups
                    this->GeneratePowerUps(box);
                } else {
                    ShakeTime = 0.05f;
                    Effects->Shake = GL_TRUE;
                    // play sound
                    this->SoundEngine->play2D("resources/audio/solid.wav", GL_FALSE);
                }

                // if ball is pass through and box is not solid then don't rebound
                if(this->Ball->PassThrough && !box.IsSolid) continue;
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);

                if(dir == LEFT || dir == RIGHT) { // horizontal collision
                    this->Ball->Velocity.x = -this->Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    GLfloat penetration = this->Ball->Radius - std::abs(diff_vector.x);
                    if(dir == LEFT) {
                        this->Ball->Position.x += penetration;
                    } else {
                        this->Ball->Position.x -= penetration;
                    }
                } else { // vertical collision
                    this->Ball->Velocity.y = -this->Ball->Velocity.y;
                    GLfloat penetration = this->Ball->Radius - std::abs(diff_vector.y);
                    if(dir == UP) {
                        this->Ball->Position.y -= penetration;
                    } else {
                        this->Ball->Position.y += penetration;
                    }
                }
            }
        }
    }

    // checkout collision between player board and ball
    Collision collision = CheckCollisionCircle2Rec(*this->Ball, *this->Player);
    if(!this->Ball->Stuck && std::get<0>(collision)) {
        // check where it hit the board, and change velocity based on where it hit the board
        GLfloat center_board = this->Player->Position.x + this->Player->Size.x / 2;
        GLfloat distance = (this->Ball->Position.x + this->Ball->Radius) - center_board;
        GLfloat percentage = distance / (this->Player->Size.x / 2); // more distances from center then larger speed
        // then move accordingly
        GLfloat strength = 2.0f;
        glm::vec2 old_velocity = this->Ball->Velocity;
        this->Ball->Velocity.x = INI_BALL_VELOCITY.x * percentage * strength;
        this->Ball->Velocity.y = -1 * abs(this->Ball->Velocity.y); // use abs func to avoid ball stick on the board
        this->Ball->Velocity = glm::normalize(this->Ball->Velocity) * glm::length(old_velocity);
        this->Ball->Stuck = this->Ball->Sticky;
        // play sound
        this->SoundEngine->play2D("resources/audio/bleep.wav", GL_FALSE);
    }

    // check collision between player board and powerups
    for(PowerUp &powerup : this->PowerUps) {
        if(!powerup.Destroyed) {
            if(powerup.Position.y >= this->Height) { // cross the bottom of screen
                powerup.Destroyed = GL_TRUE;
            }
            if(CheckCollisionRec2Rec(*this->Player, powerup)) {
                ActivatePowerUp(powerup);
                powerup.Destroyed = GL_TRUE;
                powerup.Activated = GL_TRUE;
                // play sound
                this->SoundEngine->play2D("resources/audio/powerup.wav", GL_FALSE);
            }
        }
    }
}

/// @brief return true or false randomly according to chance
/// @param chance possibility of return true
/// @return true or false
GLboolean ShouldSpawn(GLuint chance)
{
    GLuint random = rand() % chance;
    return random == 0;
}
void Game::GeneratePowerUps(GameObject &block)
{
    if(ShouldSpawn(25)) { // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, \
                                         block.Position, ResourceManager::GetTexture("powerup_speed")));
    }
    if(ShouldSpawn(25)) {
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, \
                                         block.Position, ResourceManager::GetTexture("powerup_sticky")));
    }
    if(ShouldSpawn(25)) {
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, \
                                         block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    }
    if(ShouldSpawn(25)) {
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, \
                                         block.Position, ResourceManager::GetTexture("powerup_increase")));
    }
    if(ShouldSpawn(15)) { // negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, \
                                         block.Position, ResourceManager::GetTexture("powerup_confuse")));
    }
    if(ShouldSpawn(15)) {
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, \
                                         block.Position, ResourceManager::GetTexture("powerup_chaos")));
    }
}

void Game::ActivatePowerUp(PowerUp &powerup)
{
    // 根据不同的powerup类型，做出不同的反应
    if(powerup.Type == "speed") {
        Ball->Velocity *= 1.2;
    } else if(powerup.Type == "sticky") {
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    } else if(powerup.Type == "pass-through") {
        Ball->PassThrough = GL_TRUE;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    } else if(powerup.Type == "pad-size-increase") {
        Player->Size.x += 50;
    } else if(powerup.Type == "confuse") {
        if(!Effects->Chaos) {
            Effects->Confuse = GL_TRUE;
        }
    } else if(powerup.Type == "chaos") {
        if(!Effects->Confuse) {
            Effects->Chaos = GL_TRUE;
        }
    }
}

void Game::UpdatePowerUps(GLfloat dt)
{
    for(PowerUp& powerUp : this->PowerUps) {
        powerUp.Position += powerUp.Velocity * dt; // move powerup

        // if powerup is activated then count down
        if(powerUp.Activated) {
            powerUp.Duration -= dt;
            if(powerUp.Duration <= 0.0f) {
                // remove powerup from list (will later be removed)
                powerUp.Activated = GL_FALSE;
                // deactivate effects
                if(powerUp.Type == "sticky") {
                    if(!IsOtherPowerUpActive(this->PowerUps, "sticky")) {
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                } else if(powerUp.Type == "pass-through") {
                    if(!IsOtherPowerUpActive(this->PowerUps, "pass-through")) {
                        Ball->PassThrough = GL_FALSE;
                        Ball->Color = glm::vec3(1.0f);
                    }
                } else if(powerUp.Type == "confuse") {
                    if(!IsOtherPowerUpActive(this->PowerUps, "confuse")) {
                        Effects->Confuse = GL_FALSE;
                    }
                } else if(powerUp.Type == "chaos") {
                    if(!IsOtherPowerUpActive(this->PowerUps, "chaos")) {
                        Effects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }

    // erase power if it's destroyed and not activated
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(), \
                                        [](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }), \
                                        this->PowerUps.end());
}

void Game::ResetLevel()
{   
    this->Lives = 3;
    if(this->Level == 0) {
        this->Levels[0].Load("resources/levels/one.lvl", this->Width, this->Height * 0.5);
    } else if(this->Level == 1) {
        this->Levels[1].Load("resources/levels/two.lvl", this->Width, this->Height * 0.5);
    } else if(this->Level == 2) {
        this->Levels[2].Load("resources/levels/three.lvl", this->Width, this->Height * 0.5);
    } else if(this->Level == 3) {
        this->Levels[3].Load("resources/levels/four.lvl", this->Width, this->Height * 0.5);
    }
}

void Game::ResetPlayer()
{   
    // player position
    this->Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    this->Player->Size = PLAYER_SIZE;

    // ball position
    this->Ball->Position = this->Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    this->Ball->Velocity = INI_BALL_VELOCITY;
    this->Ball->Stuck = GL_TRUE;
    this->Ball->PassThrough = GL_FALSE;
    this->Ball->Sticky = GL_FALSE;
    this->Ball->Color = glm::vec3(1.0f);

    // reset powerups and effects
    this->PowerUps.clear();
    Effects->Chaos = Effects->Confuse = Effects->Shake = GL_FALSE;
}