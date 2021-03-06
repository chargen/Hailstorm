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
#ifndef SCOTT_HAILSTORM_RUNTIME_INITIALIZABLE
#define SCOTT_HAILSTORM_RUNTIME_INITIALIZABLE

#include "runtime\exceptions.h"
#include <string>

// Runner up name: ICanHazInitialization
// TODO: Document me.
class Initializable
{
public:
    Initializable();
    virtual ~Initializable();

protected:
    void SetIsInitialized();
    bool IsInitialized() const;

private:
    bool mIsInitialized;
};


// Exception that is thrown when a method is called on an non-initialized instance.
class NotInitializedException : public HailstormException
{
public:
    NotInitializedException(
        const std::wstring& className,
        const char * fileName,
        unsigned int lineNumber);
};

#endif