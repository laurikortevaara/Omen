//
// Created by Lauri Kortevaara(personal) on 28/11/15.
//

#ifndef OMEN_CAMERA_H
#define OMEN_CAMERA_H


#include <OpenGL/OpenGL.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/detail/type_vec3.hpp>

const GLfloat DEFAULT_VIEW_ANGLE = 90.0f;
const glm::vec3 DEFAULT_POSITION = {0, 0, 0};
const glm::vec3 DEFAULT_LOOKAT = {0, 0, 1};
const GLfloat DEFAULT_NEAR = 0.1f;
const GLfloat DEFAULT_FAR = 100.0f;

class Camera {
public:
    Camera();

    virtual ~Camera();

    Camera(const Camera &c) {
        position = c.position;
        lookAt = c.lookAt;
        near = c.near;
        far = c.far;
        viewAngle = c.viewAngle;
        view = c.view;
        projection = c.projection;
        projectionView = c.projectionView;
    }

    Camera(Camera *ptrCam) {
        *this = *ptrCam;
    }

    glm::vec3 velocity = {0,0,0};
    glm::vec3 position = DEFAULT_POSITION;
    glm::vec3 lookAt = DEFAULT_LOOKAT;
    GLfloat near = DEFAULT_NEAR;
    GLfloat far = DEFAULT_FAR;
    GLfloat viewAngle = DEFAULT_VIEW_ANGLE; // Horizontal view of view in angles (by default 90-degrees (wide-angle) and 30-degress (zoomed in));

    glm::mat4 view;
    glm::mat4 projection;

    glm::mat4 projectionView; /// Projection * View

    void update();
};


#endif //OMEN_CAMERA_H
