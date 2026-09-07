#include "D3D11Renderer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

bool D3D11Renderer::Initialize(HWND hwnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    swapChainDesc.BufferCount = 1;

    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapChainDesc.OutputWindow = hwnd;

    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        m_SwapChain.GetAddressOf(),
        m_Device.GetAddressOf(),
        nullptr,
        m_DeviceContext.GetAddressOf()
    );

    if (FAILED(hr))
        return false;

    // SwapChain의 BackBuffer 가져오기
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    hr = m_SwapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(backBuffer.GetAddressOf())
    );

    if (FAILED(hr))
        return false;

    // BackBuffer를 RenderTarget으로 사용
    hr = m_Device->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        m_RenderTargetView.GetAddressOf()
    );

    if (FAILED(hr))
        return false;

    D3D11_TEXTURE2D_DESC depthDesc = {};

    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_Device->CreateTexture2D(
        &depthDesc,
        nullptr,
        m_DepthStencilBuffer.GetAddressOf()
    );

    if (FAILED(hr))
        return false;

    hr = m_Device->CreateDepthStencilView(
        m_DepthStencilBuffer.Get(),
        nullptr,
        m_DepthStencilView.GetAddressOf()
    );

    if (FAILED(hr))
        return false;

    m_DeviceContext->OMSetRenderTargets(
        1,
        m_RenderTargetView.GetAddressOf(),
        m_DepthStencilView.Get()
    );

    // 렌더링 영역
    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);

    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_DeviceContext->RSSetViewports(1, &viewport);

    return true;
}

void D3D11Renderer::BeginFrame()
{
    const float clearColor[4] =
    {
        0.05f,
        0.15f,
        0.25f,
        1.0f
    };

    m_DeviceContext->ClearRenderTargetView(
        m_RenderTargetView.Get(),
        clearColor
    );

    m_DeviceContext->ClearDepthStencilView(
        m_DepthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0
    );
}

void D3D11Renderer::EndFrame()
{
    m_SwapChain->Present(1, 0);
}

void D3D11Renderer::Shutdown()
{
    if (m_DeviceContext)
    {
        m_DeviceContext->ClearState();
    }

    m_DepthStencilView.Reset();
    m_DepthStencilBuffer.Reset();

    m_RenderTargetView.Reset();

    m_SwapChain.Reset();
    m_DeviceContext.Reset();
    m_Device.Reset();
}