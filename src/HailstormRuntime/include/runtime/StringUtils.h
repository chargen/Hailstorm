/*
 * Copyright 2014 Scott MacDonald
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
#ifndef SCOTT_STRING_UTILS_H
#define SCOTT_STRING_UTILS_H

#include <string>

namespace Utils
{
    std::wstring ConvertUtf8ToWideString(const std::string& text);
    std::string ConvertWideStringToUtf8(const std::wstring& text);
    std::wstring GetWinApiErrorText(unsigned long errorCode);
    std::wstring GetHResultErrorText(unsigned long errorCode);
}

#endif