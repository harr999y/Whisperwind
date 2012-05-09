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
#ifndef _SCENE_NODE_H_
#define _SCENE_NODE_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include "Util.h"
#include "EngineForwardDeclare.h"

namespace Engine
{
	class WHISPERWIND_API SceneNode : public boost::enable_shared_from_this<SceneNode>
	{
	public:
		explicit SceneNode(const Util::Wstring & name) :
		    mName(name)
		{}

	protected:
		virtual ~SceneNode()
		{}

	public:
		void addChildNode(const SceneNodePtr & sceneNode);
		void destroyChildNode(const Util::Wstring & name);
		void destroyAllChildNode();
		bool getChildNode(const Util::Wstring & name, SceneNodePtr & outChildNode);
		bool getParentNode(SceneNodePtr & outParentNode);

		/// I don't use ref here,because ref cannot auto convert derived class ptr of SceneObject to SceneObjectPtr.
		void attachSceneObject(SceneObjectPtr sceneObj);
	    void dettachSceneObject(SceneObjectPtr & sceneObj);
		void dettachAllSceneObject();

		void preUpdate(Util::time elapsedTime);
		void postUpdate(Util::time elapsedTime);

		template <typename CallBack>
		void regUpdateCallback(CallBack cb)
		{ mCallback = cb; }

	public:
		GET_CONST_VALUE(Util::Wstring, Name);
		SET_VALUE(SceneNodePtr, ParentNode);

	private:
		virtual void preUpdate_impl(Util::time elapsedTime) = 0;
		virtual void postUpdate_impl(Util::time elapsedTime) = 0;

	protected:
		SceneObjectMap mSceneObjectMap;
		SceneNodeMap mChildSceneNodeMap;
		SceneNodePtr mParentNode;
		Util::Wstring mName;

	private:
		typedef boost::function<void (SceneNodePtr, Util::time)> Callback;
		Callback mCallback;

	private:
		DISALLOW_COPY_AND_ASSIGN(SceneNode);
	};
}

#endif