#include "rubikCube.h"

int fillMode = 0;
RubikCube* RC;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_M && action == GLFW_PRESS) fillMode = (fillMode+1) % 3;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) RC->move("L1");
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) RC->move("U1");
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) RC->move("R1");
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) RC->move("D1");
    if (key == GLFW_KEY_F && action == GLFW_PRESS) RC->move("F1");
    if (key == GLFW_KEY_B && action == GLFW_PRESS) RC->move("B1");

}

int main(){

    // Initialize GLFW, GLAD, and shaders
    if (initOpenGL() == -1) return -1;

    glfwSetKeyCallback(window, key_callback);

    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);


    // Point pt(0.0f,0.0f,0.0f);
    // pt.setColor(1.0,0.0,0.0);

    // Triangle tr = Triangle(-0.5,-0.5,0.0,
    //                        0.5,-0.5,0.0,
    //                        0.0,0.5,0.0);
    // tr.setColor(0.0,1.0,0.0);
    // tr.scale(1.5,1.5,1.5);

    // Rectangle rec = Rectangle(-0.5,-0.5,0.0,
    //                          0.5,0.5,0.0);
    // rec.setColor(0.0,0.0,1.0);

    // Circle cir = Circle(0.0, 0.0,0.0,0.7, 8);
    // cir.setColor(randF(),randF(),randF());


    // Cube cube = Cube(-0.5,-0.5,-0.5,
    //                  0.5,0.5,0.5);

    // cube.translate(0.7, 0.0,0.0);
    // cube.rotate(0.0,1.0,0.0);

    // for (int i = 0; i < 6; ++i) cube.setColor(i, randF(),randF(),randF());

    RC = new RubikCube();

    while (!glfwWindowShouldClose(window))     {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        // if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) fillMode = (fillMode+1) % 3;

        // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) RC->left();
        // else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) RC->up();
        // else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) RC->right();
        // else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) RC->down();
        // else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) RC->front();
        // else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) RC->back();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		ourShader.setMat4("model", model);

        // pt.render(fillMode);
        // tr.render(fillMode);
        // rec.render(fillMode);
        // cir.render(fillMode);

        // cube.render(fillMode);
        // cube2.render(fillMode);

        // tr.rotate(0.0,0.0,0.001);

        // rec.rotate(0.0,0.001,0.0);
        // rec.translate(0.002, 0.0,0.0);
        // rec.scale(0.001,0.001,0.001);

        // cube.rotate(0.003,0.003,0.001);
        // cube.scale(1.0001,1.0001,1.0001);
        // cube.translate(0.003,0.003,0.0);

        RC->animate();
        RC->render(fillMode);

        // Animaciones utiles: 
        // RC->rotate(0.01,0.01,0.005);
        // RC->scale(0.9999,0.9999,0.9999);

        // Animaciones xd: 
        // RC->rotateX(0.01);
        // RC->rotateY(0.01);
        // RC->rotateZ(0.01);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}