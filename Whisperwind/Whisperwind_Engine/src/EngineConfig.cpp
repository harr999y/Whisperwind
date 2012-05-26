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

#include "UtilWarningDisable.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "ExceptionDefine.h"
#include "XmlManipulator.h"
#include "EngineConfigDefines.h"
#include "EngineConfig.h"

namespace Engine
{
	static const Util::String CONFIG_VALUE("value");
	//---------------------------------------------------------------------
	EngineConfig::EngineConfig(const Util::String & name) : 
	    Config(name),
		mFullScreen(false),
		mVSync(false),
		mPerfHUD(false)
	{}
	//---------------------------------------------------------------------
	void  EngineConfig::parse_impl()
	{
		Util::XmlNode * rootNode = mXmlReader->getRootNode();
		IF_NULL_EXCEPTION(rootNode, "Engine config donnot have root!");

		Util::XmlNode * node = NULL;
		Util::String valueStr;

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::FULL_SCREEN);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::FULL_SCREEN + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		setFullScreen(boost::lexical_cast<bool>(valueStr));

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::RESOLUTION);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::RESOLUTION + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		Util::StringVector strVec;
		boost::split(strVec, valueStr, boost::is_any_of("*"));
		Util::UintPair resPair;
		resPair = std::make_pair(boost::lexical_cast<Util::u_int>(strVec[0]), boost::lexical_cast<Util::u_int>(strVec[1]));
		setResolutionPair(resPair);

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::MULTI_SAMPLE_QUALITY);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::MULTI_SAMPLE_QUALITY + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		setMultiSampleQuality(boost::lexical_cast<Util::u_int>(valueStr));

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::MULTI_SAMPLE_TYPE);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::MULTI_SAMPLE_TYPE + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		setMultiSampleType(boost::lexical_cast<Util::s_int>(valueStr));

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::VSYNC);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::VSYNC + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		setVSync(boost::lexical_cast<bool>(valueStr));

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::PERF_HUD);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::PERF_HUD + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		setPerfHUD(boost::lexical_cast<bool>(valueStr));

		node = mXmlReader->getFirstNode(rootNode, EngineConfigDefine::DEBUG_RENDERING);
		IF_FALSE_EXCEPTION(node, "Engine config " + EngineConfigDefine::DEBUG_RENDERING + " parse failed!");
		valueStr = mXmlReader->getAttribute(node, CONFIG_VALUE);
		setDebugRendering(boost::lexical_cast<bool>(valueStr));
	}
}