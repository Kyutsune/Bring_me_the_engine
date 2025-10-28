#include "rendering/Shader.h"
#include "math/Vec.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string & vertexPath, const std::string & fragmentPath, const std::string & geometryPath) {

    std::string vertCode = loadFile(vertexPath);
    std::string fragCode = loadFile(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragCode);

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

    GLuint geometryShader = 0;
    if (!geometryPath.empty()) {
        std::string geomCode = loadFile(geometryPath);
        if (geomCode.empty()) {
            std::cerr << "Erreur: Shader geometry vide ou non trouvé: " << geometryPath << std::endl;
        } else {
            geometryShader = compileShader(GL_GEOMETRY_SHADER, geomCode);
            if (geometryShader) {
                glAttachShader(ID, geometryShader);
            }
        }
    }

    glLinkProgram(ID);

    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n"
                  << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader)
        glDeleteShader(geometryShader);

    if (vertexShader == 0 || fragmentShader == 0 || (!geometryPath.empty() && geometryShader == 0)) {
        std::cerr << "Compilation échouée, shader non valide\n";
        ID = 0;
        return;
    }
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

std::string Shader::loadFile(const std::string & path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Problème dans l'ouverture de ce fichier: " << path << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(GLenum type, const std::string & source) const {
    GLuint shader = glCreateShader(type);
    const char * src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n"
                  << infoLog << "\n";
    }
    return shader;
}

void Shader::setBool(const std::string & name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string & name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string & name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string & name, const Mat4 & mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.ptr());
}

void Shader::setVec3(const std::string & name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string & name, const Vec3 & vec) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, vec.ptr());
}

void Shader::set(const std::string & name, const Vec3 & vec) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}
void Shader::set(const std::string & name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set(const std::string & name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set(const std::string & name, const Mat4 & mat, const bool & should_transpose) {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, should_transpose ? GL_TRUE : GL_FALSE, mat.ptr());
}

void Shader::setArray(const std::string & name, const GLint * values, int count) {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1iv(location, count, values);
    }
}

void Shader::setVec3Array(const std::string & name, const glm::vec3 * values, int count) {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform3fv(location, count, glm::value_ptr(values[0]));
    }
}

void Shader::setFloatArray(const std::string & name, const float * values, int count) {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1fv(location, count, values);
    }
}