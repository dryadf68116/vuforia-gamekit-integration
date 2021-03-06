/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreGLSLESLinkProgramManager.h"
#include "OgreGLSLESGpuProgram.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreGLSLESProgram.h"

// Apple doesn't define this in their extension.  We'll do it just for convenience.
// Using the value from desktop GL
#if GL_EXT_shadow_samplers
#define GL_SAMPLER_2D_SHADOW_EXT 0x8B62
#endif

namespace Ogre {

	//-----------------------------------------------------------------------
	template<> GLSLESLinkProgramManager* Singleton<GLSLESLinkProgramManager>::msSingleton = 0;

	//-----------------------------------------------------------------------
    GLSLESLinkProgramManager* GLSLESLinkProgramManager::getSingletonPtr(void)
    {
        return msSingleton;
    }

	//-----------------------------------------------------------------------
    GLSLESLinkProgramManager& GLSLESLinkProgramManager::getSingleton(void)
    {  
        assert( msSingleton );  return ( *msSingleton );  
    }

	//-----------------------------------------------------------------------
	GLSLESLinkProgramManager::GLSLESLinkProgramManager(void) :
        GLSLESProgramManagerCommon(), mActiveLinkProgram(NULL) { }

	//-----------------------------------------------------------------------
	GLSLESLinkProgramManager::~GLSLESLinkProgramManager(void)
	{
		// iterate through map container and delete link programs
		for (LinkProgramIterator currentProgram = mLinkPrograms.begin();
			currentProgram != mLinkPrograms.end(); ++currentProgram)
		{
			OGRE_DELETE currentProgram->second;
		}
	}

	//-----------------------------------------------------------------------
	GLSLESLinkProgram* GLSLESLinkProgramManager::getActiveLinkProgram(void)
	{
		// If there is an active link program then return it
		if (mActiveLinkProgram)
			return mActiveLinkProgram;

		// No active link program so find one or make a new one
		// Is there an active key?
		uint64 activeKey = 0;

		if (mActiveVertexGpuProgram)
		{
			activeKey = static_cast<uint64>(mActiveVertexGpuProgram->getProgramID()) << 32;
		}
		if (mActiveFragmentGpuProgram)
		{
			activeKey += static_cast<uint64>(mActiveFragmentGpuProgram->getProgramID());
		}

		// Only return a link program object if a vertex or fragment program exist
		if (activeKey > 0)
		{
			// Find the key in the hash map
			LinkProgramIterator programFound = mLinkPrograms.find(activeKey);
			// Program object not found for key so need to create it
			if (programFound == mLinkPrograms.end())
			{
				mActiveLinkProgram = new GLSLESLinkProgram(mActiveVertexGpuProgram,mActiveFragmentGpuProgram);
				mLinkPrograms[activeKey] = mActiveLinkProgram;
			}
			else
			{
				// Found a link program in map container so make it active
				mActiveLinkProgram = programFound->second;
			}

		}
		// Make the program object active
		if (mActiveLinkProgram) mActiveLinkProgram->activate();

		return mActiveLinkProgram;
	}

	//-----------------------------------------------------------------------
	void GLSLESLinkProgramManager::setActiveFragmentShader(GLSLESGpuProgram* fragmentGpuProgram)
	{
		if (fragmentGpuProgram != mActiveFragmentGpuProgram)
		{
			mActiveFragmentGpuProgram = fragmentGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = NULL;
		}
	}

	//-----------------------------------------------------------------------
	void GLSLESLinkProgramManager::setActiveVertexShader(GLSLESGpuProgram* vertexGpuProgram)
	{
		if (vertexGpuProgram != mActiveVertexGpuProgram)
		{
			mActiveVertexGpuProgram = vertexGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = NULL;
		}
	}
}
