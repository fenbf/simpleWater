/** @file shaderLoader.h
*  @brief useful functions for shader management
*
*	@author Bartlomiej Filipek 
*/


#pragma once

namespace textureLoader
{
    /// by default it is false and only the filename is printed
    extern bool gPrintFullPathToShaderFile;

    bool loadTexture(const char *filename, unsigned int *outTexId);
} // namespace textureLoader
