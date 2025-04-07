#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int ContadorDeAnimacion = 0;
bool Animacion = false;
///////////////////////////


void ActualizarColores(vector<glm::vec3>&, vector<glm::vec3>);
GLfloat getOffsetColores(glm::vec3);
glm::vec2 getOffsetPosicion(int);

#define MAX 7776

#define RED glm::vec3(0.81,0.8,0.8) // esto es un vector de 3 componentes: x,y,z (r,g,b)
#define GREEN glm::vec3(0.79,0.8,0.8) 
#define BLUE glm::vec3(0.8,0.81,0.8)

#define WHITE glm::vec3(0.8,0.79,0.8)
#define YELLOW glm::vec3(0.8,0.8,0.81)
#define ORANGE glm::vec3(0.8,0.8,0.79)
#define BLACK glm::vec3(0.0,0.0,0.0)

// esto son las posiciones de los vertices en el vector de vertices, sirve para saber que vertices forman cada cara
#define CUBE_UP {11, 14,17, 16, 15, 12, 9, 10}  // gira en sentido antihorario
#define CUBE_DOWN {18, 21, 24, 25, 26, 23, 20, 19} // gira en sentido antihorario
#define CUBE_RIGHT {15, 16, 17, 8, 26, 25, 24, 6} // gira en sentido antihorario
#define CUBE_LEFT {11, 10, 9, 3, 18, 19, 20, 5} // gira en sentido antihorario
#define CUBE_FRONT {9, 12, 15, 6, 24, 21, 18, 3} // gira en sentido antihorario
#define CUBE_BACK {17, 14, 11, 5, 20, 23, 26, 8} // gira en sentido antihorario
#define nullTexture {glm::vec2(0.0,0.0),glm::vec2(0.1,0.0),glm::vec2(0.0,0.1),glm::vec2(0.1,0.1)} // textura nula


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

/*
class Cameraa {
public:
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); 
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Right; 
    glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // camera option
    float MovementSpeed = 8.5f;
    float MouseSensitivity = 0.1f;

    bool firstMouse = true;
    float Yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float Pitch = 0.0f; 
    float lastX = 800.0f / 2.0;
    float lastY = 600.0 / 2.0;
    float ZOOM = 45.0f; 

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            cameraPos += cameraFront * velocity;
        if (direction == BACKWARD)
            cameraPos -= cameraFront * velocity;
        if (direction == LEFT)
            cameraPos -= Right * velocity;
        if (direction == RIGHT)
            cameraPos += Right * velocity;
    }

    void updateFrame() {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
};
*/

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

