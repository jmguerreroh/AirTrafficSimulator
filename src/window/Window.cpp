#include "Window.h"

#include <glad/glad.h>
// GLFW debe incluirse después de GLAD.
#include <GLFW/glfw3.h>

#include <cmath>
#include <stdexcept>
#include <string>

namespace {

thread_local std::string g_lastGlfwError;

void onGlfwError(int code, const char* description) {
    g_lastGlfwError = std::string(description ? description : "?") + " (codigo " + std::to_string(code) + ")";
}

constexpr double kClickTolerancePx = 5.0;  // mover menos que esto entre pulsar y soltar = clic, no arrastre

}  // namespace

Window::GlfwLibrary::GlfwLibrary() {
    g_lastGlfwError.clear();
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) {
        throw std::runtime_error("Simulador: no se pudo inicializar GLFW: " + g_lastGlfwError);
    }
}

Window::GlfwLibrary::~GlfwLibrary() { glfwTerminate(); }

void Window::WindowDeleter::operator()(GLFWwindow* w) const { glfwDestroyWindow(w); }

Window::Window(int width, int height, const char* title) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);  // antialiasing MSAA

    window_.reset(glfwCreateWindow(width, height, title, nullptr, nullptr));
    if (!window_) {
        throw std::runtime_error("Simulador: no se pudo crear la ventana OpenGL 3.3: " + g_lastGlfwError);
    }
    glfwMakeContextCurrent(window_.get());
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        throw std::runtime_error("Simulador: no se pudieron cargar las funciones de OpenGL");
    }
    glfwSwapInterval(1);  // sincronización vertical: ~60 FPS

    glfwSetWindowUserPointer(window_.get(), this);
    glfwSetScrollCallback(window_.get(), &Window::onScroll);
    glfwSetKeyCallback(window_.get(), &Window::onKey);
}

Window::~Window() {
    // Soltar el contexto antes de destruir la ventana (window_ se destruye después).
    glfwMakeContextCurrent(nullptr);
}

bool Window::shouldClose() const { return glfwWindowShouldClose(window_.get()) != 0; }

void Window::swapBuffers() { glfwSwapBuffers(window_.get()); }

glm::ivec2 Window::framebufferSize() const {
    int w = 0, h = 0;
    glfwGetFramebufferSize(window_.get(), &w, &h);
    return {w, h};
}

void Window::onScroll(GLFWwindow* w, double, double dy) {
    static_cast<Window*>(glfwGetWindowUserPointer(w))->scroll_ += static_cast<float>(dy);
}

void Window::onKey(GLFWwindow* w, int key, int, int action, int) {
    if (action != GLFW_PRESS) return;
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w, GLFW_TRUE);
    if (key == GLFW_KEY_R || key == GLFW_KEY_1) self->resetPressed_ = true;
    if (key == GLFW_KEY_2) self->topPressed_ = true;
}

Input Window::pollInput() {
    glfwPollEvents();
    GLFWwindow* w = window_.get();
    auto down = [&](int key) { return glfwGetKey(w, key) == GLFW_PRESS; };

    Input in;
    double x = 0.0, y = 0.0;
    glfwGetCursorPos(w, &x, &y);
    if (haveCursor_) in.cursorDelta = {static_cast<float>(x - lastX_), static_cast<float>(y - lastY_)};
    lastX_ = x;
    lastY_ = y;
    haveCursor_ = true;

    const bool left = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    in.panning = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
                 glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

    // Distinguir clic (selección) de arrastre (rotar la cámara).
    if (left && !leftWasDown_) {
        pressX_ = x;
        pressY_ = y;
        dragged_ = false;
    }
    if (left && std::hypot(x - pressX_, y - pressY_) > kClickTolerancePx) dragged_ = true;
    in.orbiting = left && dragged_;
    if (!left && leftWasDown_ && !dragged_) {
        int winW = 1, winH = 1;
        glfwGetWindowSize(w, &winW, &winH);
        in.clicked = true;
        in.clickNdc = {static_cast<float>(2.0 * x / winW - 1.0), static_cast<float>(1.0 - 2.0 * y / winH)};
    }
    leftWasDown_ = left;

    in.scroll = scroll_;
    scroll_ = 0.0f;

    if (down(GLFW_KEY_W) || down(GLFW_KEY_UP)) in.keyMove.y += 1.0f;
    if (down(GLFW_KEY_S) || down(GLFW_KEY_DOWN)) in.keyMove.y -= 1.0f;
    if (down(GLFW_KEY_D) || down(GLFW_KEY_RIGHT)) in.keyMove.x += 1.0f;
    if (down(GLFW_KEY_A) || down(GLFW_KEY_LEFT)) in.keyMove.x -= 1.0f;
    if (down(GLFW_KEY_E)) in.keyRotate += 1.0f;
    if (down(GLFW_KEY_Q)) in.keyRotate -= 1.0f;
    if (down(GLFW_KEY_EQUAL) || down(GLFW_KEY_KP_ADD)) in.keyZoom += 1.0f;
    if (down(GLFW_KEY_MINUS) || down(GLFW_KEY_KP_SUBTRACT)) in.keyZoom -= 1.0f;

    in.resetRequested = resetPressed_;
    in.topViewRequested = topPressed_;
    resetPressed_ = false;
    topPressed_ = false;
    return in;
}
