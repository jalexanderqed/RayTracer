//
// Created by jalexander on 10/4/18.
//

#ifndef GLITTER_OPENGL_CONST_H
#define GLITTER_OPENGL_CONST_H

namespace gl_code {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

}  // namespace gl_code

#endif //GLITTER_OPENGL_CONST_H
