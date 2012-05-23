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
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MathDefine.h"
#include "EngineForwardDeclare.h"

namespace Engine
{
	enum MoveDirection
	{
		MD_LEFT = 1 << 1,
		MD_RIGHT = 1 << 2,
		MD_FORWARD = 1<<3,
		MD_BACK = 1 << 4
	};

	class WHISPERWIND_API Camera
	{
	public:
		Camera(Util::real nearCilp, Util::real farClip, Util::real width, Util::real height);

		~Camera()
		{}

	public:
		XMMATRIX getViewMatrix();
		XMMATRIX getProjMatrix() const;
		void move(Util::u_int moveDirection);
		void stopMove(Util::u_int moveDirection);
		void rotate(Util::real pitchAngle, Util::real yawAngle/*, Util::real zoom*/);
		void lookAt(FXMVECTOR destVec);
		void update(Util::time elapsedTime);

		void setPosition(FXMVECTOR pos) { XMStoreFloat3(&mPosition, pos); }
		XMVECTOR getPosition() { return XMLoadFloat3(&mPosition); }

	public:
		SET_VALUE(Util::real, MoveSpeed);
		SET_VALUE(Util::real, NearClip);
		SET_VALUE(Util::real, FarClip);

	private:
		void doMove(Util::time deltaTime);

	private:
		XMFLOAT3 mPosition;
		XMFLOAT3 mLookAt;
		XMFLOAT3 mUpDirection;
		XMFLOAT4 mOrientation;
		XMFLOAT3 mPosLookDelta;

		bool mIsMoveDirection[4]; /// The sequence is forward,back,left,right.
		Util::real mMoveSpeed;

		Util::real mNearClip;
		Util::real mFarClip;
		Util::real mAspect;

		XMFLOAT4X4 mProjMatrix;
		XMFLOAT4X4 mViewMatrix;
		bool mNeedUpdateViewMatrix;

		Util::real mPitchRadians;
		Util::real mYawRadians;

	private:
		DISALLOW_COPY_AND_ASSIGN(Camera);
	};
}

#endif