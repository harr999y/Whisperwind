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

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "ExceptionDefine.h"
#include "XmlWriter.h"
#include "FbxXmlConverter.h"

namespace Tool
{
	static const Util::Wstring XML_SUFFIX(TO_UNICODE(".wmesh.xml"));
	//---------------------------------------------------------------------
	FbxXmlConverter::FbxXmlConverter(const Util::String & filePath)  :
        mPath(filePath)
	{
		mFbxManager = FbxManager::Create();
		IF_NULL_EXCEPTION(mFbxManager, (filePath + " FbxManager create failed!").c_str());

		FbxIOSettings * fbxIos = FbxIOSettings::Create(mFbxManager, IOSROOT);
		IF_NULL_EXCEPTION(fbxIos, (filePath + " FbxIOSettings create failed!").c_str());
		mFbxManager->SetIOSettings(fbxIos);

		FbxString path = FbxGetApplicationDirectory();
		mFbxManager->LoadPluginsDirectory(path.Buffer());

		mFbxScene = FbxScene::Create(mFbxManager, "");
		IF_NULL_EXCEPTION(mFbxScene, (filePath + " FbxScene create failed!").c_str());

		FbxImporter * sceneImporter = FbxImporter::Create(mFbxManager, "");
		IF_NULL_EXCEPTION(sceneImporter, (filePath + " FbxImporter create failed!").c_str());
		IF_FALSE_EXCEPTION(sceneImporter->Initialize(filePath.c_str(), -1, fbxIos), (filePath + " parse error!").c_str());
		sceneImporter->Import(mFbxScene);
		sceneImporter->Destroy();
	}
		//---------------------------------------------------------------------
	FbxXmlConverter::~FbxXmlConverter()
	{
		if (mFbxManager)
			mFbxManager->Destroy();
	}
	//---------------------------------------------------------------------
	Util::Wstring FbxXmlConverter::convertToXml()
	{
		/// save to xml
		try
		{
			mXmlWriter = boost::make_shared<Util::XmlWriter>();

			FbxNode * rootNode = mFbxScene->GetRootNode();
			IF_NULL_EXCEPTION(rootNode, (mPath + " Get root node failed!").c_str());
			for (int it = 0; it < rootNode->GetChildCount(); ++it)
			{
				doWalk(rootNode->GetChild(it));
			}

			Util::Wstring wstrPath;
			Util::StringToWstring(mPath, wstrPath);
			Util::WstringVector wstrVec;
			boost::split(wstrVec, wstrPath, boost::is_any_of("."));
			wstrPath = wstrVec[0] + XML_SUFFIX;
			mXmlWriter->writeToFile(wstrPath);

			return wstrPath;
		}
		catch (Util::Exception & /*e*/)
		{
			WHISPERWIND_EXCEPTION((mPath + " write to xml failed!"));
		}
	}
	//---------------------------------------------------------------------
	void FbxXmlConverter::doWalk(FbxNode * fbxNode)
	{
		IF_NULL_RETURN(fbxNode);

		FbxNodeAttribute * nodeAtribute = fbxNode->GetNodeAttribute();
		IF_NULL_RETURN(nodeAtribute);

		switch (nodeAtribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			{
				processMesh(fbxNode);
				break;
			}
		}

		for (int it = 0; it < fbxNode->GetChildCount(); ++it)
		{
			doWalk(fbxNode);
		}
	}
	//---------------------------------------------------------------------
	void FbxXmlConverter::processMesh(FbxNode * fbxNode)
	{
		IF_NULL_RETURN(fbxNode);

		FbxMesh * fbxMesh = static_cast<FbxMesh *>(fbxNode->GetNodeAttribute());

		if (!fbxMesh->IsTriangleMesh())
		{
			FbxGeometryConverter converter(mFbxManager);
			converter.TriangulateInPlace(fbxNode);
			fbxMesh = static_cast<FbxMesh *>(fbxNode->GetNodeAttribute());
		}

		/// save
		{
			Util::XmlNode * meshNode = mXmlWriter->appendNode(mXmlWriter->getRootNode(), "mesh");

			mXmlWriter->appendNode(meshNode, "material");
			mXmlWriter->appendNode(meshNode, "vertexbuffer");

		}
	}

}