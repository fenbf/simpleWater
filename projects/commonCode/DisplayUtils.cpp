#include "commonCode.h"
#include "Init.h"
#include "DisplayUtils.h"

namespace displayUtils
{
    ///////////////////////////////////////////////////////////////////////////////
    void initQuadGeometry(GLuint* vao, GLuint* vbo)
    {
        assert(vao && "vao cannot be null");
        assert(vbo && "vbo cannot be null");

        // triangle strip
        static const float quadData[] = 
        {
            -1.0f, -1.0f, 0.0f, // pos
            0.0f, 0.0f,         // tex

            -1.0f, 1.0f, 0.0f,
            0.0f, 1.0f,

            1.0f, -1.0f, 0.0f,
            1.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f,
        };

        const GLsizei STRIDE = sizeof(float)*5;

        glGenBuffers(1, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

        //
        // VAO setup
        //
        glGenVertexArrays(1, vao); 
        glBindVertexArray(*vao);

        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, STRIDE, (const void *)(sizeof(float)*3));
        CHECK_OPENGL_ERRORS();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void drawQuad(GLuint vao)
    {
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}