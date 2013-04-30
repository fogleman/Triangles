#ifdef __APPLE__
    #define GLFW_INCLUDE_GL3
    #define GLFW_NO_GLU
#else
    #include <GL/glew.h>
#endif

#include <GL/glfw.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define SIZE 512

void make_rect_buffer(GLuint *position_buffer, GLuint *uv_buffer) {
    float x = SIZE;
    float y = SIZE;
    float position_data[] = {
        0, 0, x, 0, 0, y,
        0, y, x, 0, x, y
    };
    x = 1;
    y = 1;
    float uv_data[] = {
        0, 0, x, 0, 0, y,
        0, y, x, 0, x, y
    };
    *position_buffer = make_buffer(
        GL_ARRAY_BUFFER, sizeof(position_data), position_data
    );
    *uv_buffer = make_buffer(
        GL_ARRAY_BUFFER, sizeof(uv_data), uv_data
    );
}

GLuint make_triangle_buffer() {
    float x = SIZE / 2;
    float y = SIZE / 2;
    float m = 200;
    float data[] = {
        x + m * cos(RADIANS(30)), y + m * sin(RADIANS(30)),
        x + m * cos(RADIANS(170)), y + m * sin(RADIANS(170)),
        x + m * cos(RADIANS(280)), y + m * sin(RADIANS(280)),
    };
    GLuint buffer = make_buffer(
        GL_ARRAY_BUFFER, sizeof(data), data
    );
    return buffer;
}

void draw_rect(
    GLuint position_buffer, GLuint position_loc,
    GLuint uv_buffer, GLuint uv_loc,
    int size, int count)
{
    glEnableVertexAttribArray(position_loc);
    glEnableVertexAttribArray(uv_loc);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(position_loc, size, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
    glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(position_loc);
    glDisableVertexAttribArray(uv_loc);
}

void draw_triangles(GLuint buffer, GLuint position_loc, int size, int count) {
    glEnableVertexAttribArray(position_loc);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(position_loc, size, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glDisableVertexAttribArray(position_loc);
}

int main(int argc, char **argv) {
    if (!glfwInit()) {
        return -1;
    }
    #ifdef __APPLE__
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
        glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif
    if (!glfwOpenWindow(SIZE, SIZE, 8, 8, 8, 0, 24, 0, GLFW_WINDOW)) {
        return -1;
    }
    glfwSwapInterval(0);
    glfwSetWindowTitle("Triangles");

    #ifndef __APPLE__
        if (glewInit() != GLEW_OK) {
            return -1;
        }
    #endif

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    GLuint quad_program = load_program(
        "shaders/quad_vertex.glsl", "shaders/quad_fragment.glsl");
    GLuint quad_matrix_loc = glGetUniformLocation(quad_program, "matrix");
    GLuint quad_sampler_loc = glGetUniformLocation(quad_program, "sampler");
    GLuint quad_position_loc = glGetAttribLocation(quad_program, "position");
    GLuint quad_uv_loc = glGetAttribLocation(quad_program, "uv");

    GLuint triangle_program = load_program(
        "shaders/triangle_vertex.glsl", "shaders/triangle_fragment.glsl");
    GLuint triangle_matrix_loc = glGetUniformLocation(triangle_program, "matrix");
    GLuint triangle_color_loc = glGetUniformLocation(triangle_program, "triangle_color");
    GLuint triangle_position_loc = glGetAttribLocation(triangle_program, "position");

    GLuint diff_program = load_program(
        "shaders/diff_vertex.glsl", "shaders/diff_fragment.glsl");
    GLuint diff_matrix_loc = glGetUniformLocation(diff_program, "matrix");
    GLuint diff_sampler1_loc = glGetUniformLocation(diff_program, "sampler1");
    GLuint diff_sampler2_loc = glGetUniformLocation(diff_program, "sampler2");
    GLuint diff_position_loc = glGetAttribLocation(diff_program, "position");
    GLuint diff_uv_loc = glGetAttribLocation(diff_program, "uv");

    GLuint target_texture;
    glGenTextures(1, &target_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, target_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glfwLoadTexture2D("target.tga", 0);

    GLuint base_texture;
    glGenTextures(1, &base_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, base_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        SIZE, SIZE,
        0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLubyte *empty_data = calloc(SIZE * SIZE * 3, sizeof(GLubyte));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SIZE, SIZE, GL_RGB,
        GL_UNSIGNED_BYTE, empty_data);
    free(empty_data);

    GLuint render_texture;
    glGenTextures(1, &render_texture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        SIZE, SIZE,
        0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint diff_texture;
    glGenTextures(1, &diff_texture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, diff_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        SIZE, SIZE,
        0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint render_buffer;
    glGenFramebuffers(1, &render_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, render_buffer);
    glFramebufferTexture(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint diff_buffer;
    glGenFramebuffers(1, &diff_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, diff_buffer);
    glFramebufferTexture(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, diff_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FPS fps = {0, 0};
    float matrix[16];

    GLuint position_buffer;
    GLuint uv_buffer;
    make_rect_buffer(&position_buffer, &uv_buffer);
    GLuint buffer = make_triangle_buffer();

    glClearColor(1, 1, 1, 1);
    glViewport(0, 0, SIZE, SIZE);
    mat_ortho(matrix, 0, SIZE, 0, SIZE, -1, 1);

    glUseProgram(quad_program);
    glUniformMatrix4fv(quad_matrix_loc, 1, GL_FALSE, matrix);

    glUseProgram(triangle_program);
    glUniformMatrix4fv(triangle_matrix_loc, 1, GL_FALSE, matrix);

    glUseProgram(diff_program);
    glUniformMatrix4fv(diff_matrix_loc, 1, GL_FALSE, matrix);
    glUniform1i(diff_sampler1_loc, 0);
    glUniform1i(diff_sampler2_loc, 2);

    GLubyte *diff_data = calloc(SIZE * SIZE * 4, sizeof(GLubyte));

    while (glfwGetWindowParam(GLFW_OPENED)) {
        update_fps(&fps, 1);

        glBindFramebuffer(GL_FRAMEBUFFER, render_buffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(quad_program);
        glUniform1i(quad_sampler_loc, 1);
        draw_rect(
            position_buffer, quad_position_loc,
            uv_buffer, quad_uv_loc, 2, 6);
        glUseProgram(triangle_program);
        glUniform4f(triangle_color_loc, 1, 1, 0, 0.5);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        draw_triangles(buffer, triangle_position_loc, 2, 3);
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, diff_buffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(diff_program);
        draw_rect(
            position_buffer, diff_position_loc,
            uv_buffer, diff_uv_loc, 2, 6);

        glActiveTexture(GL_TEXTURE3);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, diff_data);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(quad_program);
        glUniform1i(quad_sampler_loc, 2);
        draw_rect(
            position_buffer, quad_position_loc,
            uv_buffer, quad_uv_loc, 2, 6);

        glfwSwapBuffers();
    }

    free(diff_data);
    glfwTerminate();
    return 0;
}
