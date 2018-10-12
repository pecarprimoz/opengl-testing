#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

enum IntType {
    kShader,
    kProgram
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

void prepare_shader_source(const char* fname, std::string& source);
void check_errors(unsigned int item, IntType type);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void check_errors(unsigned int obj_int, IntType type) {
    // check if the shader loading failed
    int  success;
    char infoLog[512];
    switch (type) {
        case IntType::kShader:
            glGetShaderiv(obj_int, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(obj_int, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER:COMPILATION_FAILED\n" << infoLog << std::endl;
            }
            break;

        case IntType::kProgram:
            glGetProgramiv(obj_int, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(obj_int, 512, NULL, infoLog);
                std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            break;
        default:
            break;
    }
}


void prepare_shader_source(const char* fname, std::string& source) {
    std::ifstream file;
    file.open(fname);
    std::stringstream stream;
    stream << file.rdbuf();
    source = stream.str();
}

int main() {

    const char* vertex_shader_source_name = "shader.vert";
    const char* fragment_shader_source_name = "shader.frag";
    std::string vertex_shader_source;
    std::string fragment_shader_source;

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
    prepare_shader_source(vertex_shader_source_name, vertex_shader_source);

    prepare_shader_source(fragment_shader_source_name, fragment_shader_source);



    // ! VERTEX SHADER
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_data = vertex_shader_source.data();
    glShaderSource(vertex_shader, 1, &vertex_data, nullptr);
    glCompileShader(vertex_shader);
    check_errors(vertex_shader, IntType::kShader);


    // ! FRAGMENT SHADER
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_data = fragment_shader_source.data();
    glShaderSource(fragment_shader, 1, &fragment_data, nullptr);
    glCompileShader(fragment_shader);
    check_errors(fragment_shader, IntType::kShader);

    // ! SHADER PROGRAM
    unsigned int shader_program;
    // create a shader program
    shader_program = glCreateProgram();
    // link the vertex and fragment shader
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    // link the program
    glLinkProgram(shader_program);
    check_errors(shader_program, IntType::kProgram);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // ! VERTEX INPUT
    // define our vertices
    float vertices[] = {
        // first triangle
        -0.5f, -0.5f, 0.0f, // bottom left - 0
        -0.5f,  0.5f, 0.0f, // top left - 1
         0.5f,  0.5f, 0.0f, // top right - 2
         0.5f, -0.5f, 0.0f, // bottom right - 3
         1.0f,  0.0f, 0.0f, // right - 4
         0.0f,  1.0f, 0.0f, // top - 5
        -1.0f,  0.0f, 0.0f, // left - 6
         0.0f, -1.0f, 0.0f, // bottom - 7
    };

    unsigned int indices[] = {
        // define triangles
        0, 1, 2,
        2, 3, 0,
        2, 4, 3,
        2, 5, 1,
        0, 6, 1,
        3, 7, 0
    };


    // need to create a vertex attribute objects, that holds the info for vbos
    unsigned int vertex_attribute_object;
    glGenVertexArrays(1, &vertex_attribute_object);

    // define the objects thats gonna hold them on the gpu
    unsigned int vertex_buffer_object;
    glGenBuffers(1, &vertex_buffer_object);

    unsigned int element_buffer_object;
    glGenBuffers(1, &element_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBindVertexArray(vertex_attribute_object);

    // need to bind the object to the buffer, now our vertice object is accessable from here
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    // copy the data to the buffers memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // tell opengl how to use the data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // wich attrib array to use
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // use our program
        glUseProgram(shader_program);
        // bind the attribute object first
        glBindVertexArray(vertex_attribute_object);
        // draw using the element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
        glDrawElements(GL_TRIANGLES, 6*3, GL_UNSIGNED_INT, 0);

        // glDrawArrays(GL_TRIANGLES, 0, 3);


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