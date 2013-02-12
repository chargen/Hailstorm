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
#ifndef SCOTT_HAILSTORM_GRAPHICS_DXRENDERER_H
#define SCOTT_HAILSTORM_GRAPHICS_DXRENDERER_H

#include "graphics/irenderer.h"

// temporary while we host the camera matrices
#include <d3dx10.h>

// Forward declarations
class MainWindow;
class GraphicsContentManager;
class CubeMesh;

struct IDXGISwapChain;
struct ID3D10RenderTargetView;
struct ID3D10Texture2D;
struct ID3D10DepthStencilView;
struct ID3D10Device;
struct ID3DX10Font;
struct ID3D10Effect;
struct ID3D10EffectTechnique;
struct ID3D10InputLayout;
struct ID3D10EffectMatrixVariable;

/**
 * This is the DirectX implementation of the abstract renderer
 */
class DXRenderer : public IRenderer
{
public:
    explicit DXRenderer( MainWindow *pWindow );
    virtual ~DXRenderer();

protected:
    virtual bool onStartRenderer();
    virtual void onStopRenderer();
    virtual void onRenderFrame( float currentTime, float deltaTime );
    virtual bool resizeRenderWindow( unsigned int width, unsigned int height );

private:
    bool createDeviceViews();
    void releaseDeviceViews();
    bool createRenderDevice();
	bool buildVertexLayout();
	bool buildFX();
    void destroyRenderDevice();
    bool createRenderFont();
    static bool verifyResult( HRESULT result, const std::string& action );

private:
    /// Pointer to the main rendering window
    MainWindow * mpMainWindow;

    /// Pointer to the D3D 10 device
    ID3D10Device * mpDevice;

    /// Pointer to the renderer swap chain
    IDXGISwapChain * mpSwapChain;

    /// Pointer to the render target view
    ID3D10RenderTargetView * mpRenderTargetView;

    /// Pointer to the depth stencil texture buffer
    ID3D10Texture2D * mpDepthStencilBuffer;

    /// Pointer to the depth stencil view
    ID3D10DepthStencilView * mpDepthStencilView;

	ID3D10Effect * mpFX;
	ID3D10EffectTechnique * mpTechnique;
	ID3D10InputLayout * mpVertexLayout;
	ID3D10EffectMatrixVariable * mpWVP;

    /// Pointer to the renderer font
    ID3DX10Font * mpRendererFont;

    /// Number of multi samples (Anti aliasing)
    UINT mMultisampleCount;

    /// Multi sample quality
    UINT mMultisampleQuality;

    /// Flag if we are rendering in windowed mode or full screen
    bool mWindowedMode;

	D3DXMATRIX mView;
	D3DXMATRIX mProjection;
	D3DXMATRIX mWVP;

	float mTheta;
	float mPhi;

    /// The currently running graphics content manager
    GraphicsContentManager * mpContentManager;

	CubeMesh * mpCubeMesh;
};

#endif