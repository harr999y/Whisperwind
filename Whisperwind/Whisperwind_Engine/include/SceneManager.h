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
#ifndef _SCENE_MANAGER_H_
#define _SCENE_MANAGER_H_

#include <boost/function.hpp>

#include "Util.h"
#include "EngineForwardDeclare.h"

namespace Engine
{
	enum NodeType
	{
		NT_STATIC = 1 << 0,
		NT_DYNAMIC = 1 << 1,
		NT_AS_CHILD = 1 << 2
	};

	class WHISPERWIND_API SceneManager
	{
	protected:
		SceneManager()
		{}

		virtual ~SceneManager();

	public:
		void init();
		void preUpdate(Util::time elapsedTime);
		void postUpdate(Util::time elapsedTime);

		SceneNodePtr & createSceneNode(const Util::Wstring & name, Util::u_int nodeType);
		SceneNodePtr & getSceneNode(const Util::Wstring & name);
		void destroySceneNode(const Util::Wstring & name);

		SceneObjectPtr & createSceneObject(const Util::Wstring & type, const Util::Wstring & name, const Util::Wstring & resourceName);
		SceneObjectPtr & getSceneObject(const Util::Wstring & name);
		void destroySceneObject(const Util::Wstring & name);

		void loadScene(const Util::Wstring & scene);

		template <typename CallBack>
		void regPreUpdateCallback(CallBack cb) { mPreCallback = cb; }
		template <typename CallBack>
		void regPostUpdateCallback(CallBack cb) { mPostCallback = cb; }

		void regSceneObjectFactory(const SceneObjectFactoryPtr & factory);

	protected:
		void destroyAllSceneNode();
		void destroyAllSceneObject();

		virtual void createDebugRendering();

	private:
		virtual void init_impl() = 0;
		virtual SceneNodePtr createSceneNode_impl(const Util::Wstring & name, Util::u_int nodeType) = 0;
		virtual void preUpdate_impl(Util::time elapsedTime) = 0;
		virtual void postUpdate_impl(Util::time elapsedTime) = 0;
		virtual void destroySceneNode_impl(const Util::Wstring & name) = 0;
		virtual void destroyAllSceneNode_impl() = 0;

	protected:
		/// Save all nodes.CANNOT use to do anything unless find or destroy!
		SceneNodeMap mSceneNodeMap;
		SceneObjectMap mSceneObjectMap;

	private:
		typedef boost::function<void (Util::time)> Callback;
		Callback mPreCallback;
		Callback mPostCallback;

		SceneObjectFactoryMap mSceneObjectFactoryMap;

	private:
		DISALLOW_COPY_AND_ASSIGN(SceneManager);
	};
}

#endif