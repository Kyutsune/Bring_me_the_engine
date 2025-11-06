#include "rendering/GpuTimer.h"

GpuTimer::GpuTimer(int bufferSize)
    : m_index(0), m_bufferSize(bufferSize) {
    m_queries.resize(bufferSize);
    for (int i = 0; i < bufferSize; ++i) {
        glGenQueries(1, &m_queries[i].start);
        glGenQueries(1, &m_queries[i].end);
    }
}

GpuTimer::~GpuTimer() {
    for (int i = 0; i < m_bufferSize; ++i) {
        glDeleteQueries(1, &m_queries[i].start);
        glDeleteQueries(1, &m_queries[i].end);
    }
}

void GpuTimer::start() {
    glQueryCounter(m_queries[m_index].start, GL_TIMESTAMP);
}

void GpuTimer::stop() {
    glQueryCounter(m_queries[m_index].end, GL_TIMESTAMP);
    m_index = (m_index + 1) % m_bufferSize;
}

bool GpuTimer::getElapsedTime(double& msOut) {
    // On lit la frame précédente
    int readIndex = (m_index + 1) % m_bufferSize;

    GLint available = 0;
    glGetQueryObjectiv(m_queries[readIndex].end, GL_QUERY_RESULT_AVAILABLE, &available);

    if (!available) {
        return false;
    }

    GLuint64 startTime, endTime;
    glGetQueryObjectui64v(m_queries[readIndex].start, GL_QUERY_RESULT, &startTime);
    glGetQueryObjectui64v(m_queries[readIndex].end, GL_QUERY_RESULT, &endTime);

    msOut = (endTime - startTime) / 1'000'000;
    return true;
}
