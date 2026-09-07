#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

class D3D11Renderer
{
public:
    bool Initialize(HWND hwnd, int width, int height);

    void BeginFrame();
    void EndFrame();
    void Shutdown();

    ID3D11Device* GetDevice() const
    {
        return m_Device.Get();
    }

    ID3D11DeviceContext* GetContext() const
    {
        return m_DeviceContext.Get();
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
};