#ifndef UTILS_H
#define UTILS_H
#include <GLFW/glfw3.h>
#include <iostream>

// check opengl error code and print it
// like a queue, if some where error happened, the error code will be stored in the queue, is queue null, error code is 0
// glGetError() will return the first error code in the queue, and remove it from the queue
#define glCheckError() {std::cout << "code: " << glGetError() << " " << __FILE__ <<" line: "<< __LINE__ << std::endl;};

#endif /* UTILS_H */