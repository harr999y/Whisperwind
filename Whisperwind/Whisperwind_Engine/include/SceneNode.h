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

#include "Util.h"
#include "MathDefine.h"
#include "EngineForwardDeclare.h"

namespace Engine
{
	class WHISPERWIND_API SceneNode : public boost::enable_shared_from_this<SceneNode>
	{
	protected:
		SceneNode(const Util::Wstring & name, Util::u_int nodeType);

		virtual ~SceneNode();

	public:
		/// NOTO:This createChildNode only means they have spatial hierarchy ralationships.
		SceneNodePtr & createChildNode(const Util::Wstring & name);
		void removeChildNode(SceneNodePtr & childNode);
		void removeAllChildNode();
		bool getChildNode(const Util::Wstring & name, SceneNodePtr & outChildNode) const;
		bool getParentNode(SceneNodePtr & outParentNode) const;
		void setParentNode(const SceneNodePtr & parentNode);

		void update();
		void addToRenderQueue();

		XMVECTOR getPosition() const;
		void setPosition(FXMVECTOR position);
		XMVECTOR getRelativePosition() const;
		void setRelativePosition(FXMVECTOR relPosition);
		XMVECTOR getOrientation() const;
		void setOrientation(FXMVECTOR orientation);
		XMVECTOR getRelativeOrientation() const;
		void setRelativeOrientation(FXMVECTOR relOrientation);

		void attachSceneObject(SceneObjectPtr & sceneObj);
	    void dettachSceneObject(SceneObjectPtr & sceneObj);
		void dettachAllSceneObject();

		void setAABB(const Util::AABBPtr & aabb);

	public:
		GET_CONST_VALUE(Util::Wstring, Name);
		GET_CONST_VALUE(Util::u_int, NodeType);
		GET_CONST_VALUE(Util::AABBPtr, AABB);

	protected:
		void addChildNode(const SceneNodePtr & childNode);

	private:
		void setNeedUpdateChilds();
		
		void mergeAABBFromSceneObject(const SceneObjectPtr & so);
		void reCalcAABB();

		/// May needed for derived class.
		virtual void updatedAABB()
		{}

	protected:
		SceneObjectVector mSceneObjectVec;
		SceneNodeVector mChildSceneNodeVec;
		SceneNodePtr mParentNode;
		Util::Wstring mName;
		bool mNeedUpdateChilds;
		Util::u_int mNodeType;
		XMFLOAT3 mPosition;
		XMFLOAT3 mRelativePosition;
		XMFLOAT4 mOrientation;
		XMFLOAT4 mRelativeOrientation;
		Util::AABBPtr mAABB;

	private:
		DISALLOW_COPY_AND_ASSIGN(SceneNode);
	};
}

#endif