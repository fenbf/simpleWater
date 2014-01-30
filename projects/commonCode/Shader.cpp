/** @file Shader.cpp
*  @brief useful class for shader management and loading
*
*	@author Bartlomiej Filipek 

*/

#include "commonCode.h"

#include "Init.h"
#include "Log.h"
#include "Shader.h"


// helpers:
const char *readAllTextFromFile(const char *fname);


///////////////////////////////////////////////////////////////////////////////
Shader::Shader(Type type) :
    mType(type),
    mId(0)
{

}

///////////////////////////////////////////////////////////////////////////////
Shader::~Shader()
{
    if (mId > 0) 
        glDeleteShader(mId);
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const char *fileName, const char *name)
{
    const char *vs = readAllTextFromFile(fileName);

    if (vs == NULL)
    {
        LOG_ERROR("cannot load shader from file %s", logger::fileNameFromPath((char *)fileName));
        return false;
    }

    if (mId > 0) 
        glDeleteShader(mId);

    mId = glCreateShader(static_cast<GLenum>(mType));
    glShaderSource(mId, 1, &vs, NULL);    

    free((void *)vs);

    // assign name:
    if (name == NULL)
        mName = std::string(logger::fileNameFromPath((char *)fileName));
    else
        mName = std::string(name);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void Shader::loadFromSource(const char *source, const char *name)
{
    if (mId > 0) 
        glDeleteShader(mId);

    mId = glCreateShader(static_cast<GLenum>(mType));

    glShaderSource(mId, 1, &source, NULL); 

    // assign name:
    if (name == NULL)
        mName = std::string("FROM SOURCE");
    else
        mName = std::string(name);
}

///////////////////////////////////////////////////////////////////////////////
bool Shader::compile()
{
    assert(mId > 0 && "id is null so the shader source should be loaded first!");

    glCompileShader(mId);

    // check:
    int compileStatus = GL_TRUE;
    glGetShaderiv(mId, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE)
    {
        LOG_ERROR("Compilation for %s shader failed!", mName.c_str());
        logShaderInfo();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void Shader::logShaderInfo()
{
    int infologLength = 0;
    glGetShaderiv(mId, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        int charsWritten  = 0;
        char *infoLog;
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(mId, infologLength, &charsWritten, infoLog);

        LOG(infoLog);

        free(infoLog);
    }
}

///////////////////////////////////////////////////////////////////////////////
//void testShaderClass()
//{
//	//
//	// simple use cases for the class
//	//
//
//	// creation & delete:
//	Shader *shader = new Shader(Shader::VERTEX);
//	delete shader;
//
//	// should not compile...:
//	/*
//	Shader testShader(Shader::VERTEX);
//	//Shader testShader = Shader::VERTEX;
//	Shader testShader2(testShader);
//	//testShader2 = testShader;
//	*/
//
//	//
//	// compilation & loading
//	//
//	std::unique_ptr<Shader> shaderPtr(new Shader(Shader::VERTEX));
//	if (shaderPtr->loadFromFile("doNotExistFile.txt"))
//		shaderPtr->compile();
//}

//////////////////////////////////////////////////////////////////////////
// from Lighthouse3D tutorial
const char *readAllTextFromFile(const char *fname) 
{
    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fname == NULL) return NULL;

#ifdef _MSC_VER
    fopen_s(&fp, fname,"rt");
#else
    fp = fopen(fname, "rt");
#endif

    if (fp == NULL) return NULL;

    fseek(fp, 0, SEEK_END);
    count = ftell(fp);
    rewind(fp);

    if (count > 0) 
    {
        content = (char *)malloc(sizeof(char) * (count+1));
        if (content)
        {
            count = (int)fread(content,sizeof(char), count, fp);
            content[count] = '\0';
        }
    }
    fclose(fp);

    return content;
}