class Cubito {
public:
    ///------------------------Constructores------------------------///
    Cubito(GLfloat* vertex, GLfloat* colors, int* index, GLuint program) {
        vertexNum = 48; 
        indexNum = 72;
        shaderProgram = program;
        vertices = vertex;
        colorCoords = colors;
        indices = index;
        iniciar();
    }
    Cubito(glm::vec3 center, glm::vec3 sideLength, vector<glm::vec3> colors, vector<glm::vec2> textCoords, GLuint program, unsigned int textureNum) {
        vertexNum = 48;
        indexNum = 72;
        shaderProgram = program;
        centro_cubo = center;
        texture = textureNum;
        modelo = glm::mat4(1.0);
        modelPos = glGetUniformLocation(shaderProgram, "model");
        glm::vec3 side = sideLength;
        side.x /= 2;
        side.y /= 2;
        side.z /= 2;
        vector<glm::vec3> newColors(6, BLACK);
        newColors.insert(newColors.end(), colors.begin(), colors.end());
        vector<glm::vec2> newTextCoords;
        newTextCoords.reserve(48);

        for (int i = 0; i < 6; ++i) {
            newTextCoords.push_back(glm::vec2(0.0, 0.0));
            newTextCoords.push_back(glm::vec2(0.0, 0.01));
            newTextCoords.push_back(glm::vec2(0.01, 0.01));
            newTextCoords.push_back(glm::vec2(0.01, 0.0));
        }

        newTextCoords.insert(newTextCoords.end(), textCoords.begin(), textCoords.end());
        vertices = new GLfloat[144];
        colorCoords = new GLfloat[144];
        normal = new GLfloat[144];
        textureCoords = new GLfloat[96];
        indices = new int[72];
        GLfloat newVertices[144] = {
                center.x - side.x, center.y - side.y, center.z - side.z,
                center.x + side.x, center.y - side.y, center.z - side.z,
                center.x + side.x, center.y + side.y, center.z - side.z,
                center.x - side.x, center.y + side.y, center.z - side.z,
                center.x + side.x, center.y - side.y, center.z + side.z,
                center.x - side.x, center.y - side.y, center.z + side.z,
                center.x - side.x, center.y + side.y, center.z + side.z,
                center.x + side.x, center.y + side.y, center.z + side.z,
                center.x - side.x, center.y - side.y, center.z - side.z,
                center.x - side.x, center.y - side.y, center.z + side.z,
                center.x + side.x, center.y - side.y, center.z + side.z,
                center.x + side.x, center.y - side.y, center.z - side.z,
                center.x + side.x, center.y + side.y, center.z - side.z,
                center.x + side.x, center.y + side.y, center.z + side.z,
                center.x - side.x, center.y + side.y, center.z + side.z,
                center.x - side.x, center.y + side.y, center.z - side.z,
                center.x - side.x, center.y + side.y, center.z - side.z,
                center.x - side.x, center.y + side.y, center.z + side.z,
                center.x - side.x, center.y - side.y, center.z + side.z,
                center.x - side.x, center.y - side.y, center.z - side.z,
                center.x + side.x, center.y - side.y, center.z - side.z,
                center.x + side.x, center.y - side.y, center.z + side.z,
                center.x + side.x, center.y + side.y, center.z + side.z,
                center.x + side.x, center.y + side.y, center.z - side.z,
                center.x - (side.x - 0.05), center.y - (side.y - 0.05), center.z - side.z - 0.001,
                center.x + (side.x - 0.05), center.y - (side.y - 0.05), center.z - side.z - 0.001,
                center.x + (side.x - 0.05), center.y + (side.y - 0.05), center.z - side.z - 0.001,
                center.x - (side.x - 0.05), center.y + (side.y - 0.05), center.z - side.z - 0.001,
                center.x + (side.x - 0.05), center.y - (side.y - 0.05), center.z + side.z + 0.001,
                center.x - (side.x - 0.05), center.y - (side.y - 0.05), center.z + side.z + 0.001,
                center.x - (side.x - 0.05), center.y + (side.y - 0.05), center.z + side.z + 0.001,
                center.x + (side.x - 0.05), center.y + (side.y - 0.05), center.z + side.z + 0.001,
                center.x - (side.x - 0.05), center.y - side.y - 0.001, center.z - (side.z - 0.05),
                center.x - (side.x - 0.05), center.y - side.y - 0.001, center.z + (side.z - 0.05),
                center.x + (side.x - 0.05), center.y - side.y - 0.001, center.z + (side.z - 0.05),
                center.x + (side.x - 0.05), center.y - side.y - 0.001, center.z - (side.z - 0.05),
                center.x + (side.x - 0.05), center.y + side.y + 0.001, center.z - (side.z - 0.05),
                center.x + (side.x - 0.05), center.y + side.y + 0.001, center.z + (side.z - 0.05),
                center.x - (side.x - 0.05), center.y + side.y + 0.001, center.z + (side.z - 0.05),
                center.x - (side.x - 0.05), center.y + side.y + 0.001, center.z - (side.z - 0.05),
                center.x - side.x - 0.001, center.y + (side.y - 0.05), center.z - (side.z - 0.05),
                center.x - side.x - 0.001, center.y + (side.y - 0.05), center.z + (side.z - 0.05),
                center.x - side.x - 0.001, center.y - (side.y - 0.05), center.z + (side.z - 0.05),
                center.x - side.x - 0.001, center.y - (side.y - 0.05), center.z - (side.z - 0.05),
                center.x + side.x + 0.001, center.y - (side.y - 0.05), center.z - (side.z - 0.05),
                center.x + side.x + 0.001, center.y - (side.y - 0.05), center.z + (side.z - 0.05),
                center.x + side.x + 0.001, center.y + (side.y - 0.05), center.z + (side.z - 0.05),
                center.x + side.x + 0.001, center.y + (side.y - 0.05), center.z - (side.z - 0.05) 
        };

        GLfloat newNormal[144] = { // 36 normals for 36 vertices
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , -1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, 0.0f , 1.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, -1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                0.0f, 1.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                -1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f,
                1.0f, 0.0f , 0.0f
        };

        GLubyte newIndices[72] = { // 36 faces, 72 vertices
                0, 1, 2,
                0 ,2 ,3,
                4 ,5 ,6,
                4 ,6 ,7,
                8 ,9,10,
                8 ,10 ,11,
                12 ,13 ,14,
                12 ,14 ,15,
                16 ,17 ,18,
                16 ,18 ,19,
                20 ,21 ,22,
                20 ,22 ,23,
                24, 25, 26,
                24 ,26 ,27,
                28 ,29 ,30,
                28 ,30 ,31,
                32 ,33,34,
                32 ,34 ,35,
                36 ,37 ,38,
                36 ,38 ,39,
                40 ,41 ,42,
                40 ,42 ,43,
                44 ,45 ,46,
                44 ,46 ,47
        };

        for (int i = 0; i < 72; ++i) {
            indices[i] = newIndices[i];
            vertices[i * 2] = newVertices[i * 2];
            vertices[i * 2 + 1] = newVertices[i * 2 + 1];
            normal[i * 2] = newNormal[i * 2];
            normal[i * 2 + 1] = newNormal[i * 2 + 1];
        }

        for (int i = 0; i < 144; i += 3) {
            colorCoords[i] = newColors[i / 12].x;
            colorCoords[i + 1] = newColors[i / 12].y;
            colorCoords[i + 2] = newColors[i / 12].z;
        }

        for (int i = 0; i < 48; ++i) {
            textureCoords[i * 2] = newTextCoords[i].x;
            textureCoords[i * 2 + 1] = newTextCoords[i].y;
        }
        iniciar();

        delete[] vertices;
        delete[] colorCoords;
        delete[] indices;
        delete[] normal;
        delete[] textureCoords;
    }

