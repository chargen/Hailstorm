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
#include "stdafx.h"
#include "gui/aboutbox.h"
#include "common/platform_windows.h"
#include "resource.h"

#include <cassert>

AboutBox::AboutBox( HINSTANCE hInstance, HWND hWnd )
    : mAppInstance( hInstance ),
      mParentWindow( hWnd )
{
}

AboutBox::~AboutBox()
{
}

void AboutBox::show()
{
    DialogBox( mAppInstance,
               MAKEINTRESOURCE(IDD_ABOUTBOX),
               mParentWindow,
               DlgWndProc );
}

// Message handler for about box.
INT_PTR CALLBACK DlgWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}