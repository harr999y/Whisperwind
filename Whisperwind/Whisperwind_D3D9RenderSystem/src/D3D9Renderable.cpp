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

#include "DebugDefine.h"
#include "EngineManager.h"
#include "RenderSystem.h"
#include "D3D9Renderable.h"
#include "D3D9Helper.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void D3D9Renderable::setEffectParamValue_impl(const Util::String & paramName, const void * data)
	{
		WHISPERWIND_ASSERT(data != NULL);

		EffectParamSize eps;
		if (mEffectParamMap.find(paramName) == mEffectParamMap.end())
		{
			eps.Handle = mEffect->GetParameterByName(NULL, paramName.c_str());
			WHISPERWIND_ASSERT(eps.Handle != NULL);

			D3DXPARAMETER_DESC paramDesc;
			DX_IF_FAILED_DEBUG_PRINT(mEffect->GetParameterDesc(eps.Handle, &paramDesc));
			eps.Size = paramDesc.Bytes;

			mEffectParamMap[paramName] = eps;
		}
		else
		{
			eps = mEffectParamMap[paramName];
		}
		WHISPERWIND_ASSERT((eps.Handle != 0) && (eps.Size != 0));

		DX_IF_FAILED_DEBUG_PRINT(mEffect->SetValue(eps.Handle, data, eps.Size));
	}
	//---------------------------------------------------------------------
	void D3D9Renderable::update_impl(Util::time /*elapsedTime*/)
	{
		EngineManager::getSingleton().getRenderSystem()->render(this->shared_from_this());
	}
}