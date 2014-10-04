/*
 * Copyright 2011 Scott MacDonald
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SCOTT_HAILSTORM_GRAPHICS_DXUTILS_H
#define SCOTT_HAILSTORM_GRAPHICS_DXUTILS_H

#include <string>

/**
 * Helper method that safely releases a directx COM object
 */
template< class T >
void SafeRelease( T** pObj )
{
    if ( *pObj != NULL )
    {
        (*pObj)->Release();
        (*pObj) = NULL;
    }
}

/**
 * Method that checks DirectX return values and deals with error conditions
 * in a way that is helpful to programmers and the end user
 */
namespace DxUtils
{
    bool CheckResult( HRESULT result, bool shouldExit, const std::string& action );
}

#endif