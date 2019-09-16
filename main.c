/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Bas Peeters <bas@peete.rs>
 *
 * Created on September 15, 2019, 8:38 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "util.h"

static struct {
    GLuint vertex_buffer, element_buffer;
    GLuint vertex_shader, fragment_shader, program;

    struct {
        GLint fade_factor;
    } uniforms;

    struct {
        GLint position;
    } attributes;

    GLfloat fade_factor;
} g_resources;

static GLuint make_buffer(
        GLenum target,
        const void *buffer_data,
        GLsizei buffer_size
) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);

    return buffer;
}

static void show_info_log(
        GLuint object,
        PFNGLGETSHADERIVPROC glGet__iv,
        PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
) {
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

static GLuint make_shader(GLenum type, const char *filename) {
    GLint length;
    GLchar *source = file_contents(filename, &length);
    GLuint shader;
    GLint shader_ok;

    if (!source)
        return 0;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar **) &source, &length);
    free(source);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader) {
    GLint program_ok;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

static const GLfloat g_vertex_buffer_data[] = {
         -0.5f,  0.5f,  0.0f,
          0.5f,  0.5f,  0.0f,
          0.5f, -0.5f,  0.0f,
         -0.5f, -0.5f,  0.0f,
};
static const GLushort g_element_buffer_data[] = {0, 1, 2, 3};

static int make_resources(void) {
    g_resources.vertex_buffer = make_buffer(
            GL_ARRAY_BUFFER,
            g_vertex_buffer_data,
            sizeof(g_vertex_buffer_data)
    );
    g_resources.element_buffer = make_buffer(
            GL_ELEMENT_ARRAY_BUFFER,
            g_element_buffer_data,
            sizeof(g_element_buffer_data)
    );

    g_resources.vertex_shader = make_shader(
            GL_VERTEX_SHADER,
            "shaders/hello-gl.v.glsl"
    );
    if (g_resources.vertex_shader == 0)
        return 0;

    g_resources.fragment_shader = make_shader(
            GL_FRAGMENT_SHADER,
            "shaders/hello-gl.f.glsl"
    );
    if (g_resources.fragment_shader == 0)
        return 0;

    g_resources.program = make_program(
            g_resources.vertex_shader,
            g_resources.fragment_shader
    );
    if (g_resources.program == 0)
        return 0;

    g_resources.uniforms.fade_factor = glGetUniformLocation(g_resources.program, "fade_factor");

    g_resources.attributes.position = glGetAttribLocation(g_resources.program, "position");

    return 1;
}

static void update_fade_factor(void) {
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    g_resources.fade_factor = sinf((float)milliseconds * 0.001f) * 0.5f + 0.5f;
    glutPostRedisplay();
}

static void render(void) {
    glUseProgram(g_resources.program);

    glUniform1f(g_resources.uniforms.fade_factor, g_resources.fade_factor);

    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
            g_resources.attributes.position,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *) 0
    );
    glEnableVertexAttribArray(g_resources.attributes.position);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);

    glDrawElements(
            GL_TRIANGLE_STRIP,
            sizeof(g_element_buffer_data),
            GL_UNSIGNED_SHORT,
            (void *) 0
    );

    glDisableVertexAttribArray(g_resources.attributes.position);

    glutSwapBuffers();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("C OpenGL");
    glutIdleFunc(&update_fade_factor);
    glutDisplayFunc(&render);

    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        return 1;
    }

    if (!make_resources()) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }

    glutMainLoop();
    return 0;
}
