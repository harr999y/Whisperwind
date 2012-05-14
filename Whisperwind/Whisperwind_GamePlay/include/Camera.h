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

namespace GamePlay
{
	class Camera
	{
	public:
		Camera(Util::real nearCilp, Util::real farClip);

		~Camera()
		{}

	public:
		XMMATRIX getViewMatrix();
		XMMATRIX getProjMatrix();

	public:
		SET_GET_CONST_VALUE(XMFLOAT3, Position);
		SET_GET_CONST_VALUE(XMFLOAT3, LookAt);
		SET_GET_CONST_VALUE(XMFLOAT3, UpDirection);
		SET_GET_CONST_VALUE(XMFLOAT4, Orientation);

	private:
		XMFLOAT3 mPosition;
		XMFLOAT3 mLookAt;
		XMFLOAT3 mUpDirection;
		XMFLOAT4 mOrientation;

		Util::real mNearClip;
		Util::real mFarClip;
		Util::real mAspect;

		Engine::ViewportPtr mViewport;
	};
}

#endif