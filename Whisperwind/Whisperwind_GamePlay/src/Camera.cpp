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

#include "EngineManager.h"
#include "RenderSystem.h"
#include "Viewport.h"
#include "Camera.h"

namespace GamePlay
{
	static const Util::real MOVE_SPEED = 1.0f;
	//---------------------------------------------------------------------
	Camera::Camera(Util::real nearCilp, Util::real farClip) :
        mNearClip(nearCilp),
	    mFarClip(farClip),
		mUpDirection(0.0f, 1.0f, 0.0f),
		mNeedUpdateViewMatrix(true),
		mPitchRadians(0.0f),
		mYawRadians(0.0f)
	{
		mViewport = Engine::EngineManager::getSingleton().getRenderSystem()->getViewport();

		mAspect = static_cast<Util::real>(mViewport->getWidth()) / static_cast<Util::real>(mViewport->getHeight());

		XMStoreFloat4(&mOrientation, XMQuaternionIdentity());

		mIsMoveDirection[0] = mIsMoveDirection[1] = mIsMoveDirection[2] = mIsMoveDirection[3] = false;

		XMStoreFloat4x4(&mProjMatrix, XMMatrixPerspectiveFovLH(XM_PI / 2.0, mAspect, mNearClip, mFarClip));
	}
	//---------------------------------------------------------------------
	XMMATRIX Camera::getViewMatrix()
	{
		if (mNeedUpdateViewMatrix)
			XMStoreFloat4x4(&mViewMatrix, XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&mLookAt), XMLoadFloat3(&mUpDirection)));

		return XMLoadFloat4x4(&mViewMatrix);
	}
	//---------------------------------------------------------------------
	XMMATRIX Camera::getProjMatrix()
	{		
		return XMLoadFloat4x4(&mProjMatrix);
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
			translationVec.z += MOVE_SPEED;
		if (mIsMoveDirection[1])
			translationVec.z -= MOVE_SPEED;
		if (mIsMoveDirection[2])
			translationVec.x -= MOVE_SPEED;
		if (mIsMoveDirection[3])
			translationVec.x += MOVE_SPEED;

		XMVECTOR vector = XMVector3Rotate(XMLoadFloat3(&translationVec), XMLoadFloat4(&mOrientation)) * deltaTime + XMLoadFloat3(&mPosition);
		XMVECTOR pos = XMLoadFloat3(&mPosition);
		XMVECTOR lookAt = vector - pos + XMLoadFloat3(&mLookAt);
		XMStoreFloat3(&mPosition, vector);
		XMStoreFloat3(&mLookAt, lookAt);
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

		mNeedUpdateViewMatrix = true;
	}
	//---------------------------------------------------------------------
	void Camera::lookAt(FXMVECTOR destVec)
	{
		XMStoreFloat3(&mLookAt, destVec);

		XMStoreFloat3(&mPosLookDelta, XMLoadFloat3(&mLookAt) - XMLoadFloat3(&mPosition));

		mNeedUpdateViewMatrix = true;
	}
	//---------------------------------------------------------------------
	void Camera::update(Util::time elapsedTime)
	{
		doMove(elapsedTime);
	}
	//---------------------------------------------------------------------
	Util::u_int Camera::getKeyCombinationFromEvent(const OIS::KeyEvent & arg)
	{
		Util::u_int flag = 0;
		if (OIS::KC_W == arg.key)
			flag |= MD_FORWARD;
		else if (OIS::KC_S == arg.key)
			flag |= MD_BACK;
		else if (OIS::KC_A == arg.key)
			flag |= MD_LEFT;
		else if (OIS::KC_D == arg.key)
			flag |= MD_RIGHT;

		return flag;
	}

}