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
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Util.h"
#include "MathDefine.h"
#include "SceneObject.h"

namespace Engine
{
	enum LightType
	{
		LT_DIRECTIONAL,
		LT_POINT,
		LT_SPOT
	};

	struct LightInfo
	{
		LightInfo() :
	        Type(LT_DIRECTIONAL),
			Color(0.0f, 0.0f, 0.0f),
			Direction(0.0f, -1.0f, 0.0f),
			EffectDistance(0.0f)
		{}

		LightType Type;
		XMFLOAT3 Color;
		XMFLOAT3 Direction;
		Util::real EffectDistance;
	};

	class WHISPERWIND_API Light : public SceneObject
	{
	protected:
		Light(LightType lightType, const Util::Wstring & name);

		virtual ~Light()
		{}

	public:
		inline bool isAffected(const Util::AABBPtr & aabb);
		inline void affectRenderable(const RenderablePtr & renderable);

	public:
		GET_CONST_VALUE(LightType, Type);
		SET_GET_CONST_VALUE(XMFLOAT3, Color);

	private:
		virtual void preUpdate_impl(Util::time /*elapsedTime*/)
		{}
		virtual void postUpdate_impl(Util::time /*elapsedTime*/)
		{}

		virtual bool isAffected_impl(const Util::AABBPtr & aabb) = 0;
		virtual void affectRenderable_impl(const RenderablePtr & renderable) = 0;

	protected:
		LightType mType;
		XMFLOAT3 mColor;

	private:
		DISALLOW_COPY_AND_ASSIGN(Light);
	};

	class DirectionalLight : public Light
	{
	public:
		explicit DirectionalLight(const Util::Wstring & name) :
	        Light(LT_DIRECTIONAL, name)
		{}

		~DirectionalLight()
		{}

	public:
		SET_GET_CONST_VALUE(XMFLOAT3, Direction);

	private:
		virtual bool isAffected_impl(const Util::AABBPtr & aabb);
		virtual void affectRenderable_impl(const RenderablePtr & renderable);

	private:
		XMFLOAT3 mDirection;
	};

	class PointLight : public Light
	{
	public:
		explicit PointLight(const Util::Wstring & name) :
		  Light(LT_POINT, name)
		{}

		~PointLight()
		{}

	public:
		SET_GET_CONST_VALUE(Util::real, EffectDistance);

	private:
		virtual bool isAffected_impl(const Util::AABBPtr & aabb);
		virtual void affectRenderable_impl(const RenderablePtr & renderable);

	private:
		Util::real mEffectDistance;
	};

}

#endif