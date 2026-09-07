#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class BasicGrid
{
public:

    bool Initialize(
        ID3D11Device* device,
        int width,
        int depth,
        float spacing
    );

    bool InitializeShaders(
        ID3D11Device* device
    );

    void Render(
        ID3D11DeviceContext* context,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& projection
    );

private:

    struct Vertex
    {
        DirectX::XMFLOAT3 position;
    };

    struct MatrixBuffer
    {
        DirectX::XMMATRIX worldViewProjection;
    };

private:

    Microsoft::WRL::ComPtr<ID3D11Buffer>
        m_VertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer>
        m_IndexBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer>
        m_MatrixBuffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>
        m_VertexShader;

    Microsoft::WRL::ComPtr<ID3D11PixelShader>
        m_PixelShader;

    Microsoft::WRL::ComPtr<ID3D11InputLayout>
        m_InputLayout;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>
        m_WireframeState;

    UINT m_IndexCount = 0;
};