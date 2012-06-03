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

#include "MathDefine.h"
#include "AABB.h"
#include "SceneNode.h"
#include "Light.h"
#include "Renderable.h"

namespace Engine
{
	//---------------------------------------------------------------------
	void Renderable::setEffectSemanticValue( const Util::String & semanticName, const void * data )
	{
		setEffectSemanticValue_impl(semanticName, data);
	}
	//---------------------------------------------------------------------
	void Renderable::setEffectParamValue(const Util::String & paramName, const void * data)
	{
		setEffectParamValue_impl(paramName, data);
	}
	//---------------------------------------------------------------------
	void Renderable::setTexture(const Util::String & paramName, const RenderTexturePtr & texture)
	{
		setTexture_impl(paramName, texture);
	}
	//---------------------------------------------------------------------
	void Renderable::preRender(Util::time /*elapsedTime*/)
	{
		this->setEffectSemanticValue("WorldViewProj", static_cast<void *>(&mWorldViewProj));
		this->setEffectSemanticValue("World", static_cast<void *>(&mWorldMatrix));
	}
	//---------------------------------------------------------------------
	void Renderable::postRender(Util::time /*elapsedTime*/)
	{}

}