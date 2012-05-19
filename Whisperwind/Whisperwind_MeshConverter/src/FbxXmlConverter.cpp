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
#include "XmlManipulator.h"
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

			Util::Wstring wstrPath = Util::StringToWstring(mPath);
			boost::erase_last(wstrPath, ".FBX");
			boost::erase_last(wstrPath, ".fbx");
			wstrPath += XML_SUFFIX;
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

			Util::XmlNode * materialNode = mXmlWriter->appendNode(meshNode, "material");
			{
				/// material
				FbxNode * fbxNode = fbxMesh->GetNode();
				Util::u_int materialCount = fbxNode->GetMaterialCount();
				if (materialCount > 0)
				{
					FbxSurfaceMaterial * fbxMaterial = fbxNode->GetMaterial(0);
					if (fbxMaterial)
					{
						const FbxImplementation * implementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL);
						if (implementation)
						{
							const FbxBindingTable * rootTable = implementation->GetRootTable();
							if (rootTable)
							{
								FbxString effectFile = rootTable->DescAbsoluteURL.Get();
								Util::StringVector strVec;
								Util::String effectName(effectFile.Buffer());
								boost::algorithm::split(strVec, effectName, boost::is_any_of("\\"));
								mXmlWriter->appendAttribute(materialNode, "effect", strVec.rbegin()->c_str());

								FbxString techniqueName = rootTable->DescTAG.Get();
								mXmlWriter->appendAttribute(materialNode, "technique", techniqueName.Buffer());

								Util::u_int entryNum = rootTable->GetEntryCount();
								for (Util::u_int entryIt = 0; entryIt < entryNum; ++entryIt)
								{
									const FbxBindingTableEntry & entry = rootTable->GetEntry(entryIt);
									const char * entrySrcType = entry.GetEntryType(true);
									FbxProperty fbxProperty;

									if (strcmp(FbxPropertyEntryView::sEntryType, entrySrcType) == 0)
									{
										fbxProperty = fbxMaterial->FindPropertyHierarchical(entry.GetSource());
										if (!fbxProperty.IsValid())
										{
											fbxProperty = fbxMaterial->RootProperty.FindHierarchical(entry.GetSource());
										}
									}
									else if (strcmp(FbxConstantEntryView::sEntryType, entrySrcType) == 0)
									{
										fbxProperty = implementation->GetConstants().FindHierarchical(entry.GetSource());
									}

									if (fbxProperty.IsValid())
									{
										if (fbxProperty.GetSrcObjectCount(FBX_TYPE(FbxTexture)) > 0)
										{
											for (Util::s_int texIt = 0; texIt < fbxProperty.GetSrcObjectCount(FBX_TYPE(FbxFileTexture)); ++texIt)
											{
												FbxFileTexture * texture = fbxProperty.GetSrcObject(FBX_TYPE(FbxFileTexture), texIt);
												IF_NULL_CONTINUE(texture);

												Util::XmlNode * paramNode = mXmlWriter->appendNode(materialNode, "param");
												mXmlWriter->appendAttribute(paramNode, "name", texture->GetDstProperty().GetNameAsCStr());
												mXmlWriter->appendAttribute(paramNode, "value", texture->GetMediaName().Buffer());
											}
										}
									}
								}
							}
						}
					}
				}
			}

			Util::u_int vertexCount = fbxMesh->GetControlPointsCount();
			Util::XmlNode * vbNode = mXmlWriter->appendNode(meshNode, "vertexbuffer");
			mXmlWriter->appendAttribute(vbNode, "vertexcount", (boost::lexical_cast<Util::String>(vertexCount)).c_str());

			if (vertexCount > 0)
			{
				std::vector<bool> vertexHasUsedVec(vertexCount, false);

				Util::u_int polygonCount = fbxMesh->GetPolygonCount();

				FbxVector4 * vertexBuf = fbxMesh->GetControlPoints();
				IF_NULL_EXCEPTION(vertexBuf, mPath + " get vertex buffer failed!");

				/// TODO:Now I just consider it only have one texcoord!
				for (Util::u_int polygonIt = 0; polygonIt < polygonCount; ++polygonIt)
				{
					Util::s_int polygonSize = fbxMesh->GetPolygonSize(polygonIt);
					if (-1 == polygonSize)
						continue;

					for (Util::s_int polygonIndex = 0; polygonIndex < polygonSize; ++polygonIndex)
					{
						Util::s_int vertexIt = fbxMesh->GetPolygonVertex(polygonIt, polygonIndex);
						if ((-1 == vertexIt) || vertexHasUsedVec[vertexIt])
							continue;

						vertexHasUsedVec[vertexIt] = true;

						FbxGeometryElementUV * elemUV = fbxMesh->GetElementUV(0);
						IF_NULL_EXCEPTION(elemUV, mPath + " get uv error!");
						{
							Util::XmlNode * vertexNode = mXmlWriter->appendNode(vbNode, "vertex");
							Util::XmlNode * positionNode = mXmlWriter->appendNode(vertexNode, "position");
							mXmlWriter->appendAttribute(positionNode, "x", boost::lexical_cast<Util::String>(vertexBuf[vertexIt][0]).c_str());
							mXmlWriter->appendAttribute(positionNode, "y", boost::lexical_cast<Util::String>(vertexBuf[vertexIt][1]).c_str());
							mXmlWriter->appendAttribute(positionNode, "z", boost::lexical_cast<Util::String>(vertexBuf[vertexIt][2]).c_str());
							
							FbxVector2 uv;
							switch (elemUV->GetMappingMode())
							{
							case FbxGeometryElement::eByControlPoint:
								{
									switch (elemUV->GetReferenceMode())
									{
									case FbxGeometryElement::eDirect:
										{
											uv = elemUV->GetDirectArray().GetAt(vertexIt);
											break;
										}
									case FbxGeometryElement::eIndexToDirect:
										{
											Util::u_int id = elemUV->GetIndexArray().GetAt(vertexIt);
											uv = elemUV->GetDirectArray().GetAt(id);
											break;
										}
									}
									break;
								}
							case FbxGeometryElement::eByPolygonVertex:
								{
									Util::s_int uvIndex = fbxMesh->GetTextureUVIndex(polygonIt, polygonIndex);
									if (-1 == uvIndex)
										continue;

									switch (elemUV->GetReferenceMode())
									{
									case FbxGeometryElement::eDirect:
									case FbxGeometryElement::eIndexToDirect:
										{
											uv = elemUV->GetDirectArray().GetAt(uvIndex);
											break;
										}
									}

									break;
								}
							} // switch

							Util::XmlNode * uvNode = mXmlWriter->appendNode(vertexNode, "texcoord");
							mXmlWriter->appendAttribute(uvNode, "u", boost::lexical_cast<Util::String>(uv[0]).c_str());
							mXmlWriter->appendAttribute(uvNode, "v", boost::lexical_cast<Util::String>(uv[1]).c_str());
						}
					}
				}
			}
			
			Util::u_int trianglesCount = fbxMesh->GetPolygonCount();
			Util::XmlNode * trianglesNode = mXmlWriter->appendNode(meshNode, "triangles");
			mXmlWriter->appendAttribute(trianglesNode, "trianglescount", boost::lexical_cast<Util::String>(trianglesCount).c_str());

			if (trianglesCount > 0)
			{
				Util::u_int indexCount = fbxMesh->GetPolygonVertexCount();
				Util::s_int * indexBuf = fbxMesh->GetPolygonVertices();
				IF_NULL_EXCEPTION(indexBuf, mPath + " get index buffer failed!");
				for (Util::u_int it = 0; it < indexCount; /**/)
				{
					Util::XmlNode * triangleNode = mXmlWriter->appendNode(trianglesNode, "triangle");
					mXmlWriter->appendAttribute(triangleNode, "v1", boost::lexical_cast<Util::String>(indexBuf[it]).c_str());
					mXmlWriter->appendAttribute(triangleNode, "v2", boost::lexical_cast<Util::String>(indexBuf[it + 1]).c_str());
					mXmlWriter->appendAttribute(triangleNode, "v3", boost::lexical_cast<Util::String>(indexBuf[it + 2]).c_str());

					it += 3;
				}
			}
		}
	}

}