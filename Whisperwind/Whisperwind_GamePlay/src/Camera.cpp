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
	//---------------------------------------------------------------------
	Camera::Camera(Util::real nearCilp, Util::real farClip) :
        mNearClip(nearCilp),
	    mFarClip(farClip)
	{
		mViewport = Engine::EngineManager::getSingleton().getRenderSystem()->getViewport();

		mAspect = static_cast<Util::real>(mViewport->getWidth()) / static_cast<Util::real>(mViewport->getHeight());
	}
	//---------------------------------------------------------------------
	XMMATRIX Camera::getViewMatrix()
	{
		return XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&mLookAt), XMLoadFloat3(&mUpDirection));
	}
	//---------------------------------------------------------------------
	XMMATRIX Camera::getProjMatrix()
	{		
		return XMMatrixPerspectiveFovLH(XM_PI / 2.0, mAspect, mNearClip, mFarClip);
	}

}