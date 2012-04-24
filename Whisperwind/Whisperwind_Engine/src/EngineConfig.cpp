/*-------------------------------------------------------------------------
This source file is a part of Whisperwind(GameEngine + GamePlay + GameTools)
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

#include "EngineConfig.h"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "EngineConfigDefines.h"
#include "XmlReader.h"

namespace Engine
{
	static const Util::String CONFIG_VALUE("value");
	//---------------------------------------------------------------------
	bool EngineConfig::parse_impl()
	{
		try
		{
			Util::String valueStr;

			IF_FALSE_RETURN_FALSE(mXmlReader->advanceFirstChildNode(FULL_SCREEN));
			valueStr = mXmlReader->getAttribute(CONFIG_VALUE);
			mFullScreen = boost::lexical_cast<bool>(valueStr);

			IF_FALSE_RETURN_FALSE(mXmlReader->advanceFirstChildNode(RESOLUTION));
			valueStr = mXmlReader->getAttribute(CONFIG_VALUE);
			Util::StringVector strVec;
			boost::split(strVec, valueStr, boost::is_any_of("*"));

			mResolutionPair.first = boost::lexical_cast<Util::u_int>(strVec[0]);
			mResolutionPair.second = boost::lexical_cast<Util::u_int>(strVec[1]);
		}
		catch(std::exception &)
		{
			return false;
		}

		return true;
	}
}