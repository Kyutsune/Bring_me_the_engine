#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include <iostream>

struct QueryPair {
    GLuint start;
    GLuint end;
};

/**
 * @brief CLasse pour mesurer le temps de rendu GPU pour une frame.
 */
class GpuTimer {
public:
    GpuTimer(int bufferSize = 5); // Le 5 est arbitraire, normalement 2 sur le principe suffirait mais on laisse une marge.
    ~GpuTimer();

    void start();
    void stop();
    bool getElapsedTime(double & msOut);

private:
    std::vector<QueryPair> m_queries;
    int m_index;
    int m_bufferSize;
};
