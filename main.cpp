#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <memory>
#include <functional>
#include <c++/v1/memory>
#include "Scene.h"


std::shared_ptr<Scene> gScene = nullptr;

void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    gScene->render();
}

void SetupRC(void) {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void keyHit(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
            exit(0);

        if (gScene != nullptr) {
            if (key == GLFW_KEY_W) {
                for (auto model : gScene->m_models)
                    if (model->m_mesh->fInnerTess > 1)
                        model->m_mesh->fInnerTess -= 1;
            }
            if (key == GLFW_KEY_E) {
                for (auto model : gScene->m_models)
                    model->m_mesh->fInnerTess += 1;
            }
            if (key == GLFW_KEY_S) {
                for (auto model : gScene->m_models)
                    if (model->m_mesh->fOuterTess > 1)
                        model->m_mesh->fOuterTess -= 1;
            }
            if (key == GLFW_KEY_D) {
                for (auto model : gScene->m_models)
                    model->m_mesh->fOuterTess += 1;
            }

            if (key == GLFW_KEY_T)
                for (auto model : gScene->m_models)
                    model->m_mesh->mPolygonMode = GL_FILL;
            if (key == GLFW_KEY_Y)
                for (auto model : gScene->m_models)
                    model->m_mesh->mPolygonMode = GL_LINE;

            if (key == GLFW_KEY_1)
                for (auto model : gScene->m_models)
                    model->m_mesh->m_use_texture = (++model->m_mesh->m_use_texture) % 3;
        }
    }

    if(key == GLFW_KEY_LEFT)
        gScene->m_camera->velocity.x -= 0.2f;
    if(key == GLFW_KEY_RIGHT)
        gScene->m_camera->velocity.x += 0.2f;
    if(key == GLFW_KEY_DOWN && mods == GLFW_MOD_SHIFT)
        gScene->m_camera->velocity.y -= 0.2f;
    if(key == GLFW_KEY_UP && mods == GLFW_MOD_SHIFT)
        gScene->m_camera->velocity.y += 0.2f;

    if(key == GLFW_KEY_DOWN && mods != GLFW_MOD_SHIFT)
        gScene->m_camera->velocity.z -= 0.2f;
    if(key == GLFW_KEY_UP && mods != GLFW_MOD_SHIFT)
        gScene->m_camera->velocity.z += 0.2f;

}

class c {
public:
    Signal<std::function<void(int, int)> >& m_s;

    c(Signal<std::function<void(int, int)> >& s) : m_s(s) {
        s.connect(this, std::function<void(int, int)>([](int i1, int i2){ std::cout << "Hello world" << std::endl;}));
    }

    ~c() {
        m_s.disconnect(this);
    }

    void test(int i1, int i2) {
        std::cout << "From C: " << i1 << ", " << i2 << std::endl;
    }
};

int main(int argc, char *argv[]) {
    GLFWwindow *window;


    {
        Signal<std::function<void(int, int)> > my_signal;

        c a(my_signal);
        my_signal.connect(
                std::function<void(int, int)>([](int i1, int i2) { std::cout << i1 << ", " << i2 << std::endl; }));
        my_signal.connect(
                std::function<void(int, int)>([](int i1, int i2) { std::cout << i2 << ", " << i1 << std::endl; }));
        my_signal.connect(std::function<void(int, int)>([](int i1, int i2) { std::cout << i2 + i1 << std::endl; }));
        my_signal.emit(5, 6);
    }

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyHit);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    SetupRC();

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    gScene = std::make_shared<Scene>(Scene());
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        RenderScene();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}