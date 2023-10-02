#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include "flutter_embedder.h"

#define ENGINE(ptr) ((FlutterEngine)glfwGetWindowUserPointer(ptr))

static double g_ratio;
static size_t g_width;
static size_t g_height;

static size_t timestamp() {
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0) {
        fprintf(stderr, "Failed to obtain timestamp\n");
        exit(errno);
    }
    return ts.tv_sec*10e6 + ts.tv_nsec/10e3;
}

static bool opengl_make_current(void* userdata) {
    glfwMakeContextCurrent((GLFWwindow*)userdata);
    return true;
}

static bool opengl_make_resource_current(void* userdata) {
    return false;
}
    
static bool opengl_clear_current(void* userdata) {
    glfwMakeContextCurrent(NULL);
    return true;
}
    
static bool opengl_present(void* userdata) {
    glfwSwapBuffers((GLFWwindow*)userdata);
    return true;
}
    
static uint32_t opengl_fbo_callback(void* userdata) {
    return 0;  
}

static void* opengl_proc_resolver(void* userdata, const char* name) {
    return (void*)glfwGetProcAddress(name);
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: (%d) %s\n", error, description);
}

static void glfw_cursor_position_phase(GLFWwindow* window, FlutterPointerPhase phase, double x, double y) {
    FlutterPointerEvent event = {
        .struct_size = sizeof(event),
        .phase = phase,
        .timestamp = timestamp(),
        .x = (double)x * g_ratio,
        .y = (double)y * g_ratio,
    };
    FlutterEngineSendPointerEvent(ENGINE(window), &event, 1);
}

static void glfw_cursor_position_callback(GLFWwindow* window, double x, double y) {
    glfw_cursor_position_phase(window, kMove, x, y);
}

static void glfw_mouse_button_callback(GLFWwindow* window, int key, int action, int mods) {
    if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        glfw_cursor_position_phase(window, kDown, x, y);
        glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
    }

    if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        glfw_cursor_position_phase(window, kUp, x, y);
        glfwSetCursorPosCallback(window, NULL);
    }
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
    FlutterWindowMetricsEvent event = {
        .struct_size = sizeof(event),
        .width = (size_t)(width * g_ratio),
        .height = (size_t)(height * g_ratio),
        .pixel_ratio = g_ratio
    };
    FlutterEngineSendWindowMetricsEvent(ENGINE(window), &event);
}

static FlutterEngine run(GLFWwindow* window, const char* bundle, const char* icudtl) {
    FlutterRendererConfig config = {
        .type = kOpenGL,
        .open_gl = {
            .struct_size = sizeof(config.open_gl),
            .make_current = opengl_make_current,
            .clear_current = opengl_clear_current,
            .present = opengl_present,
            .fbo_callback = opengl_fbo_callback,
            .make_resource_current = opengl_make_resource_current,
            .gl_proc_resolver = opengl_proc_resolver, 
        },
    };

    const char* command_line[] = {"flutter", "--enable-impeller=true"};
    FlutterProjectArgs args = {
        .struct_size = sizeof(FlutterProjectArgs),
        .assets_path = bundle,
        .icu_data_path = icudtl,
        .command_line_argc = 2,
        .command_line_argv = command_line,
    };
  
    FlutterEngine engine = NULL;
    FlutterEngineRun(FLUTTER_ENGINE_VERSION, &config, &args, window, &engine);

    glfwSetWindowUserPointer(window, engine);
    glfw_window_size_callback(window, g_width, g_height);

    return engine;
}

#define EXIT(v) {ret = v; goto exit;}

int main(int argc, const char** argv) {
    if(argc != 3) {
        fprintf(stderr, "Unexpect input args.");
        return -1;
    }

    int ret = 0;

    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() != GLFW_TRUE) EXIT(-1);

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if(monitor == NULL) EXIT(-1);

    const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
    if(vidmode == NULL) EXIT(-1);
   
    g_width = vidmode->width;
    g_height = vidmode->height;

    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "Flutter", monitor, NULL);
    if (window == NULL) EXIT(-1);
  
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    g_ratio = width / g_width;

    FlutterEngine engine = run(window, argv[1], argv[2]);
    if(engine == NULL) {
        fprintf(stderr, "Could not run the Flutter engine.");
        EXIT(-1);
    }
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);

    printf("\n\npress 'q' to exit.\n\n");
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    FlutterEngineShutdown(engine);
    glfwDestroyWindow(window);
exit:
    glfwTerminate();
    return ret;
}
