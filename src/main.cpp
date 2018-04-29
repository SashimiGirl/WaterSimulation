#define OUTPUT_ANIMATION 0

#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "Simulator.h"
#include "LoadShader.cpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#if OUTPUT_ANIMATION
#include <opencv2/opencv.hpp>
#endif


inline float clip(const float& n, const float& lower, const float& upper)
{
    return glm::max(lower, glm::min(n, upper));
}
float theta = M_PI/8;
float phi = -M_PI/8+M_PI_2;
float dist = 2.5;
int width = 800;
int height = 800;
float zoom = 1.0f;
int frame = 0;
float rotationX = 0.0f;
float rotationY = 0.0f;
const int render_step = 3;
int mx, my;

const glm::dvec3 u = glm::dvec3(0, 1, 0);
const char* vertex_file_path = "shaders/shader.vert";
const char* fragment_file_path = "shaders/shader.frag";

GLuint pid;
glm::mat4 world_view_matrix = glm::mat4(1.0f);
glm::mat4 projection_matrix = glm::mat4(1.0f);
glm::dvec3 w = glm::dvec3(0, 0, -1);
/*
void display(void);

void reshape(int width, int height);

void idle(void)
{
    particles.step();
    glutPostRedisplay();
    if(frame/render_step >= 300)
        return;
    if(frame%render_step == 0)
    {
        #if OUTPUT_ANIMATION
        cv::Mat3b image(height, width);
        glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        cv::flip(image, image, 0);
        char fn[512];
        sprintf(fn, "result/%04d.png", frame/render_step);
        cv::imwrite(fn, image);
        #endif
    }
    frame++;
}

void mouse(int button, int state, int x, int y);

void motion(int x, int y);

void keyboard(unsigned char c, int x, int y)
{
    switch(c)
    {
    case 'o' :
        break;
    }
}
*/


void glLookAt(glm::vec3 pos, glm::vec3 dir, float d) // look at x,y,z along vector dx,dy,dz from distance -d.
{
    w = glm::normalize(dir);
    glm::vec3 eye = pos - d * dir;
    glm::vec3 up(0.0, 1.0, 0.0);
    world_view_matrix = glm::lookAt(eye, pos, up);
    GLuint loc = glGetUniformLocation(pid, "modelViewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(world_view_matrix));
}

void glFrustum(float fov, float near, float far) {
    projection_matrix = glm::frustum(-fov, fov, -fov, fov, 0.01f, 10.0f);
    //transform here
    GLuint loc = glGetUniformLocation(pid, "projectionMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection_matrix)); 
}

void keyboardFunc(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_UP:
                rotationX += 0.08f;
                break;
            case GLFW_KEY_DOWN:
                rotationX -= 0.08f;
                break;
            case GLFW_KEY_LEFT:
                rotationY += 0.08f;
                break;
            case GLFW_KEY_RIGHT:
                rotationY -= 0.08f;
                break;
            default:
                break;
        }
    }
}

void scrollFunc(GLFWwindow* win, double x, double y) {
    zoom += 0.4f*y;
}

int main(int argc, char** argv)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        exit(EXIT_FAILURE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Fluidy Fun", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyboardFunc);
    glfwSetScrollCallback(window, scrollFunc);
    //glfwSetCursorPosCallback(window, mouseFunc);
    //glfwSetMouseButtonCallback(window, clickFunc);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    /* Loop until the user closes the window */
    pid = LoadShaders(vertex_file_path, fragment_file_path);
    glUseProgram(pid);
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrustum(0.01, 0.01, 10);
   // glLookAt(glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(0.0, 0.0, -1.0), 1.0);
    glm::vec3 orig(0.0, 0.0, 0.0);
    Simulator* simulator = new Simulator(pid);
    
    for (int i = 1; i > argc; i += 1) {
      std::string path = argv[i];
      simulator->load(path);
    }
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glm::vec3 view = glm::rotate(rotationY, glm::vec3(0.0, 1.0, 0.0)) * 
                          glm::rotate(rotationX, glm::vec3(1.0, 0.0, 0.0)) * 
                          glm::vec4(0.0, 0.0, -1.0, 0.0);
        glLookAt(orig, view, zoom);

        simulator->step();

        simulator->render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

/*
void reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // your drawing code goes here
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1, 0.01, 100);
    gluLookAt(dist*sin(phi)*cos(theta), dist*cos(phi), dist*sin(phi)*sin(theta),
            0, 0, 0,
            0, 1, 0);

    particles.render();

    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mx = x;
        my = y;
    }
}

void motion(int x, int y)
{
    int dx = x - mx;
    int dy = y - my;
    mx = x;
    my = y;
    if(abs(dx) > abs(dy))
        theta += dx*0.005;
    else
        phi -= dy*0.005;
    if(theta > 2*M_PI)
        theta -= 2*M_PI;
    if(theta < -2*M_PI)
        theta += 2*M_PI;
    phi = clip(phi, M_PI/12, M_PI*11/12);
    glutPostRedisplay();
}*/
