/** @file Shader.h
*  @brief useful class for shader management and loading
*
*	@author Bartlomiej Filipek 

*/

#pragma once

/// manages single shader loading ad compilation
class Shader
{
public:
    enum class Type 
    {
        VERTEX   = GL_VERTEX_SHADER, 
        FRAGMENT = GL_FRAGMENT_SHADER, 
        GEOMETRY = GL_GEOMETRY_SHADER
    };
private:
    GLuint mId;
    Type   mType;
    std::string mName;
public:
    /// create shader, ctor is almost empty, mid is generated (by OpenGL) in loadFrom* methods
    explicit Shader(Type type);
    ~Shader();

    /// loads source of the shader and generates its ID
    /// if the shader was loaded and generated before then a new id will be created and
    /// the previous one will be destroyed
    /// @param name optional name, if NULL then the path is assigned to its internal name
    bool loadFromFile(const char *fileName, const char *name = NULL);

    /// loads source of the shader and generates its ID
    /// if the shader was loaded and generated before then a new id will be created and
    /// the previous one will be destroyed
    /// @param name optional name, if NULL then the "FROM SOURCE" is assigned to its internal name
    void loadFromSource(const char *source, const char *name = NULL);

    /// compiles the shader and checks the compilation status, outputs Error using utLOG_ERROR
    bool compile();

    GLuint getId()        const { return mId; }
    const char *getName() const { return mName.c_str(); }
private:
    void logShaderInfo();

    // block copying:
    Shader(const Shader &);
    Shader & operator=(const Shader &);
};
