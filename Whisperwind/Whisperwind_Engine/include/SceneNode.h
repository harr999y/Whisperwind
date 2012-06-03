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

#include <utility>
#include <boost/enable_shared_from_this.hpp>

#include "Util.h"
#include "MathDefine.h"
#include "EngineForwardDeclare.h"

namespace Engine
{
	enum NodeType
	{
		NT_STATIC = 1 << 0,
		NT_DYNAMIC = 1 << 1,
		NT_AS_CHILD = 1 << 2
	};

	enum NodeTrackMode
	{
		NTM_AS_PARENT,
		NTM_AS_WORLD
	};

	struct NodeControllPoint
	{
		NodeControllPoint() :
	        Position(0.0f, 0.0f, 0.0f),
			Orientation(0.0f, 0.0f, 0.0f, 1.0f)
		{}

		XMFLOAT3 Position;
		XMFLOAT4 Orientation;
	};

	struct NodeTrack
	{
		NodeTrack() :
	        MoveSpeed(0.0f),
			RotateSpeed(0.0f),
			CurrentPoint(-1),
			TrackMode(NTM_AS_WORLD),
			RestTrans(0.0f),
			CurrentDirection(0.0f, 0.0f, 0.0f)
		{}

		bool isEmpty() { return ControllPointVec.empty(); }

		NodeTrackMode TrackMode;
		Util::real MoveSpeed;
		Util::real RotateSpeed;
		NodeControllPointVector ControllPointVec;
		Util::s_int CurrentPoint;
		Util::real RestTrans;
		XMFLOAT3 CurrentDirection;
	};

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

		void update(Util::time elapsedTime);
		void addToRenderQueue();

		XMVECTOR getPosition() const;
		void setPosition(FXMVECTOR position);
		XMVECTOR getRelativePosition() const;
		void setRelativePosition(FXMVECTOR relPosition);
		XMVECTOR getOrientation() const;
		void setOrientation(FXMVECTOR orientation);
		XMVECTOR getRelativeOrientation() const;
		void setRelativeOrientation(FXMVECTOR relOrientation);

		void move(FXMVECTOR trans);
		void rotate(FXMVECTOR orientation);

		void attachSceneObject(SceneObjectPtr & sceneObj);
	    void dettachSceneObject(SceneObjectPtr & sceneObj);
		void dettachAllSceneObject();

		void setAABB(const Util::AABBPtr & aabb);

		void addTrackPoint(const NodeControllPoint & point);

	public:
		GET_CONST_VALUE(Util::Wstring, Name);
		GET_CONST_VALUE(Util::u_int, NodeType);
		GET_CONST_VALUE(Util::AABBPtr, AABB);
		GET_VALUE(NodeTrack, NodeTrack);

	protected:
		void addChildNode(const SceneNodePtr & childNode);

	private:
		void setNeedUpdateChilds();

		void updateNodeTrack(Util::time elapsedTime);
		
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
		NodeTrack mNodeTrack;

	private:
		DISALLOW_COPY_AND_ASSIGN(SceneNode);
	};
}

#endif