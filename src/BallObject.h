#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "GameObject.h"

// ball object inherit from GameObject
class BallObject : public GameObject {
public:
    // ball state	
    GLfloat   Radius;
    GLboolean Stuck;
    GLboolean Sticky, PassThrough;
    // constructor(s)
    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
    // move the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
    glm::vec2 Move(GLfloat dt, GLuint window_width);
    // resets the ball to original state with given position and velocity
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};


#endif