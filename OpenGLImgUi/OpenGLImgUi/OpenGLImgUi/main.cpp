#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


bool prepare_shader_source(const char* fname, const char** source);

bool prepare_shader_source(const char* fname, const char** source) {
    // read the vertex shader source
    std::ifstream read_stream;
    read_stream.open(fname);
    if (!read_stream) {
        fprintf(stderr, "Cannot open stream to read, filename %s.", fname);
        return false;
    }
    const char* current_shader_source = "";
    char current_char;
    while (read_stream >> current_char) {
        current_shader_source += current_char;
    }
    *source = current_shader_source;
    read_stream.close();
    return true;
}

int main() {


    const char* vertex_shader_source_name = "shader.vert";
    const char* fragment_shader_source_name = "shader.frag";
    const char* vertex_shader_source = "";
    const char* fragment_shader_source = "";

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // read the shader into a string for later usage
    if (!prepare_shader_source(vertex_shader_source_name, &vertex_shader_source)) {
        std::cout << "Failed to prepare vertex shader." << std::endl;
        return -1;
    }

    if (!prepare_shader_source(fragment_shader_source_name, &fragment_shader_source)) {
        std::cout << "Failed to prepare fragment shader." << std::endl;
        return -1;
    }

    // ! VERTEX INPUT
    // define our vertices
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    // define the objects thats gonna hold them on the gpu
    unsigned int vertex_buffer_object;
    glGenBuffers(1, &vertex_buffer_object);

    // need to bind the object to the buffer, now our vertice object is accessable from here
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

    // copy the data to the buffers memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ! VERTEX SHADER
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    // check if the shader loading failed
    int  success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ! FRAGMENT SHADER
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    success;
    infoLog[512];
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ! SHADER PROGRAM
    unsigned int shader_program;
    // create a shader program
    shader_program = glCreateProgram();
    // link the vertex and fragment shader
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    // link the program
    glLinkProgram(shader_program);
    
    success;
    infoLog[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}