#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <stdlib.h>

#define KEY_ESCAPE 27

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

void SetupRC(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void keyHit(unsigned char c, int i, int i2)
{
    if(c=='q' || c == KEY_ESCAPE)
        exit(0);

}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(1280,720);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Simple");
    glutDisplayFunc(RenderScene);
    glutKeyboardFunc(keyHit);
    SetupRC();
    glutMainLoop();
    return 0;
}