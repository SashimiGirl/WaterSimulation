#define OUTPUT_ANIMATION 0

#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "Simulator.h"
#include "LoadShader.cpp"
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
int frame = 0;
int mx, my;


const int render_step = 3;
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


void glLookAt(double x, double y, double z, double dx, double dy, double dz, double d) // look at x,y,z along vector dx,dy,dz from distance -d.
{
    float fov = 0.01;
    projection_matrix = glm::frustum(-fov, fov, -fov, fov, 0.01f, 10.0f);
    //transform here
    GLuint loc = glGetUniformLocation(pid, "projectionMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection_matrix)); 

    w = glm::normalize(glm::dvec3(dx, dy, dz));
    glm::dvec3 r = glm::normalize(glm::cross(w, u));
    glm::dvec4 wl = glm::dvec4(w, 0.0);
    glm::dvec4 rl = glm::dvec4(r, 0.0);
    glm::dvec4 ul = glm::dvec4(glm::cross(r, w), 0.0);
    glm::dvec4 trans = glm::vec4(-x+d*w.x, -y+d*w.y, -z+d*w.z, 1.0);
    world_view_matrix = glm::mat4(rl, ul, -wl, trans);
    loc = glGetUniformLocation(pid, "modelViewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(world_view_matrix));
    
    //glMatrixMode( GL_PROJECTION );
    //glLoadIdentity();
    //glFrustum(-fov, fov, fov, -fov, 0.01f, 10.0f);
    
    
    
    //glMatrixMode( GL_MODELVIEW );
      //  GLdouble foo[16] = {
    //      1.0,  0.0,  0.0,  0.0,
      //    0.0,  1.0,  0.0,  0.0,
        //  0.0,  0.0,  1.0,  0.0,
          //0.0,  0.0,  -1.0,  1.0};
    
 //   GLfloat foo[16] = {
   //       r.x,  r.y,  r.z,  0.0,
     //     ul.x,  ul.y,  ul.z,  0.0,
       //  -w.x, -w.y, -w.z,  0.0,
         //-x+d*w.x, -y+d*w.y, -z+d*w.z, 1.0};
    //glLoadMatrixd(foo);
    //SDL_GL_SwapBuffers();  
    //glLoadIdentity();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glRotatef(0, 1.0f, 0.0f, 0.0f);
    //glRotatef(0, 0.0f, 1.0f, 0.0f);
    //glRotatef(0, 0.0f, 0.0f, 1.0f);
    //glTranslated(-x - d * dx, -y - d * dy, -z - d * dz);
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

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    /* Loop until the user closes the window */
    pid = LoadShaders(vertex_file_path, fragment_file_path);
    glUseProgram(pid);
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 1.5);
    
    Simulator* simulator = new Simulator(pid);
    
    for (int i = 1; i > argc; i += 1) {
      std::string path = argv[i];
      simulator->load(path);
    }
    
    //return EXIT_SUCCESS; // FIXME: temporary escape to test init.
    
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        simulator->step();
        simulator->render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        
        //return EXIT_SUCCESS; // FIXME: temporary escape to test init.
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
