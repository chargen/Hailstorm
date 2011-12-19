/*
 * Copyright (C) 2011 Scott MacDonald. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "stdafx.h"
#include "common/platform_windows.h"
#include "common/platform.h"
#include "common/delete.h"
#include "gui/window.h"
#include "gui/aboutbox.h"
#include "gui/assertiondialog.h"

#include "resource.h"

#include <cassert>

namespace
{
    /**
     * Converts an STL string into a WindowsNT wchar* wrapped inside of a
     * pretty wstring
     *
     * \param  str  The STL string you want to convert
     * \return A wstring representing the input
     */
    std::wstring WinNTStringToWideString( const std::string& str )
    {
        // Find the length of the soon to be allocated wstring
        size_t slen = str.length();
        int len     = MultiByteToWideChar( CP_ACP, 0,
                                            str.c_str(),
                                            static_cast<int>(slen) + 1,
                                            0,
                                            0 );

        // Allocate space for the new wstring, and then convert the input
        wchar_t *buffer = new wchar_t[len];

        MultiByteToWideChar( CP_ACP, 0,
                                str.c_str(),
                                static_cast<int>(slen) + 1,
                                buffer,
                                len );

        // Now create the wstring, copy the temporary wchar* buffer into it and
        // then destroy the buffer before returning
        std::wstring result( buffer );
        DeleteArray( buffer );

        return result;
    }
}

namespace App
{
    /**
     * Generates a assertion reporting dialog (or console output) to show to the
     * player, before exiting the application
     *
     * \param  message     An accompanying assertion description (if provided)
     * \param  expression  String containing the expression text
     * \param  filename    Name of the file that generated the assertion
     * \param  lineNumber  Line that generated the assertion
     */
    EAssertionStatus raiseAssertion( const char *pMessage,
                                     const char *pExpression,
                                     const char *pFilename,
                                     unsigned int lineNumber )
    {
        std::wstring expression = L"NULL";
        std::wstring filename   = L"NULL";

        // Convert the message and expression cstrings into STL wide strings
        //  (so long as the character pointers are not null)
        if ( pExpression != NULL )
        {
            expression = WinNTStringToWideString( pExpression );
        }
        
        if ( pFilename != NULL )
        {
            filename = WinNTStringToWideString( pFilename );
        }

        // Configure the assertion dialog before displaying it
        AssertionDialog dialog( expression, filename, lineNumber );
        
        if ( pMessage != NULL )
        {
            dialog.setMessage( WinNTStringToWideString( pMessage ) );
        }

        // Display the assertion to the user, and deal with their returned action
        if ( dialog.show() )
        {
            return EAssertion_Halt;
        }
        else
        {
            App::quit( EPROGRAM_ASSERT_FAILED,
                       "User quit after encountering assertion");
            return EAssertion_Continue;
        }
    }

    /**
     * Generates a non-fatal error message that is displayed to the player, and
     * the player is allowed to choose whether to continue or quit.
     *
     * \param  message  The main error message
     * \param  details  (optional) Details about the problem
     */
    void raiseError( const std::string& message,
                     const std::string& details )
    {

    }

    /**
     * Displays a fatal error message to the player before he/she is forced to
     * quit playing.
     *
     * \param  message  The main error message
     * \param  details  (optional) Details about the problem
     */
    void raiseFatalError( const std::string& message,
                          const std::string& details )
    {

    }

    /**
     * Performs windows specific tasks that need to happen before the game
     * starts up
     */
    void startup()
    {
        // Force SDL to use the direct driver
        _putenv("SDL_VideoDriver=directx");
    }

    /**
     * Quit the program with the requested status and reason
     */
    void quit( EProgramStatus programStatus, const std::string& message )
    {
        exit( programStatus );
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    Window * pWindow = NULL;

    //
    // Grab a pointer to the Window* instance that is sending this message.
    //  Either a window was created, in which case we need to save the pointer,
    //  or we need to look up the saved value
    //
    if ( message == WM_NCCREATE )
    {
        // We need to intercept the WM_NCCREATE message, since it is the first
        // message that a newly create window will send.  Once we get it, we will
        // grab the encoded Window* pointer and use SetWindowLong to save it for
        // future use
        pWindow = reinterpret_cast<Window*>( (LPCREATESTRUCT) lParam );
        assert( pWindow != NULL && "Failed to find window pointer");

        ::SetWindowLongPtr(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>( pWindow ) );
    }
    else
    {
        // Try to look up the pointer that is stored in the window's userdata
        // field
        pWindow =
            reinterpret_cast<Window*>( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
    }

    AssertionDialog ad( L"true == false", L"stupid.cpp", 4514u );
    

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            //assert( pWindow != NULL );
            ad.show();
  //          AboutBox aboutBox( pWindow->appInstance(), pWindow->windowHandle() );
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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