/*-------------------------------------------------------------------------
This source file is a part of Whisperwind.(GameEngine + GamePlay + GameTools)
For the latest info, see http://lisuyong.com

Copyright (c) 2012 Suyong Li (79481268@qq.com)

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
THE SOFTWARE
-------------------------------------------------------------------------*/
#ifndef _SCENE_RESOURCE_H_
#define _SCENE_RESOURCE_H_

#include "Util.h"
#include "XmlManipulator.h"
#include "EngineForwardDeclare.h"
#include "Resource.h"

namespace Engine
{
	class WHISPERWIND_API SceneResource : public Resource
	{
	public:
		SceneResource()
		{}

		~SceneResource()
		{}

	public:
		GET_VALUE(CameraPtr, Camera);

	private:
		virtual void load(const Util::Wstring & resourcePath);

		void processScene( const Util::XmlNode * sceneNode) const;
		void processCamera( const Util::XmlNode * cameraNode) const;
		void processSceneNode(const Util::XmlNode * snNode) const;
		void processChildSceneNode(const Util::XmlNode * snNode, SceneNodePtr & parentSceneNode) const;
		void processSceneObject(const Util::XmlNode * soNode, SceneNodePtr & parentSceneNode) const;
		void processLight(const Util::XmlNode * lightNode, SceneNodePtr & parentSceneNode) const;

		void processSceneNodeContent(const Util::XmlNode * snNode, SceneNodePtr & sceneNode, bool isChild) const;

	private:
		CameraPtr mCamera;
		Util::XmlReaderPtr mXmlReader;

	private:
		DISALLOW_COPY_AND_ASSIGN(SceneResource);
	};
}

#endif