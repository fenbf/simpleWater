/** @file shaderLoader.cpp
*  @brief useful functions for shader management
*
*	@author Bartlomiej Filipek 
*/

#include "commonCode.h"

#include "Init.h"
#include "Log.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "shaderLoader.h"

namespace shaderLoader
{
    ///////////////////////////////////////////////////////////////////////////////
    bool gPrintFullPathToShaderFile = false;

    ///////////////////////////////////////////////////////////////////////////////
    bool loadAndBuildShaderPairFromFile(ShaderProgram *outProg, const char *vs, const char *fs)
    {
        assert(outProg != NULL && "create the object for shader first!");
        assert(vs && fs);

        Shader *vert = new Shader(Shader::Type::VERTEX);
        if (vert->loadFromFile(vs) == false || vert->compile() == false)
        {
            delete vert;
            return false;
        }
        Shader *frag = new Shader(Shader::Type::FRAGMENT);
        if (frag->loadFromFile(fs) == false || frag->compile() == false)
        {
            delete frag;
            delete vert;
            return false;
        }
        outProg->create();
        outProg->attachShader(vert);
        outProg->attachShader(frag);
        if (outProg->link() == false)
        {
            return false;
        }

        LOG_SUCCESS("program %s and %s ready!", logger::fileNameFromPath(vs), logger::fileNameFromPath(fs));

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool loadAndBuildShaderPairFromSource(ShaderProgram *outProg, const char *vsSource, const char *fsSource)
    {
        assert(outProg != NULL && "create the object for shader first!");
        assert(vsSource && fsSource);

        Shader *vert = new Shader(Shader::Type::VERTEX);
        vert->loadFromSource(vsSource);
        if (vert->compile() == false)
        {
            delete vert;
            return false;
        }
        Shader *frag = new Shader(Shader::Type::FRAGMENT);
        frag->loadFromSource(fsSource);
        if (frag->compile() == false)
        {
            delete frag;
            delete vert;
            return false;
        }
        outProg->create();
        outProg->attachShader(vert);
        outProg->attachShader(frag);
        if (outProg->link() == false)
        {
            return false;
        }

        LOG_SUCCESS("shader program from SOURCE loaded and built properly!");

        return true;
    }

} // namespce shaderLoader