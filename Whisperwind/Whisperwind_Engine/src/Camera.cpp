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

#include <boost/make_shared.hpp>

#include "EngineManager.h"
#include "RenderSystem.h"
#include "Viewport.h"
#include "Frustum.h"
#include "Camera.h"

namespace Engine
{
	//---------------------------------------------------------------------
	Camera::Camera(Util::real nearCilp, Util::real farClip, const Util::UintPair & leftTop, const Util::UintPair & rightButtom) :
		mUpDirection(0.0f, 1.0f, 0.0f),
		mPitchRadians(0.0f),
		mYawRadians(0.0f),
		mMoveSpeed(1.0f),
		mNeedUpdateView(true)
	{
		mFrustum = boost::make_shared<Frustum>(nearCilp, farClip, 
			static_cast<Util::real>(rightButtom.first) - static_cast<Util::real>(leftTop.first), 
			static_cast<Util::real>(rightButtom.second) - static_cast<Util::real>(leftTop.second));

		mViewport = boost::make_shared<Viewport>(leftTop, rightButtom);

		XMStoreFloat4(&mOrientation, XMQuaternionIdentity());

		mIsMoveDirection[0] = mIsMoveDirection[1] = mIsMoveDirection[2] = mIsMoveDirection[3] = false;
	}
	//---------------------------------------------------------------------
	void Camera::move(Util::u_int moveDirection)
	{
		if (moveDirection & MD_FORWARD)
			mIsMoveDirection[0] = true;
		else if (moveDirection & MD_BACK)
			mIsMoveDirection[1] = true;
		else if (moveDirection & MD_LEFT)
			mIsMoveDirection[2] = true;
		else if (moveDirection & MD_RIGHT)
			mIsMoveDirection[3] = true;
	}
	//---------------------------------------------------------------------
	void Camera::stopMove(Util::u_int moveDirection)
	{
		if (moveDirection & MD_FORWARD)
			mIsMoveDirection[0] = false;
		else if (moveDirection & MD_BACK)
			mIsMoveDirection[1] = false;
		else if (moveDirection & MD_LEFT)
			mIsMoveDirection[2] = false;
		else if (moveDirection & MD_RIGHT)
			mIsMoveDirection[3] = false;
	}
	//---------------------------------------------------------------------
	void Camera::doMove(Util::time deltaTime)
	{
		if (!(mIsMoveDirection[0] || mIsMoveDirection[1] || mIsMoveDirection[2] || mIsMoveDirection[3]))
			return;

		XMFLOAT3 translationVec(0.0f, 0.0f, 0.0f);
		if (mIsMoveDirection[0])
			translationVec.z += mMoveSpeed;
		if (mIsMoveDirection[1])
			translationVec.z -= mMoveSpeed;
		if (mIsMoveDirection[2])
			translationVec.x -= mMoveSpeed;
		if (mIsMoveDirection[3])
			translationVec.x += mMoveSpeed;

		XMVECTOR vector = XMVector3Rotate(XMLoadFloat3(&translationVec), XMLoadFloat4(&mOrientation)) * deltaTime + XMLoadFloat3(&mPosition);
		XMVECTOR pos = XMLoadFloat3(&mPosition);
		XMVECTOR lookAt = vector - pos + XMLoadFloat3(&mLookAt);
		XMStoreFloat3(&mPosition, vector);
		XMStoreFloat3(&mLookAt, lookAt);

		mNeedUpdateView = true;
	}
	//---------------------------------------------------------------------
	void Camera::rotate(Util::real pitchAngle, Util::real yawAngle/*, Util::real zoom*/)
	{
		Util::real pitchRadians = XMConvertToRadians(pitchAngle * 0.1f);
		mPitchRadians += pitchRadians;
		mPitchRadians = XMMax(-XM_PI / 2.0f, mPitchRadians);
		mPitchRadians = XMMin(+XM_PI / 2.0f, mPitchRadians);

		Util::real yawRadians = XMConvertToRadians(yawAngle * 0.1f);
		mYawRadians += yawRadians;

		XMMATRIX rotMatrix;
		rotMatrix = XMMatrixRotationRollPitchYaw(mPitchRadians, mYawRadians, 0.0f);

		XMVECTOR lookAtVec;
		XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMStoreFloat3(&mUpDirection, XMVector3TransformCoord(upVec, rotMatrix));
		lookAtVec = XMVector3TransformCoord(XMLoadFloat3(&mPosLookDelta), rotMatrix);

		/// update lookat
		XMStoreFloat3(&mLookAt, XMLoadFloat3(&mPosition) + lookAtVec);

		/// update orientation
		XMStoreFloat4(&mOrientation, XMQuaternionRotationMatrix(rotMatrix));

		mNeedUpdateView = true;
	}
	//---------------------------------------------------------------------
	void Camera::lookAt(FXMVECTOR destVec)
	{
		XMStoreFloat3(&mLookAt, destVec);

		XMStoreFloat3(&mPosLookDelta, XMLoadFloat3(&mLookAt) - XMLoadFloat3(&mPosition));
	}
	//---------------------------------------------------------------------
	void Camera::update(Util::time elapsedTime)
	{
		doMove(elapsedTime);

		if (!mNeedUpdateView)
			return;

		mFrustum->setViewParams(XMLoadFloat3(&mPosition), XMLoadFloat3(&mLookAt), XMLoadFloat3(&mUpDirection));

		mNeedUpdateView = false;
	}

}