// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>

#include "terrain/terrain.h"

#include "trackball.h"
#include "framebuffer/framebuffer.h"
#include "heightmap/heightmap.h"
#include "skybox/skybox.h"

Terrain terrain;
FrameBuffer framebuffer;
HeightMap heightmap;    
Terrain water;
Skybox skybox;

GLuint heightmap_texture_id;

using namespace glm;

mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;
mat4 quad_model_matrix;

int window_width = 1200;
int window_height = 1000;

float speed_x = 0.0f;
float speed_y = 0.0f;

float eps = 0.0005;

Trackball trackball;

void Init(GLFWwindow* window) {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 0.9 /*solid*/);
    
    // enable depth test.
    glEnable(GL_DEPTH_TEST);
    
    // setup view and projection matrices
    vec3 cam_pos(2.0f, 2.0f, 2.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    view_matrix = lookAt(cam_pos, cam_look, cam_up);
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 20.0f);
    
    view_matrix = translate(mat4(1.0f), vec3(0.0f, -0.3f, 0.0f));
    trackball_matrix = IDENTITY_MATRIX;
    quad_model_matrix = translate(mat4(1.0f), vec3(0.0f, -0.25f, 0.0f));
    
    // on retina/hidpi displays, pixels != screen coordinates
    // this unsures that the framebuffer has the same size as the window
    // (see http://www.glfw.org/docs/latest/window.html#window_fbsize)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    heightmap_texture_id = framebuffer.Init(window_width, window_height, true);
    heightmap.Init();
    terrain.Init(window_width, window_height, heightmap_texture_id, false);
    water.Init(window_width, window_height, heightmap_texture_id, true);
    skybox.Init();

    framebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        heightmap.Draw();
    framebuffer.Unbind();

}

// gets called for every frame.
void Display() {    
    const float time = glfwGetTime();
    
    int sign_x = speed_x / abs(speed_x);
    int sign_y = speed_y / abs(speed_y);
    
    if (abs(speed_x) >= eps)
        speed_x -= (0.00001 * sign_x);
    
    if (abs(speed_y) >= eps)
        speed_y -= (0.00001 * sign_y);
    
    if (abs(speed_x) > 0.01)
        speed_x = 0.01f * sign_x;
    
    if (abs(speed_y) > 0.01)
        speed_y = 0.01f * sign_y;
    
    if (abs(speed_x) < eps)
        speed_x = 0;
    
    if (abs(speed_y) < eps)
        speed_y = 0;
    
    view_matrix = translate(view_matrix, vec3(speed_y, 0.0f, speed_x));
    trackball_matrix = translate(trackball_matrix, vec3(speed_y, 0.0f, speed_x));

    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrain.Draw(time, trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);
    skybox.Draw(trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);
    water.Draw(time, trackball_matrix * quad_model_matrix, view_matrix, projection_matrix);
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    static float oldY = y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        // TODO 3: Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        trackball_matrix = old_trackball_matrix * trackball.Drag(p.x, p.y);
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // TODO 4: Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        float factor = y - oldY;
        oldY = y;
        view_matrix = glm::translate(view_matrix, vec3(0.0f, 0.0f, 0.03f*factor));
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        oldY = y;
    }
}

// gets called when the windows/framebuffer is resized.
void ResizeCallback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
    
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);
    
    glViewport(0, 0, window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // only act on release
    //if(action != GLFW_RELEASE) {
    //    return;
    //}
        switch(key) {
            case 'Q':
                heightmap.setH(+0.05);
                break;
            case 'W':
                heightmap.setH(+-0.05);
                break;
            case 'E':
                heightmap.setLacunarity(+0.05);
                break;
            case 'R':
                heightmap.setLacunarity(-0.05);
                break;
            case 'T':
                heightmap.setOctaves(+1);
                break;
            case 'Z':
                heightmap.setOctaves(-1);
                break;
            case 'U':
                heightmap.setOffset(+0.05);
                break;
            case 'I':
                heightmap.setOffset(-0.05);
                break;
            case 'O':
                heightmap.setGain(+0.05);
                break;
            case 'P':
                heightmap.setGain(-0.05);
                break;
            case GLFW_KEY_UP:
                speed_x += 0.001;
                break;
            case GLFW_KEY_DOWN:
                speed_x -= 0.001;
                break;
            case GLFW_KEY_LEFT:
                speed_y += 0.001;
                break;
            case GLFW_KEY_RIGHT:
                speed_y -= 0.001;
                break;
            default:
                break;
        }
}


int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height,
                                          "Trackball", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init(window);


    // render loop
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    terrain.Cleanup();
    framebuffer.Cleanup();
    heightmap.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
