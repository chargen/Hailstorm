/*
 * Copyright 2011 - 2014 Scott MacDonald
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
#ifndef SCOTT_HAILSTORM_RENDERING_WINDOW_H
#define SCOTT_HAILSTORM_RENDERING_WINDOW_H

#include "runtime/Size.h"

/**
 * Basic rendering window interface. All clients must implement this interface.
 */
class RenderingWindow
{
public:
    virtual HWND WindowHandle() const = 0;
    virtual unsigned int Width() const = 0;
    virtual unsigned int Height() const = 0;
    virtual bool WasResized() const = 0;
    virtual bool IsResizing() const = 0;
    virtual bool IsPaused() const = 0;
    virtual bool IsMinimized() const = 0;
    virtual Size WindowSize() const = 0;
    virtual bool ProcessMessages() = 0;
    virtual bool IsClosing() const = 0;

    virtual void ClearResizedFlag() = 0;  // TODO: Is this really supposed to be here or in DXRenderer?
};

#endif