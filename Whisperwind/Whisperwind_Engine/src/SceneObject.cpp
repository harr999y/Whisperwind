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

#include <boost/foreach.hpp>

#include "DebugDefine.h"
#include "SceneComponent.h"
#include "SceneObject.h"

namespace Engine
{
	static const SceneComponentPtr NULL_SCENE_COMPONENT;
	//---------------------------------------------------------------------
	void SceneObject::preUpdate( Util::time elapsedTime )
	{
		BOOST_FOREACH(SceneComponentPtr & comp, mSceneComponents)
		{
			if (comp != NULL)
				comp->preUpdate(elapsedTime);
		}

		preUpdate_impl(elapsedTime);
	}
	//---------------------------------------------------------------------
	void SceneObject::postUpdate( Util::time elapsedTime )
	{
		BOOST_FOREACH(SceneComponentPtr & comp, mSceneComponents)
		{
			if (comp != NULL)
				comp->postUpdate(elapsedTime);
		}

		postUpdate_impl(elapsedTime);
	}
	//---------------------------------------------------------------------
	void SceneObject::regComponent(ComponentType type, const SceneComponentPtr & sceneComp)
	{
		WHISPERWIND_ASSERT((NULL == mSceneComponents[type]) && (sceneComp != NULL));

		mSceneComponents[type] = sceneComp;
	}
	//---------------------------------------------------------------------
	bool SceneObject::getComponent(ComponentType type, SceneComponentPtr & outSceneComp)
	{
		if (NULL == mSceneComponents[type])
			return false;

		outSceneComp = mSceneComponents[type];
		return true;
	}
	//---------------------------------------------------------------------
	void SceneObject::unRegComponent(ComponentType type)
	{
		mSceneComponents[type] = NULL_SCENE_COMPONENT;
	}

}