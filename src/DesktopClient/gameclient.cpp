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
#include "gameclient.h"
#include "host/RenderingWindow.h"
#include "graphics/dxrenderer.h"
#include "graphics/DemoScene.h"
#include "camera/Camera.h"

#include "runtime/gametime.h"
#include "runtime/logging.h"
#include "runtime/exceptions.h"
#include "runtime/debugging.h"

#include <Winnt.h>

/**
 * Inspiration and help for the game loop came from the following sources:
 *
 * Gaffer: "Fix Your Timestep!"
 * http://gafferongames.com/game-physics/fix-your-timestep/
 *
 * Stack Exchange "Fixed time step vs variable time step"
 * http://gamedev.stackexchange.com/questions/1589/fixed-time-step-vs-variable-time-step
 *
 * "Understanding GameTime"
 * http://blogs.msdn.com/b/shawnhar/archive/2007/07/25/understanding-gametime.aspx
 *
 * Information about the granularity and precision of the Windows Timers
 * came from the following sources:
 * "http://www.geisswerks.com/ryan/FAQS/timing.html"
 */

/**
 * Game client constructor
 */
GameClient::GameClient(
    std::shared_ptr<Camera> camera,
    std::shared_ptr<RenderingWindow> window,
    std::unique_ptr<DXRenderer> renderer)
    : mWindow(window),
      mRenderer(std::move(renderer)),
      mCamera(camera),
      mDemoScene(),
      mIsGameRunning(false),
      mIsRunningSlowly(false),
      mTimerFrequency(0.0f),
      mUpdateFrequency(1.0f / 50.0f), // 20ms, 50 times per second
      mMaximumSleepSkew(0.01f)        // 10ms
{
}

/**
 * Game client destructor
 */
GameClient::~GameClient()
{
}

/**
 * Starts up and runs the game. This method will not return until after the
 * player has quit the game
 */
void GameClient::Run(DemoScene * pDemoScene)
{
    VerifyNotNull(pDemoScene);
    mDemoScene.reset(pDemoScene);

    // Let the game initialize core systems.
    InitializeClient();
    Initialize();

    // Now load resources before entering the main game loop.
    LoadContent();

    // Enter the game
    RunMainGameLoop();

    // Make sure we unload all of our game's resources before the game client exits.
    UnloadContent();
}

/**
 * Core game loop logic
 */
void GameClient::RunMainGameLoop()
{
    LOG_INFO("GameClient") << "Entering the main game loop";

    assert(mTimerFrequency > 0.0f);
    mIsGameRunning = true;

    // Start simulation time tracking.
    TimeT simulationTime = 0.0f;   // t
    TimeT systemTime     = GetCurrentTime();   // currentTime
    TimeT accumulatedTime = 0.0f;

    // This is where it all starts!
    while (mIsGameRunning && (!mWindow->IsClosing()))
    {
        // Make sure we process ALL THE MESSAGES (before doing any useful simulation stuffs).
        mWindow->ProcessMessages();

        // Get the current system time, and then calculate how much time has elapsed since the last graphics update
        // (which we will call frameTime).
        TimeT newTime = GetCurrentTime();
        TimeT frameTime = newTime - systemTime;

        systemTime = newTime;

        // Check the amount of time that we have spent since the last loop iteration. If the value exceeds a threshold,
        // assume that we are in danger of hitting the "spiral of death" from a slow simulator. To avoid this, limit
        // the maximum frame time to a more reasonable value
        if (frameTime > 0.25f)
        {
            frameTime = 0.25f;
        }

        // Update the simulation. If the simulation is running too far behind the system time we will need to run the
        // simulation multiple times until it is caught up
        size_t numUpdates = 0;
        accumulatedTime += frameTime;

        while (accumulatedTime >= mUpdateFrequency)
        {
            // Is the simulation running multiple times?
            //  TODO: What's the best way to tell if simulation is slow or gfx?
            mIsRunningSlowly = (numUpdates > 0);

            // Update the simulation
            Update(simulationTime, mUpdateFrequency);

            // The simulation has been updated, and we need to increment our time tracking variables before the next
            // (possible) loop iteration.
            numUpdates += 1;

            simulationTime += mUpdateFrequency;
            accumulatedTime -= mUpdateFrequency;
        }

        // Calculate the amount of interpolation that will our renderer will need to account for when rendering between
        // the last simulation update and the next upcoming update
        double interpolation = 1.0f - (accumulatedTime / mUpdateFrequency);

        // Now draw the next frame
        Draw(simulationTime, interpolation);
        mRenderer->Update(*mDemoScene.get(), simulationTime, frameTime);

        // If there is a large delta between the system time and the time before
        // the next simulation update, we can afford to sleep a tiny bit and
        // allow windows some breathing room. (This can be tweaked or disabled)
        if (accumulatedTime + mMaximumSleepSkew < mUpdateFrequency)
        {
            Sleep(2);
        }
    }

    LOG_NOTICE("GameClient") << "Game has left the main game loop";
    return;
}