    ///------------------------Destructor------------------------///
    ~Cubito() {
        delete vertices;
        delete colorCoords;
        delete indices;
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(3, VBOs);
        glDeleteBuffers(1, &EBO);
    }


    void iniciar() {
        ////////////CLASICO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(4, VBOs);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertexNum * 3, vertices, GL_STATIC_DRAW);

        ///////////////////////////////////////////////////////////EnableVertexAttribArray_1
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]); ////Buffer VBO_1
        glBufferData(GL_ARRAY_BUFFER, sizeof(colorCoords[0]) * vertexNum * 3, colorCoords, GL_STATIC_DRAW);
        ///////////////////////////////////////////////////////////EnableVertexAttribArray_1
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);////Buffer VBO_2
        glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords[0]) * vertexNum * 2, textureCoords, GL_STATIC_DRAW);
        ///////////////////////////////////////////////////////////EnableVertexAttribArray_2
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);////Buffer VBO_3
        glBufferData(GL_ARRAY_BUFFER, sizeof(normal[0]) * vertexNum * 3, normal, GL_STATIC_DRAW);
        ///////////////////////////////////////////////////////////EnableVertexAttribArray_3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); ////Buffer EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indexNum, indices, GL_STATIC_DRAW);
    }

    void dibujar() {
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniformMatrix4fv(modelPos, 1, GL_FALSE, &modelo[0][0]);
        glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, 0);
    }

    void rotate(GLfloat angulo, int eje) {
        glm::vec3 direccion_rot;
        switch (eje) {
            case 0:
                direccion_rot = glm::vec3(1.0, 0.0, 0.0);
                break;
            case 1:
                direccion_rot = glm::vec3(0.0, 1.0, 0.0);
                break;
            case 2:
                direccion_rot = glm::vec3(0.0, 0.0, 1.0);
                break;
        }
        modelo = glm::rotate(glm::mat4(1.0f), angulo, direccion_rot) * modelo;
        centro_cubo = glm::mat3(glm::rotate(glm::mat4(1.0f), angulo, direccion_rot)) * centro_cubo;
    }

    void traslate(glm::vec3 traslacion) {
        modelo = glm::translate(glm::mat4(1.0f), traslacion) * modelo;
        centro_cubo = glm::mat3(glm::translate(glm::mat4(1.0f), traslacion)) * centro_cubo;
    }

    void Alejarse(glm::vec3 centro, float distancia) {
        glm::vec3 direccion_alej = centro_cubo - centro;
        if (direccion_alej.x > 0) {
            direccion_alej.x = 1;
        }
        if (direccion_alej.y > 0) {
            direccion_alej.y = 1;
        }
        if (direccion_alej.z > 0) {
            direccion_alej.z = 1;
        }
        modelo = glm::translate(glm::mat4(1.0), direccion_alej * distancia) * modelo;
        centro_cubo = glm::mat3(glm::translate(glm::mat4(1.0), direccion_alej * distancia)) * centro_cubo;
    }

    void Mover_Cubito(float distancia) {
        if (centro_cubo.x > 0) { 
            centro_cubo.x = 1; 
        }
        if (centro_cubo.y > 0) { 
            centro_cubo.y = 1;
        }
        if (centro_cubo.z > 0) {
            centro_cubo.z = 1;
        }
        modelo = glm::translate(glm::mat4(1.0), centro_cubo * distancia) * modelo; 
        centro_cubo = glm::mat3(glm::translate(glm::mat4(1.0), centro_cubo * distancia)) * centro_cubo; 

    }
    ////// 

    //funcion trasladar durante un tiempo definido 
    void trasladar(glm::vec3 traslacion, int tiempo) {
        if (ContadorDeAnimacion < tiempo) { // ContadorDeAnimacion es una variable global que se inicializa en 0, ejemplo: int ContadorDeAnimacion = 0;
            modelo = glm::translate(glm::mat4(1.0f), traslacion) * modelo;
            centro_cubo = glm::mat3(glm::translate(glm::mat4(1.0f), traslacion)) * centro_cubo;
            ContadorDeAnimacion++;
        }
        else {
            Animacion = false; // Animacion es una variable global que se inicializa en false, ejemplo: bool Animacion = false;
            ContadorDeAnimacion = 0;
        }
    }
    ////////

    ///------------------------Variables------------------------///
    GLfloat* vertices, * colorCoords, * textureCoords, * normal;
    GLuint modelPos;
    glm::mat4 modelo;
    glm::vec3 centro_cubo;
    unsigned int shaderProgram, texture;
    unsigned int VAO, VBOs[3], EBO;
    unsigned int vertexNum, indexNum;
    int orientation;
    int* indices;
};

