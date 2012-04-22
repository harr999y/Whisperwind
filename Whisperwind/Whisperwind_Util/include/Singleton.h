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
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/** Finally I comment it,because the problem when using multi-dll.
      There are many solutions to solve it,but now,as I only use it in EngineManager,
	  I donot think now I have the motion to fix it.Well,maybe someday I'll fix it.
@todo:
     Fix it,make it surpported multi-dll.
*/ 
// #include "Util.h"
// 
// namespace Util
// {
// 	template <class T> class Singleton
// 	{
// 	public:
// 		~Singleton() {}
// 	protected:
// 		Singleton() {}
// 	private:
// 		DISALLOW_COPY_AND_ASSIGN(Singleton);
// 
// 	public:
// 		static T * getSingletonPtr() 
// 		{
// 			static T instance;
// 			return &instance;
// 		}
// 		static T & getSingleton() 
// 		{
// 			return *getSingletonPtr();
// 		}
// 	};
// }

#endif