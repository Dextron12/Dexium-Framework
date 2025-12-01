

#include <iostream>

#include "windowContext.hpp"

int main(int argc, char** argv){
    Dexium::Core::windowContext window("Dexium Framework", 720, 480);

    bool appState = true;
    while (appState) {
        glClearColor(.4f, .4f, .2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.window, true);
            appState = false;
        }

        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
}
