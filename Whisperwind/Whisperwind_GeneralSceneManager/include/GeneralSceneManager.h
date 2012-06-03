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
#ifndef _GENERAL_SCENE_MANAGER_H_
#define _GENERAL_SCENE_MANAGER_H_

#include <boost/enable_shared_from_this.hpp>

#include "SceneManager.h"
#include "GeneralForwardDeclare.h"

namespace Engine
{
	
	/** This structure has been refered to Clayman's very article:http://blog.csdn.net/soilwork/article/details/4131367
	      Thanks very much Clayman!
	*/
	class GeneralSceneManager : public SceneManager, public boost::enable_shared_from_this<GeneralSceneManager>
	{
	public:
		GeneralSceneManager();

		~GeneralSceneManager();

	public:
		GET_VALUE(ABTreePtr, ABTree);
		GET_VALUE(LooseOctreePtr, LooseOctree);

	private:
		virtual SceneNodePtr createSceneNode_impl(const Util::Wstring & name, Util::u_int nodeType);
		virtual void init_impl();
		virtual void preUpdate_impl(Util::time elapsedTime);
		virtual void postUpdate_impl(Util::time elapsedTime);
		virtual void destroySceneNode_impl(const Util::Wstring & name);
		virtual void destroyAllSceneNode_impl();

		void updateSceneGraph(Util::time elapsedTime);

		virtual void createDebugRendering();

	private:
		/// Just for the dynamic objects which have transform hierarchy ralationships with others.And only care about the position and orientation.
		SceneNodeVector mSceneGraphVec;
		/// For the dymamic objects which need to do dynamic spatial partition.DONNOT care about the position and orientation.
		ABTreePtr mABTree;
		/// For the static objects which only need to do static spatial partition.DONNOT care about the position and orientation.
		LooseOctreePtr mLooseOctree;
	};
}

#endif