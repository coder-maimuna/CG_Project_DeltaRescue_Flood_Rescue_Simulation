
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Read shader files
std::string readFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Cannot open shader: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compile a shader
GLuint compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* code = source.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader error: " << log << '\n';
    }

    return shader;
}

// Create shader program
GLuint createShaderProgram()
{
    GLuint vertex = compileShader(
        GL_VERTEX_SHADER, readFile("shaders/basic.vert")
    );

    GLuint fragment = compileShader(
        GL_FRAGMENT_SHADER, readFile("shaders/basic.frag")
    );

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking error: " << log << '\n';
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

// Unit cube vertices and indices
float vertices[] = {
    -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f
};

unsigned int indices[] = {
    0,1,2, 2,3,0,
    4,5,6, 6,7,4,
    0,4,7, 7,3,0,
    1,5,6, 6,2,1,
    0,1,5, 5,4,0,
    3,2,6, 6,7,3
};

GLuint VAO, VBO, EBO;

void createCube()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                 indices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


GLuint roofVAO, roofVBO, roofEBO;

void createRoofMesh()
{
    float roofVertices[] = {
        // Front: left eave, ridge, right eave
        -1.35f, 0.0f,  1.30f,
         0.00f, 0.9f,  1.30f,
         1.35f, 0.0f,  1.30f,

        // Back: left eave, ridge, right eave
        -1.35f, 0.0f, -1.30f,
         0.00f, 0.9f, -1.30f,
         1.35f, 0.0f, -1.30f
    };

    unsigned int roofIndices[] = {
        // Left roof slope
        0, 1, 4,
        4, 3, 0,

        // Right roof slope
        1, 2, 5,
        5, 4, 1,

        // Front and back gable triangles
        0, 1, 2,
        3, 5, 4
    };

    glGenVertexArrays(1, &roofVAO);
    glGenBuffers(1, &roofVBO);
    glGenBuffers(1, &roofEBO);

    glBindVertexArray(roofVAO);

    glBindBuffer(GL_ARRAY_BUFFER, roofVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roofVertices),
                 roofVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roofEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(roofIndices),
                 roofIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// Draw a scaled and positioned cube

void drawCube(
    GLuint shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    glm::vec3 position,
    glm::vec3 scale,
    glm::vec3 color,
    glm::vec3 rotation = glm::vec3(0.0f),
    bool animatedWater = false,
    float time = 0.0f)
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(model, glm::radians(rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));

    model = glm::rotate(model, glm::radians(rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::rotate(model, glm::radians(rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "model"),
        1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "view"),
        1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "projection"),
        1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(
        glGetUniformLocation(shader, "objectColor"),
        1, glm::value_ptr(color));

    glUniform1f(
        glGetUniformLocation(shader, "time"),
        time);

    glUniform1i(
        glGetUniformLocation(shader, "waterEffect"),
        animatedWater ? 1 : 0);

    glBindVertexArray(VAO);

    glDrawElements(
        GL_TRIANGLES,
        36,
        GL_UNSIGNED_INT,
        0);
}


void drawGableRoof(
    GLuint shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    glm::vec3 position)
{
    glm::mat4 model(1.0f);

    model = glm::translate(
        model,
        position + glm::vec3(0.0f, 1.55f, 0.0f)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "model"),
        1, GL_FALSE, glm::value_ptr(model)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "view"),
        1, GL_FALSE, glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shader, "projection"),
        1, GL_FALSE, glm::value_ptr(projection)
    );

    glBindVertexArray(roofVAO);

    // Roof: reddish-brown tin
    glUniform3f(
        glGetUniformLocation(shader, "objectColor"),
        0.48f, 0.19f, 0.10f
    );

    glUniform1i(
        glGetUniformLocation(shader, "waterEffect"), 0
    );

    glDrawElements(
        GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr
    );

    // Gable ends: match the house walls
    glUniform3f(
        glGetUniformLocation(shader, "objectColor"),
        0.76f, 0.57f, 0.36f
    );

    glDrawElements(
        GL_TRIANGLES, 6, GL_UNSIGNED_INT,
        (void*)(12 * sizeof(unsigned int))
    );

    glBindVertexArray(0);
}


void drawHouse(
    GLuint shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    glm::vec3 position,
    float time)
{
    // Main mud-colored walls
    drawCube(shader, view, projection,
        position + glm::vec3(0.0f, 0.8f, 0.0f),
        {2.0f, 1.6f, 2.0f},
        {0.76f, 0.57f, 0.36f});

    // Connected triangular roof
    drawGableRoof(shader, view, projection, position);

    // Roof ridge beam
    drawCube(shader, view, projection,
        position + glm::vec3(0.0f, 2.48f, 0.0f),
        {0.12f, 0.12f, 2.7f},
        {0.30f, 0.12f, 0.06f});

    // Front wooden veranda floor
    drawCube(shader, view, projection,
        position + glm::vec3(0.0f, 0.12f, 1.45f),
        {2.7f, 0.20f, 0.9f},
        {0.48f, 0.31f, 0.16f});

    // Veranda support pillars
    for (float x : {-1.05f, 1.05f})
    {
        drawCube(shader, view, projection,
            position + glm::vec3(x, 0.85f, 1.65f),
            {0.12f, 1.45f, 0.12f},
            {0.35f, 0.20f, 0.10f});
    }

    // Main wooden door
    drawCube(shader, view, projection,
        position + glm::vec3(0.0f, 0.55f, 1.015f),
        {0.48f, 1.05f, 0.07f},
        {0.30f, 0.13f, 0.06f});

    // Door handle
    drawCube(shader, view, projection,
        position + glm::vec3(0.16f, 0.55f, 1.06f),
        {0.05f, 0.08f, 0.025f},
        {0.85f, 0.65f, 0.18f});

    // Two blue windows
    for (float x : {-0.68f, 0.68f})
    {
        // Outer wooden frame
        drawCube(shader, view, projection,
            position + glm::vec3(x, 1.05f, 1.035f),
            {0.48f, 0.52f, 0.10f},
            {0.28f, 0.16f, 0.08f});

        // Window glass
        drawCube(shader, view, projection,
            position + glm::vec3(x, 1.05f, 1.095f),
            {0.35f, 0.39f, 0.025f},
            {0.25f, 0.55f, 0.72f});
    }
}

