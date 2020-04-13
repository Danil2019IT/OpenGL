#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

/* Create struct for our shaders */
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

/* Create function for parse our file with shaders */
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath); // Open our file

    /* Create enum class for understanding the current state of the data */
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    /* Create variable and array to store information */
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    /* Read and process our file */
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

/* Function for compiling shader */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    /* Creates a shader object */
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr); // Replaces the source code in a shader object
    glCompileShader(id); // Compiles a shader object

    /* Check for success compiles */
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message); // Return error information
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
            std::endl; // Print information about error
        std::cout << message << std::endl;
        glDeleteShader(id); // Delete shader
        return 0;
    }

    return id;
}

/* Function for creating shader */
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // Creates a program object

    /* Compiles shaders */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    /* Attaches a shader objects to a program object */
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    /* Delete unnecessary data */
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library GLFW */
    if (!glfwInit())
        return -1;

    /* Creating a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize the library GLEW */
    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    /* Print version OpenGl to console */
    std::cout << glGetString(GL_VERSION);

    /* Create vertex positions */
    float positions[] = {
       -0.5f, -0.5f,  // 0
        0.5f, -0.5f,  // 1
        0.5f,  0.5f,  // 2
       -0.5f,  0.5f   // 3

    };

    /* Creating vertex sequence */
    unsigned int indices[] = {
        0, 1, 2,
        3, 0, 2
    };


    /* Allocate memory and give data to buffer */
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    /* Define an array of generic vertex attribute data */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    /* Creating our index buffer and allocate memory for him */
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    /* Parse our file with shaders */
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    /* Creating shader using our function */
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    glUseProgram(shader); // Installs a program object as part of current rendering state

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
     
        /* Draw triangle */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    /* Delete program */
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}