class Rubik {
public:
    ///------------------------Constructores------------------------///
    Rubik(){};

    Rubik(glm::vec3 mid_, GLfloat arista_, GLfloat offset_, GLuint shaderProgram_, unsigned int numTexturauwu) :cubitos(27, nullptr), Animacion(false), ContadorDeAnimacion(0), TiempoDeAnimacion(30), texturauwu(numTexturauwu) {
        this->midX = mid_.x;
        this->midY = mid_.y;
        this->midZ = mid_.z;
        this->arista = arista_;
        this->offset = offset_;
        this->shaderProgram = shaderProgram_;

        ReinicioColores();
    }
    ///------------------------Destructor------------------------///
    ~Rubik() {
        for (Cubito* cubito : cubitos) {
            delete cubito;
        }
    }

    ///------------------------Funciones------------------------///
    void Generador() {
        vector<glm::vec3> auxColores(6, glm::vec3(0.1));
        glm::vec3 tamDistribucion[3][3][3];
        getTamanos(tamDistribucion);
        glm::vec3 centros[3][3][3];
        getCentros(centros);
        /// F -> Cara Amarila
        /// L -> Cara Azul
        /// B -> Cara Blanca
        /// R -> Cara Verde
        /// U -> Cara Naranja
        /// D -> Cara Roja
        ActualizarColores(auxColores, { BLACK,BLACK,BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("F5")]) }); 
        cubitos[0] = new Cubito(centros[2][1][1], tamDistribucion[2][1][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 0, 0, 0, 5 }), shaderProgram, texturauwu);//F5
        ActualizarColores(auxColores, { BLACK,BLACK,BLACK,BLACK,BLACK,BLACK });
        cubitos[1] = new Cubito(centros[1][1][1], tamDistribucion[1][1][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 0, 0, 0, 0 }), shaderProgram, texturauwu);//0
        ActualizarColores(auxColores, { BLACK,BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("B5")]),BLACK });
        cubitos[2] = new Cubito(centros[0][1][1], tamDistribucion[0][1][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 0, 0, 5, 0 }), shaderProgram, texturauwu);//B5
        ActualizarColores(auxColores, { BLACK,BLACK,getVarColor(colores[getPosicionDuple("L6")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("F4")]) });
        cubitos[3] = new Cubito(centros[2][0][1], tamDistribucion[2][0][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 6, 0, 0, 4 }), shaderProgram, texturauwu);//F4-L6
        ActualizarColores(auxColores, { BLACK,BLACK,getVarColor(colores[getPosicionDuple("L5")]),BLACK,BLACK,BLACK });
        cubitos[4] = new Cubito(centros[1][0][1], tamDistribucion[1][0][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 5, 0, 0, 0 }), shaderProgram, texturauwu);//L5
        ActualizarColores(auxColores, { BLACK,BLACK,getVarColor(colores[getPosicionDuple("L4")]),BLACK,getVarColor(colores[getPosicionDuple("B6")]),BLACK });
        cubitos[5] = new Cubito(centros[0][0][1], tamDistribucion[0][0][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 4, 0, 6, 0 }), shaderProgram, texturauwu);//B6-L4
        ActualizarColores(auxColores, { BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("R4")]),BLACK,getVarColor(colores[getPosicionDuple("F6")]) });
        cubitos[6] = new Cubito(centros[2][2][1], tamDistribucion[2][2][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 0, 4, 0, 6 }), shaderProgram, texturauwu);//F6-R4
        ActualizarColores(auxColores, { BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("R5")]),BLACK,BLACK });
        cubitos[7] = new Cubito(centros[1][2][1], tamDistribucion[1][2][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 0, 5, 0, 0 }), shaderProgram, texturauwu);//R5
        ActualizarColores(auxColores, { BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("R6")]),getVarColor(colores[getPosicionDuple("B4")]),BLACK });
        cubitos[8] = new Cubito(centros[0][2][1], tamDistribucion[0][2][1], auxColores, getTexturasXYZ(auxColores, {0, 0, 0, 6, 4, 0 }), shaderProgram, texturauwu);//R6-B4
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U7")]),getVarColor(colores[getPosicionDuple("L3")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("F1")]) });																																						 //---
        cubitos[9] = new Cubito(centros[2][0][2], tamDistribucion[2][0][2], auxColores, getTexturasXYZ(auxColores, {0, 7, 3, 0, 0, 1 }), shaderProgram, texturauwu);//F1-L3-U7
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U4")]),getVarColor(colores[getPosicionDuple("L2")]),BLACK,BLACK,BLACK });
        cubitos[10] = new Cubito(centros[1][0][2], tamDistribucion[1][0][2], auxColores, getTexturasXYZ(auxColores, {0, 4, 2, 0, 0, 0 }), shaderProgram, texturauwu);//L2-U4
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U1")]),getVarColor(colores[getPosicionDuple("L1")]),BLACK,getVarColor(colores[getPosicionDuple("B3")]),BLACK });
        cubitos[11] = new Cubito(centros[0][0][2], tamDistribucion[0][0][2], auxColores, getTexturasXYZ(auxColores, {0, 1, 1, 0, 3, 0 }), shaderProgram, texturauwu);//B3-L1-U1
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U8")]),BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("F2")]) });
        cubitos[12] = new Cubito(centros[2][1][2], tamDistribucion[2][1][2], auxColores, getTexturasXYZ(auxColores, {0, 8, 0, 0, 0, 2 }), shaderProgram, texturauwu);//F2-U8
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U5")]),BLACK,BLACK,BLACK,BLACK });
        cubitos[13] = new Cubito(centros[1][1][2], tamDistribucion[1][1][2], auxColores, getTexturasXYZ(auxColores, {0, 5, 0, 0, 0, 0 }), shaderProgram, texturauwu);//U5
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U2")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("B2")]),BLACK });
        cubitos[14] = new Cubito(centros[0][1][2], tamDistribucion[0][1][2], auxColores, getTexturasXYZ(auxColores, {0, 2, 0, 0, 2, 0 }), shaderProgram, texturauwu);//U2-B8
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U9")]),BLACK,getVarColor(colores[getPosicionDuple("R1")]),BLACK,getVarColor(colores[getPosicionDuple("F3")]) });
        cubitos[15] = new Cubito(centros[2][2][2], tamDistribucion[2][2][2], auxColores, getTexturasXYZ(auxColores, {0, 9, 0, 1, 0, 3 }), shaderProgram, texturauwu);//F3-U9-R1
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U6")]),BLACK,getVarColor(colores[getPosicionDuple("R2")]),BLACK,BLACK });
        cubitos[16] = new Cubito(centros[1][2][2], tamDistribucion[1][2][2], auxColores, getTexturasXYZ(auxColores, {0, 6, 0, 2, 0, 0 }), shaderProgram, texturauwu);//R2-U8
        ActualizarColores(auxColores, { BLACK,getVarColor(colores[getPosicionDuple("U3")]),BLACK,getVarColor(colores[getPosicionDuple("R3")]),getVarColor(colores[getPosicionDuple("B1")]),BLACK });
        cubitos[17] = new Cubito(centros[0][2][2], tamDistribucion[0][2][2], auxColores, getTexturasXYZ(auxColores, {0, 3, 0, 3, 1, 0 }), shaderProgram, texturauwu);//R3-B1-U3
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D1")]),BLACK,getVarColor(colores[getPosicionDuple("L9")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("F7")]) });
        cubitos[18] = new Cubito(centros[2][0][0], tamDistribucion[2][0][0], auxColores, getTexturasXYZ(auxColores, {1, 0, 9, 0, 0, 7 }), shaderProgram, texturauwu);//F7-L9-D1
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D4")]),BLACK,getVarColor(colores[getPosicionDuple("L8")]),BLACK,BLACK,BLACK });
        cubitos[19] = new Cubito(centros[1][0][0], tamDistribucion[1][0][0], auxColores, getTexturasXYZ(auxColores, {4, 0, 8, 0, 0, 0 }), shaderProgram, texturauwu);//L8-D2
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D7")]),BLACK,getVarColor(colores[getPosicionDuple("L7")]),BLACK,getVarColor(colores[getPosicionDuple("B9")]),BLACK });
        cubitos[20] = new Cubito(centros[0][0][0], tamDistribucion[0][0][0], auxColores, getTexturasXYZ(auxColores, {7, 0, 7, 0, 9, 0 }), shaderProgram, texturauwu);//L7-B9-D3
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D2")]),BLACK,BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("F8")]) });
        cubitos[21] = new Cubito(centros[2][1][0], tamDistribucion[2][1][0], auxColores, getTexturasXYZ(auxColores, {2, 0, 0, 0, 0, 8 }), shaderProgram, texturauwu);//F8-D2
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D5")]),BLACK,BLACK,BLACK,BLACK,BLACK });
        cubitos[22] = new Cubito(centros[1][1][0], tamDistribucion[1][1][0], auxColores, getTexturasXYZ(auxColores, {5, 0, 0, 0, 0, 0 }), shaderProgram, texturauwu);//D5
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D8")]),BLACK,BLACK,BLACK,getVarColor(colores[getPosicionDuple("B8")]),BLACK });
        cubitos[23] = new Cubito(centros[0][1][0], tamDistribucion[0][1][0], auxColores, getTexturasXYZ(auxColores, {8, 0, 0, 0, 8, 0 }), shaderProgram, texturauwu);//B8-D2
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D3")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("R7")]),BLACK,getVarColor(colores[getPosicionDuple("F9")]) });
        cubitos[24] = new Cubito(centros[2][2][0], tamDistribucion[2][2][0], auxColores, getTexturasXYZ(auxColores, {3, 0, 0, 7, 0, 9 }), shaderProgram, texturauwu);//F9-R7-D3
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D6")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("R8")]),BLACK,BLACK });
        cubitos[25] = new Cubito(centros[1][2][0], tamDistribucion[1][2][0], auxColores, getTexturasXYZ(auxColores, {6, 0, 0, 8, 0, 0 }), shaderProgram, texturauwu);//D6-R8
        ActualizarColores(auxColores, { getVarColor(colores[getPosicionDuple("D9")]),BLACK,BLACK,getVarColor(colores[getPosicionDuple("R9")]),getVarColor(colores[getPosicionDuple("B7")]),BLACK });
        cubitos[26] = new Cubito(centros[0][2][0], tamDistribucion[0][2][0], auxColores, getTexturasXYZ(auxColores, {9, 0, 0, 9, 7, 0 }), shaderProgram, texturauwu);//R9-B7-D1

        for (auto cubito : cubitos) {
            cubito->traslate(-centros[1][1][1]);
        }
        midX = centros[1][1][1].x;
        midY = centros[1][1][1].y;
        midZ = centros[1][1][1].z;
    }

    void Dibujar() {
        for (Cubito* cubito : cubitos) {
            cubito->dibujar();
        }
    }

    void Movimientos() {
        if (!ColaDesordenada.empty()) {
            if (!Animacion) {
                AnimacionActual = ColaDesordenada.front();
                setAnimacion(AnimacionActual);
                ColaDesordenada.pop();
            }
        }
        if (!ColaDeSolucion.empty()) {
            if (!Animacion) {
                AnimacionActual = ColaDeSolucion.front();
                setAnimacion(AnimacionActual);
                ColaDeSolucion.pop();
            }
        }
        if (ContadorDeAnimacion == TiempoDeAnimacion) {
            Animacion = false;
            ActualizarIndice();
            ContadorDeAnimacion++;
        }
        if (Animacion) {
            Mover();
            ContadorDeAnimacion++;
        }
    }

    void Desordenador(vector<string> vecDesorden_) {
        TempMovimientos = vecDesorden_;	//Guardo los movimientos para usarlos para resolver
        for (string move : vecDesorden_) {
            ColaDesordenada.push(move);
        }
    }

    void Expandir(float distancia_) {
        for (Cubito* cubito : cubitos) {
            if (cubito != cubitos[1])
                cubito->Alejarse(glm::vec3(midX, midY, midZ), distancia_);
        }
    }

    /*void Moverrr(float distancia_) {
        for (Cubito* cubito : cubitos) {
            //trasladar el cubo hacia arriba 
            cubito->traslate(glm::vec3(0.0f, distancia_, 0.0f));  // los parametros son los valores de traslacion en x,y,z
            //if (cubito != cubitos[1])
                //cubito->Mover_Cubito(distancia_);
        }
    }*/

    ///mover hacia el norte 
    void move_north(float distancia_){
        for (Cubito* cubito : cubitos) 
            cubito->traslate(glm::vec3(0.0f, distancia_, 0.0f));  
    }

    //mover hacia el sur
    void move_south(float distancia_){
        for (Cubito* cubito : cubitos) 
            cubito->traslate(glm::vec3(0.0f, -distancia_, 0.0f));  
    }

    //mover hacia el este
    void move_east(float distancia_){
        for (Cubito* cubito : cubitos) 
            cubito->traslate(glm::vec3(distancia_, 0.0f, 0.0f));  
    }

    //mover hacia el oeste
    void move_west(float distancia_){
        for (Cubito* cubito : cubitos) 
            cubito->traslate(glm::vec3(-distancia_, 0.0f, 0.0f));  
    }

    ///////////////////////////////

    void ReinicioColores() {
        this->colores = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"; //Colores por defecto
    }

    void setColores(string colores_) {
        this->colores = colores_;
    }

    void setOffset(GLfloat offset_) {
        this->offset = offset_;
    }

    bool setAnimacion(string movimiento_) {
        if (!Animacion) {
            AnimacionActual = movimiento_;
            Animacion = true;
            ContadorDeAnimacion = 0;
            if (AnimacionActual.length() == 2 && AnimacionActual[1] == '2')
                TiempoDeAnimacion = 60;
            else
                TiempoDeAnimacion = 30;
            return true;
        }
        return false;
    }
    
    void Solve(vector<string> solucion_) {
        for (string move : solucion_) {
            ColaDeSolucion.push(move);
        }
    }

    vector<string> getSolucion() {
        return TempMovimientos;
    }

    void AgregarCentros(vector<int>& indices_) {
        switch (AnimacionActual[0]) {
            case 'U':
                indices_.push_back(13);  // 13 es el indice del centro de la cara U
                break;
            case 'D':
                indices_.push_back(22); // 22 es el indice del centro de la cara D
                break;
            case 'R':
                indices_.push_back(7); // 7 es el indice del centro de la cara R
                break;
            case 'L':
                indices_.push_back(4); // 4 es el indice del centro de la cara L
                break;
            case 'F':
                indices_.push_back(0); // 0 es el indice del centro de la cara F
                break;
            case 'B':
                indices_.push_back(2); // 2 es el indice del centro de la cara B
                break;           
        }
    }

    void Mover() {
        vector<int> camadas = getIndices();
        AgregarCentros(camadas);
        int axis;
        GLfloat angulo = getAngulo();
        angulo *= glm::pi<float>() / 60;
        switch (AnimacionActual[0]) {
            case 'U':
                axis = 2; // 2 es el eje Z
                break;
            case 'D':
                axis = 2; // 2 es el eje Z
                break;
            case 'R':
                axis = 1; // 1 es el eje Y
                break;
            case 'L':
                axis = 1; // 1 es el eje Y
                break;
            case 'F':
                axis = 0; // 0 es el eje X
                break;
            case 'B':
                axis = 0; // 0 es el eje X
                break;
        }
        if (AnimacionActual.length() == 2 && AnimacionActual[1] == '\'')
            angulo *= -1;
        for (int i : camadas) {
            cubitos[i]->rotate(angulo, axis);////////////////// el cambio esta aqui
        }
    }

    void ActualizarIndice() {
        int offset = 6;
        if (AnimacionActual.length() == 2 && AnimacionActual[1] == '\'')
            offset = 2;
        if (AnimacionActual.length() == 2 && AnimacionActual[1] == '2')
            offset = 4;

        vector<int> camadas = getIndices();
        vector<Cubito*> cubitosOriginales = cubitos;
        for (size_t i = 0, top = camadas.size(); i < top; ++i) {
            cubitos[camadas[i]] = cubitosOriginales[camadas[(i + offset) % top]];
        }
    }

    void getCentros(glm::vec3 cubo[3][3][3]) {
        glm::vec3 tamDistribucion[3][3][3];
        getTamanos(tamDistribucion);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 3; ++k) {
                    cubo[i][j][k].x = tamDistribucion[i][j][k].x / 2;
                    if (i > 0)
                        cubo[i][j][k].x += cubo[i - 1][j][k].x + tamDistribucion[i - 1][j][k].x / 2 + offset;

                    cubo[i][j][k].y = tamDistribucion[i][j][k].y / 2;
                    if (j > 0)
                        cubo[i][j][k].y += cubo[i][j - 1][k].y + tamDistribucion[i][j - 1][k].y / 2 + offset;

                    cubo[i][j][k].z = tamDistribucion[i][j][k].z / 2;
                    if (k > 0)
                        cubo[i][j][k].z += cubo[i][j][k - 1].z + tamDistribucion[i][j][k - 1].z / 2 + offset;
                }
            }
        }
    }

    void getTamanos(glm::vec3 cubo[3][3][3]) {
        float cambio = 0.5;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[0][i][j].x = cambio * 3 * arista;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[1][i][j].x = cambio * 3 * arista;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[2][i][j].x = cambio * 3 * arista;
            }
        }


        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[i][0][j].y = cambio * 3 * arista;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[i][1][j].y = cambio * 3 * arista;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[i][2][j].y = cambio * 3 * arista;
            }
        }


        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[i][j][0].z = cambio * 3 * arista;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[i][j][1].z = cambio * 3 * arista;
            }
        }
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cubo[i][j][2].z = cambio * 3 * arista;
            }
        }
    }

    unsigned int getPosicionDuple(string duple_) {
        int lado, num;
        switch (duple_[0]) {
            case 'F':
                lado = 2; // 2 es el eje Z
                break;
            case 'D':
                lado = 3; // 3 es el eje Y
                break;
            case 'U':
                lado = 0; // 0 es el eje Y
                break;
            case 'B':
                lado = 5; // 5 es el eje Z
                break;
            case 'R':
                lado = 1; // 1 es el eje X
                break;
            case 'L':
                lado = 4;   // 4 es el eje X
                break;
        }
        num = duple_[1] - '0'; 
        return lado * 9 + num - 1;
    }

    GLfloat getAngulo() {
        float angulo;
        switch (AnimacionActual[0]) {
            case 'U':
                angulo = -1.0; // -1.0 porque es en sentido horario (visto desde arriba) 
                break;
            case 'D':
                angulo = 1.0; // 1.0 porque es en sentido antihorario (visto desde abajo)
                break;
            case 'R':
                angulo = -1.0;  // -1.0 porque es en sentido horario (visto desde la derecha)   
                break;
            case 'L':
                angulo = 1.0;   // 1.0 porque es en sentido antihorario (visto desde la izquierda)
                break;
            case 'F':
                angulo = -1.0;  // -1.0 porque es en sentido horario (visto desde adelante)
                break;
            case 'B':
                angulo = 1.0;   // 1.0 porque es en sentido antihorario (visto desde atras)
                break;
        }
        return angulo;
    }

    vector<int> getIndices() {
        vector<int> camadas;
        switch (AnimacionActual[0]) {
            case 'U':
                camadas = CUBE_UP;
                break;
            case 'D':
                camadas = CUBE_DOWN;
                break; 
            case 'R':
                camadas = CUBE_RIGHT;
                break;
            case 'L':
                camadas = CUBE_LEFT;
                break;
            case 'F':
                camadas = CUBE_FRONT;
                break;
            case 'B':
                camadas = CUBE_BACK;
                break;
        }
        return camadas;
    }

    vector<glm::vec2> getTexturasXYZ(vector<glm::vec3> colores_, vector<int> posicion_) {
        vector<glm::vec2> coordenadas;
        glm::vec2 offset;
        coordenadas.reserve(24);
        GLfloat offsetX, offsetY, varOffset = 1.0 / 18.0;
        for (size_t i = 0, top = colores_.size(); i < top; ++i) {
            if (colores_[i] != BLACK) {
                offsetX = getOffsetColores(colores_[i]);
                offsetY = 0;
                offset = getOffsetPosicion(posicion_[i]);
                offsetX += offset.x;
                offsetY += offset.y;
                coordenadas.push_back(glm::vec2(offsetX, offsetY + varOffset * 6));
                coordenadas.push_back(glm::vec2(offsetX, offsetY));
                coordenadas.push_back(glm::vec2(offsetX + varOffset, offsetY));
                coordenadas.push_back(glm::vec2(offsetX + varOffset, offsetY + varOffset * 6));
            }
            else {
                coordenadas.push_back(glm::vec2(0.0, 0.0));
                coordenadas.push_back(glm::vec2(0.0, 0.01));
                coordenadas.push_back(glm::vec2(0.01, 0.01));
                coordenadas.push_back(glm::vec2(0.01, 0.0));
            }

        }
        return coordenadas;
    }

    glm::vec3 getVarColor(char c) {
        glm::vec3 color;
        switch (c) {
            case 'F':
                color = YELLOW;
                break;
            case 'D':
                color = GREEN;
                break;
            case 'U':
                color = BLUE;
                break;
            case 'B':
                color = WHITE;
                break;
            case 'R':
                color = ORANGE;
                break;
            case 'L':
                color = RED;
                break;
        }
        return color;
    }

    void ActualizarColores(vector<glm::vec3>& colores_, vector<glm::vec3> coloresNuevos_) {
        for (size_t i = 0, top = coloresNuevos_.size(); i < top; ++i) {
            colores_[i] = coloresNuevos_[i];
        }
    }

    GLfloat getOffsetColores(glm::vec3 color_) {
        GLfloat offset, varColor = 1.0 / 6.0;

        if (color_ == YELLOW) {
            offset = 0.0;
        }
        else if (color_ == ORANGE) {
            offset = varColor;
        }
        else if (color_ == BLUE) {
            offset = varColor * 2;
        }
        else if (color_ == WHITE) {
            offset = varColor * 3;
        }
        else if (color_ == RED) {
            offset = varColor * 4;
        }
        else if (color_ == GREEN) {
            offset = varColor * 5;
        }
        return offset;
    }

    glm::vec2 getOffsetPosicion(int posicion_) {
        float varOffset = 1.0 / 18.0;
        return glm::vec2(varOffset * ((posicion_ - 1) % 3), varOffset * ((posicion_ - 1) / 3) * 6);
    }
    ///------------------------Variables------------------------///
    vector<Cubito*> cubitos;
    GLfloat midX;
    GLfloat midY;
    GLfloat midZ;
    GLfloat arista;
    GLfloat offset;
    GLuint shaderProgram;
    string colores;
    unsigned int texturauwu;
    bool Animacion;
    string AnimacionActual;
    int ContadorDeAnimacion;
    int TiempoDeAnimacion;
    vector<string> TempMovimientos;
    queue<string> ColaDesordenada;
    queue<string> ColaDeSolucion;
};

void ActualizarColores(vector<glm::vec3>& colores_, vector<glm::vec3> coloresNuevos_) {
    for (size_t i = 0, top = coloresNuevos_.size(); i < top; ++i) {
        colores_[i] = coloresNuevos_[i];
    }
}

GLfloat getOffsetColores(glm::vec3 color_) {
    GLfloat offset, varColor = 1.0 / 6.0;

    if (color_ == YELLOW) {
        offset = 0.0;
    }
    else if (color_ == ORANGE) {
        offset = varColor;
    }
    else if (color_ == BLUE) {
        offset = varColor * 2;
    }
    else if (color_ == WHITE) {
        offset = varColor * 3;
    }
    else if (color_ == RED) {
        offset = varColor * 4;
    }
    else if (color_ == GREEN) {
        offset = varColor * 5;
    }
    return offset;
}

glm::vec2 getOffsetPosicion(int posicion_) {
    float varOffset = 1.0 / 18.0;
    return glm::vec2(varOffset * ((posicion_ - 1) % 3), varOffset * ((posicion_ - 1) / 3) * 6);
}

///------------------------Funciones------------------------///
void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);