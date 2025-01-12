#include "rubikCube.h"

int main(){

    // Initialize GLFW, GLAD, and shaders
    if (initOpenGL() == -1) return -1;

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


    RubikCube RC = RubikCube();



    int fill = 0;

    while (!glfwWindowShouldClose(window))     {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) fill = (fill+1) % 3;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		ourShader.setMat4("model", model);

        // pt.render(fill);
        // tr.render(fill);
        // rec.render(fill);
        // cir.render(fill);

        // cube.render(fill);
        // cube2.render(fill);

        // tr.rotate(0.0,0.0,0.001);

        // rec.rotate(0.0,0.001,0.0);
        // rec.translate(0.002, 0.0,0.0);
        // rec.scale(0.001,0.001,0.001);

        // cube.rotate(0.003,0.003,0.001);
        // cube.scale(1.0001,1.0001,1.0001);
        // cube.translate(0.003,0.003,0.0);


        RC.render(fill);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}