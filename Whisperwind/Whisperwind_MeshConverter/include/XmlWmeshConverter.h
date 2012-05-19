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
#ifndef _XML_WMESH_CONVERTER_H_
#define _XML_WMESH_CONVERTER_H_

#include <boost/lexical_cast.hpp>

#include "Util.h"
#include "XmlManipulator.h"
#include "RenderMappingDefines.h"

namespace Tool
{
	class XmlWmeshConverter
	{
	public:
		explicit XmlWmeshConverter(const Util::String & path);

		~XmlWmeshConverter()
		{}

		void converteToWmesh();

	private:
		Util::XmlReaderPtr mXmlReader;
		Util::String mPath;

	private:
		DISALLOW_COPY_AND_ASSIGN(XmlWmeshConverter);

	private:
		template <typename type>
		Engine::IndexMapping getIndexMapping(const Util::XmlNode * trianglesNode, Util::u_int indexCount)
		{
			Engine::IndexMapping indexBound;

			indexBound.HasIndex = true;

			Engine::Uint8Vector dataVec((sizeof(type) / sizeof(Engine::Uint8Vector::value_type)) * indexCount);
			type * data = reinterpret_cast<type *>(dataVec.data());

			Util::XmlNode * triNode = mXmlReader->getFirstNode(trianglesNode, "triangle");
			if (triNode)
			{
				do
				{
					*(data++) = boost::lexical_cast<type>(mXmlReader->getAttribute(triNode, "v1"));
					*(data++) = boost::lexical_cast<type>(mXmlReader->getAttribute(triNode, "v2"));
					*(data++) = boost::lexical_cast<type>(mXmlReader->getAttribute(triNode, "v3"));

					triNode = mXmlReader->getNextSiblingNode(triNode);
				} while (triNode);
			}

			indexBound.IndexData.DataVec = dataVec;
			indexBound.IndexData.DataSize = sizeof(type) * indexCount;
			indexBound.IndexData.Stride = sizeof(type);

			return indexBound;
		}
	};

}

#endif