/**
 * Code that initializes the game client and all required game subsystems
 * such as rendering, resource management and sound.
 *
 * \return  True if all components were initialized successfully, false otherwies
 */
void GameClient::InitializeClient()
{
    // We need to find the internal tick rate before using time.
    mTimerFrequency = CalculateSystemTimerFrequency();
    mRenderer->Initialize();
}

/**
 * Initializes the game client in preparation for the running the game. This method
 * should be overridden by an implementing class, and should load any classes or
 * systems that are required for first play.
 *
 * \return  True if all components were initialized successfully, false otherwise
 */
void GameClient::Initialize()
{
    LOG_NOTICE("GameClient") << "Initializing primary demo scene";
    DXRenderer *pRenderer = mRenderer.get();
    mDemoScene->Initialize(*pRenderer);
}

/**
 * Loads content required for the game to function
 */
void GameClient::LoadContent()
{
    LOG_NOTICE("GameClient") << "Loading the game's art assets";
    DXRenderer *pRenderer = mRenderer.get();
    mDemoScene->LoadContent(*pRenderer);
}

void GameClient::UnloadContent()
{
    LOG_NOTICE("GameClient") << "Unloading the game's art assets";
    DXRenderer *pRenderer = mRenderer.get();
    mDemoScene->UnloadContent(*pRenderer);
}

/**
 * This is an override able method that is called by the main game loop and is
 * supposed to be advance the simulation by deltaTime.
 *
 * The simulation time is divorced from the system time, and is stepped by a
 * constant amount every call. It may be called multiple times in a row to ensure
 * that the simulation is properly caught up.
 *
 * \param  simlulationTime  The current simulation time
 * \param  deltaTime        Amount of time that has elapsed since the last call
 *                          to this method (Always the same amount)
 */
void GameClient::Update(TimeT simulationTime, TimeT deltaTime)
{
    mCamera->Update(simulationTime, deltaTime);
    mDemoScene->Update(simulationTime, deltaTime);
}

/**
 * Draw is called by the main game loop to render the simulation display. It
 * should be overridden by the implementing class to implement game specific drawing
 * code, seeing as the default implementation does nothing. :)
 *
 * \param  simulationTime  The current simulation time
 * \param  deltaTime       Amount of time since the last simulation
 * \param  interpolation   Amount to interpolate between (1.0 use the simT)
 */
void GameClient::Draw(TimeT simulationTime, double interpolation)
{
    mDemoScene->Render(*mRenderer.get(), simulationTime, interpolation);
}

/**
 * Sets the number of times the game will call the update method
 */
void GameClient::SetUpdateFrequency(int numUpdatesPerSecond)
{
    assert(numUpdatesPerSecond > 0);

    // Calculate the new update frequency and verify that it isn't zero
    // for some ungodly reason
    mUpdateFrequency = 1 / static_cast<TimeT>(numUpdatesPerSecond);
    assert(mUpdateFrequency > 0);

    // Logging
    LOG_TRACE("GameClient") << "Setting the update frequency to " << mUpdateFrequency;
}

/**
 * Queries the Windows API to find out this computer's update frequency, which
 * is the number of times the CPU ticks per second. The Hailstorm engine uses this
 * value to convert system tick count into seconds.
 *
 * Post-condition: Return value is always > 0.
 */
TimeT GameClient::CalculateSystemTimerFrequency()
{
    // Query windows for the internal high precision timer frequency. We
    // need to know this value in order to correctly convert timer values
    // into floating point seconds
    //      (look into possible skew with speed step or CPUTHRM)
    LARGE_INTEGER procFreq;
    BOOL result = ::QueryPerformanceFrequency( &procFreq );

    if (! result )
    {
        throw HailstormException(L"Unable to query performance frequency"); // TODO: Throw more specific exception.
    }

    if (procFreq.QuadPart == 0)
    {
        throw HailstormException(L"Failed to query processor frequency (quad part == 0)");
    }

    return static_cast<TimeT>(procFreq.QuadPart);
}

/**
 * Returns the current system time in seconds
 */
TimeT GameClient::GetCurrentTime() const
{
    // Get the current system time. We need to lock down the thread affinity because it is possible on MP machines that
    // cores have slightly different clock skews (yay).
    LARGE_INTEGER now;
    DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);

    if (!::QueryPerformanceCounter(&now))
    {
        throw HailstormException(L"Unable to query performance counter for time"); // TODO: Throw more specific exception.
    }

    ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);

    // Take the current time and the timer frequency to obtain a floating
    // point representation of the system time
    return static_cast<TimeT>(now.QuadPart) / mTimerFrequency;
}