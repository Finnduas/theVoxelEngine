#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <stdio.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Constants.h"
#include "Utils.h"


static SDL_Window* window = NULL;
static SDL_GLContext glcontext;

//VAO
GLuint gVertexArrayObject = 0;
//VBO
GLuint gVertexBufferObject = 0;

//Program Object (for shaders)
GLuint gGraphicsPipelineShaderProgram = 0;

//Shaders for testing purposes: TODO: implement file reading --------------------------------------------------
const char* vShaderSource =
"#version 410\n"
"in vec4 position;"
"void main() {gl_Position = vec4(position.x, position.y, position.z, position.w);}";

const char* fShaderSource =
"#version 410\n"
"out vec4 color;"
"void main() {color = vec4(0.9, 0.5, 0.1, 1.0);}";

//-------------------------------------------------------------------------------------------------------------
void cleanup() {
    SDL_GL_DestroyContext(glcontext);
    SDL_DestroyWindow(window);
}

GLuint CompileShader(GLuint shaderType, const char* shaderSource) {
    GLuint shaderObject = 0;
    
    if (shaderType == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    } 
    else if(shaderType == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    glShaderSource(shaderObject, 1, &shaderSource, NULL);
    glCompileShader(shaderObject);

    return shaderObject;
}

GLuint create_shader_program(const char* vss, const char* fss) {
    GLuint programObject = glCreateProgram();
    
    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vss);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fss);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    //validate program
    glValidateProgram(programObject);
    
    return programObject;
}

void create_graphics_pipeline() {
    gGraphicsPipelineShaderProgram = create_shader_program(vShaderSource, fShaderSource);
}

void vertex_specification() {
    GLfloat* vertexPosition = malloc(sizeof(GLfloat) * 9); //T0D0: autoscaling array
    if (vertexPosition == NULL) {
        printf("failed to initialize vertexPositions");
        exit(TRUE);
    }
    *vertexPosition = -0.8f;
    *(vertexPosition + 1) = -0.8f;
    *(vertexPosition + 2) = 0.0f;

    *(vertexPosition + 3) = 0.8f;
    *(vertexPosition + 4) = -0.8f;
    *(vertexPosition + 5) = 0.0f;

    *(vertexPosition + 6) = 0.0f;
    *(vertexPosition + 7) = 0.8f;
    *(vertexPosition + 8) = 0.0f;

    
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    glGenBuffers(1, &gVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
    //load in vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, vertexPosition, GL_STATIC_DRAW);
    //describe structure of the buffer to openGL for use with the VAO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}

void pre_draw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.1f, 0.7f, 0.5f, 1.0f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(gGraphicsPipelineShaderProgram);
}

void draw() {
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); //OPENGL version 4.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    window = SDL_CreateWindow("test", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_Log("Couldn't create window and renderer: %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    glcontext = SDL_GL_CreateContext(window);
    
    if (glcontext == NULL) {
        printf("Couldn't create OpenGL context: %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!gladLoadGLLoader(&SDL_GL_GetProcAddress)) {
        printf("Failed to load GL loader \n");
        return SDL_APP_FAILURE;
    }

    get_openGL_version_info();
    
    //graphics setup
    vertex_specification();
    create_graphics_pipeline();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        cleanup();
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate) {
    //render stuff
    pre_draw();
    draw();

    //and draw it to the screen
    SDL_GL_SwapWindow(window);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_DestroyWindow(window);
    SDL_GL_DestroyContext(glcontext);
}
