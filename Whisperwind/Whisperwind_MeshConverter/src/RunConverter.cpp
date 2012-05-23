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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "Util.h"
#include "ExceptionDefine.h"
#include "DebugDefine.h"
#include "StringConverter.h"
#include "FbxXmlConverter.h"
#include "XmlWmeshConverter.h"

int main(int argc, char ** argv)
{
	try
	{
		if (argc > 3)
		{
			WHISPERWIND_EXCEPTION("Too many args!");
		}
		else if (argc <= 1)
		{
			WHISPERWIND_EXCEPTION("You should assign a file path,the best way is dragging the file to the exe or write a bat.");
		}

		if (boost::algorithm::find_first(argv[1], ".fbx") || boost::algorithm::find_first(argv[1], ".FBX"))
		{
			Tool::FbxXmlConverter fbx2XmlConverter(argv[1]);
			if ((argc == 3) && (boost::algorithm::equals(argv[2], "uv_inverse")))
				fbx2XmlConverter.setUVInverse(true);
			Util::Wstring xmlPath = fbx2XmlConverter.convertToXml();

			Tool::XmlWmeshConverter xml2WmeshConverter(Util::WstringToString(xmlPath));
			xml2WmeshConverter.converteToWmesh();
		}
		else if (boost::algorithm::find_first(argv[1], ".wmesh.xml"))
		{
			Tool::XmlWmeshConverter xml2WmeshConverter(argv[1]);
			xml2WmeshConverter.converteToWmesh();
		}
		else if (boost::algorithm::find_first(argv[1], ".wmesh"))
		{
			/// TODO! .wmesh converte to .wmesh.xml. Now I donnot need it.
		}
	}
	catch (boost::exception & e)
	{
#ifdef WHISPERWIND_DEBUG
		DEBUG_PRINT_RED(boost::diagnostic_information_what(e));
#else
		WHISPERWIND_LOG(Util::StringToWstring(boost::diagnostic_information_what(e)));
		::MessageBox(NULL, TO_UNICODE("Xml to wmesh failed!"), TO_UNICODE("Error!"), MB_OK);		
#endif
	}

	return 0;
}