void drawWoodenBridge(
    GLuint shader,
    const glm::mat4& view,
    const glm::mat4& projection,
    float zPosition)
{
    glm::vec3 woodColor(0.38f, 0.21f, 0.10f);
    glm::vec3 darkWood(0.25f, 0.13f, 0.06f);

    // Wooden deck planks across the river
    for (int i = 0; i < 9; i++)
    {
        float x = -1.9f + i * 0.48f;

        drawCube(shader, view, projection,
            {x, 0.20f, zPosition},
            {0.45f, 0.14f, 0.75f},
            woodColor);
    }

    // Bridge support posts
    for (float x : {-2.0f, 2.0f})
    {
        drawCube(shader, view, projection,
            {x, 0.55f, zPosition - 0.30f},
            {0.16f, 0.8f, 0.16f},
            darkWood);

        drawCube(shader, view, projection,
            {x, 0.55f, zPosition + 0.30f},
            {0.16f, 0.8f, 0.16f},
            darkWood);
    }

    // Side railings
    for (float z : {-0.30f, 0.30f})
    {
        drawCube(shader, view, projection,
            {0.0f, 0.85f, zPosition + z},
            {4.0f, 0.12f, 0.12f},
            woodColor);
    }
}



int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720,
        "DeltaRescue - Chapter 1: Before the Flood",
        nullptr, nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "GLAD initialization failed!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint shader = createShaderProgram();
    createCube();
    createRoofMesh();

    glm::vec3 cameraPosition(19.0f, 17.0f, 23.0f);

glm::mat4 view = glm::lookAt(
    cameraPosition,
    glm::vec3(0.0f, 0.5f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

    glm::mat4 projection = glm::perspective(
    glm::radians(48.0f),
    1280.0f / 720.0f,
    0.1f,
    150.0f
);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.53f, 0.78f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        // Village terrain
        drawCube(shader, view, projection,
            {0.0f, -0.3f, 0.0f},
            {20.0f, 0.5f, 20.0f},
            {0.25f, 0.55f, 0.20f});

        // Animated river
        float currentTime = static_cast<float>(glfwGetTime());
        drawCube(shader, view, projection,
            {0.0f, -0.02f, 0.0f},
            {3.0f, 0.08f, 20.0f},
            {0.10f, 0.48f, 0.78f},
            {0.0f, 0.0f, 0.0f},
            true, currentTime);

        // Left riverbank
drawCube(shader, view, projection,
    {-1.68f, 0.02f, 0.0f},
    {0.45f, 0.18f, 20.0f},
    {0.47f, 0.35f, 0.19f});

// Right riverbank
drawCube(shader, view, projection,
    {1.68f, 0.02f, 0.0f},
    {0.45f, 0.18f, 20.0f},
    {0.47f, 0.35f, 0.19f});

        // Agricultural fields
        drawCube(shader, view, projection,
            {-6.0f, 0.0f, 5.0f},
            {5.0f, 0.08f, 5.0f},
            {0.35f, 0.68f, 0.22f});

        drawCube(shader, view, projection,
            {6.0f, 0.0f, -5.0f},
            {5.0f, 0.08f, 5.0f},
            {0.45f, 0.70f, 0.25f});

        // Traditional village houses
drawHouse(shader, view, projection,
    {-5.0f, 0.0f, -3.0f}, currentTime);

drawHouse(shader, view, projection,
    {5.0f, 0.0f, 3.0f}, currentTime);

drawHouse(shader, view, projection,
    {-7.0f, 0.0f, 5.0f}, currentTime);

        // Trees: trunks and leafy tops
        for (int i = 0; i < 5; i++)
        {
            float x = -8.0f + i * 3.5f;

            drawCube(shader, view, projection,
                {x, 0.8f, -7.0f},
                {0.35f, 1.6f, 0.35f},
                {0.35f, 0.20f, 0.08f});

            drawCube(shader, view, projection,
                {x, 2.0f, -7.0f},
                {1.8f, 1.8f, 1.8f},
                {0.08f, 0.42f, 0.12f});
        }

        // Wooden bridge crossing the river
        drawWoodenBridge(shader, view, projection, -3.0f);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &roofVAO);
glDeleteBuffers(1, &roofVBO);
glDeleteBuffers(1, &roofEBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
