#include "GameLevel.h"

void GameLevel::Load(const GLchar *file, GLuint levelWidth, GLuint levelHeight)
{   
    // first clear the old data
    this->Bricks.clear();

    // load from file
    GLuint tileNumCode;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<GLuint>> tileData;

    if(fstream) {
        // get one line
        while(getline(fstream, line)) {
            std::istringstream sstream(line);
            std::vector<GLuint> row;
            // from each line get tileNumCode
            while(sstream >> tileNumCode) {
                row.push_back(tileNumCode);
            }
            tileData.push_back(row);
        }
    }

    if(tileData.size() > 0) {
        this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for(GameObject &tile : this->Bricks) {
        if(!tile.Destroyed) {
            tile.Draw(renderer);
        }
    }
}

GLboolean GameLevel::IsCompleted()
{
    for(GameObject &tile : this->Bricks) {
        if(!tile.IsSolid && !tile.Destroyed) {
            return GL_FALSE;
        }
    }
    return GL_TRUE;
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
    // calculate grid size
    GLuint height = tileData.size();
    GLuint width = tileData[0].size();
    GLfloat unit_width = levelWidth / static_cast<GLfloat>(width);
    GLfloat unit_height = levelHeight / static_cast<GLfloat>(height);

    // initialize level tiles based on tileData
    /*
        数字0：无砖块，表示关卡中空的区域
        数字1：一个坚硬的砖块，不可被摧毁
        大于1的数字：一个可被摧毁的砖块，不同的数字区分砖块的颜色
    */
    for(GLuint y = 0; y < height; ++y) {
        for(GLuint x = 0; x < width; ++x) {
            // different brisks have different colors and hardness
            if(tileData[y][x] == 1) {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = GL_TRUE;
                this->Bricks.push_back(obj);
            } else if(tileData[y][x] > 1) {
                glm::vec3 color = glm::vec3(1.0f);
                if(tileData[y][x] == 2) {
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                } else if(tileData[y][x] == 3) {
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                } else if(tileData[y][x] == 4) {
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                } else if(tileData[y][x] == 5) {
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                }

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color));
            }
        }
    }
}
