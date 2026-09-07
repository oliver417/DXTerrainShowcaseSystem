#include "BasicGrid.h"

#include <vector>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

bool BasicGrid::Initialize(
    ID3D11Device* device,
    int width,
    int depth,
    float spacing)
{
    if (!device)
        return false;

    std::vector<Vertex> vertices;

    // -------------------------------
    // Vertex 생성
    // -------------------------------

    const float halfWidth =
        (width - 1) * spacing * 0.5f;

    const float halfDepth =
        (depth - 1) * spacing * 0.5f;

    for (int z = 0; z < depth; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            float px =
                static_cast<float>(x) * spacing
                - halfWidth;

            float pz =
                static_cast<float>(z) * spacing
                - halfDepth;

            Vertex vertex;

            vertex.position =
                XMFLOAT3(
                    px,
                    0.0f,
                    pz
                );

            vertices.push_back(vertex);
        }
    }

    // -------------------------------
    // Index 생성
    //
    // 하나의 셀을 삼각형 2개로 만든다.
    //
    // 0 ---- 1
    // |    / |
    // |  /   |
    // 2 ---- 3
    // -------------------------------

    std::vector<UINT> indices;

    for (int z = 0; z < depth - 1; ++z)
    {
        for (int x = 0; x < width - 1; ++x)
        {
            UINT topLeft =
                z * width + x;

            UINT topRight =
                topLeft + 1;

            UINT bottomLeft =
                (z + 1) * width + x;

            UINT bottomRight =
                bottomLeft + 1;

            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_IndexCount =
        static_cast<UINT>(indices.size());

    // -------------------------------
    // Vertex Buffer
    // -------------------------------

    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.Usage =
        D3D11_USAGE_DEFAULT;

    vertexBufferDesc.ByteWidth =
        static_cast<UINT>(
            sizeof(Vertex) *
            vertices.size()
            );

    vertexBufferDesc.BindFlags =
        D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};

    vertexData.pSysMem =
        vertices.data();

    HRESULT hr =
        device->CreateBuffer(
            &vertexBufferDesc,
            &vertexData,
            m_VertexBuffer.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    // -------------------------------
    // Index Buffer
    // -------------------------------

    D3D11_BUFFER_DESC indexBufferDesc = {};

    indexBufferDesc.Usage =
        D3D11_USAGE_DEFAULT;

    indexBufferDesc.ByteWidth =
        static_cast<UINT>(
            sizeof(UINT) *
            indices.size()
            );

    indexBufferDesc.BindFlags =
        D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};

    indexData.pSysMem =
        indices.data();

    hr =
        device->CreateBuffer(
            &indexBufferDesc,
            &indexData,
            m_IndexBuffer.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    // -------------------------------
    // Constant Buffer
    // -------------------------------

    D3D11_BUFFER_DESC matrixBufferDesc = {};

    matrixBufferDesc.Usage =
        D3D11_USAGE_DEFAULT;

    matrixBufferDesc.ByteWidth =
        sizeof(MatrixBuffer);

    matrixBufferDesc.BindFlags =
        D3D11_BIND_CONSTANT_BUFFER;

    hr =
        device->CreateBuffer(
            &matrixBufferDesc,
            nullptr,
            m_MatrixBuffer.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    return InitializeShaders(device);
}

bool BasicGrid::InitializeShaders(
    ID3D11Device* device)
{
    Microsoft::WRL::ComPtr<ID3DBlob>
        vertexShaderBlob;

    Microsoft::WRL::ComPtr<ID3DBlob>
        pixelShaderBlob;

    Microsoft::WRL::ComPtr<ID3DBlob>
        errorBlob;

    HRESULT hr =
        D3DCompileFromFile(
            L"Shaders/TerrainVS.hlsl",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main",
            "vs_5_0",
            D3DCOMPILE_DEBUG,
            0,
            vertexShaderBlob.GetAddressOf(),
            errorBlob.GetAddressOf()
        );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()
                    )
            );
        }

        return false;
    }

    errorBlob.Reset();

    hr =
        D3DCompileFromFile(
            L"Shaders/TerrainPS.hlsl",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main",
            "ps_5_0",
            D3DCOMPILE_DEBUG,
            0,
            pixelShaderBlob.GetAddressOf(),
            errorBlob.GetAddressOf()
        );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()
                    )
            );
        }

        return false;
    }

    hr =
        device->CreateVertexShader(
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            nullptr,
            m_VertexShader.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    hr =
        device->CreatePixelShader(
            pixelShaderBlob->GetBufferPointer(),
            pixelShaderBlob->GetBufferSize(),
            nullptr,
            m_PixelShader.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    // -------------------------------
    // Input Layout
    // -------------------------------

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    hr =
        device->CreateInputLayout(
            layout,
            1,
            vertexShaderBlob->GetBufferPointer(),
            vertexShaderBlob->GetBufferSize(),
            m_InputLayout.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    // -------------------------------
    // Wireframe Rasterizer
    // -------------------------------

    D3D11_RASTERIZER_DESC rasterDesc = {};

    rasterDesc.FillMode =
        D3D11_FILL_WIREFRAME;

    rasterDesc.CullMode =
        D3D11_CULL_NONE;

    rasterDesc.DepthClipEnable =
        TRUE;

    hr =
        device->CreateRasterizerState(
            &rasterDesc,
            m_WireframeState.GetAddressOf()
        );

    if (FAILED(hr))
        return false;

    return true;
}

void BasicGrid::Render(
    ID3D11DeviceContext* context,
    const XMMATRIX& view,
    const XMMATRIX& projection)
{
    if (!context)
        return;

    UINT stride =
        sizeof(Vertex);

    UINT offset = 0;

    ID3D11Buffer* vertexBuffer =
        m_VertexBuffer.Get();

    context->IASetVertexBuffers(
        0,
        1,
        &vertexBuffer,
        &stride,
        &offset
    );

    context->IASetIndexBuffer(
        m_IndexBuffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0
    );

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    context->IASetInputLayout(
        m_InputLayout.Get()
    );

    // -------------------------------
    // Matrix
    // -------------------------------

    XMMATRIX world =
        XMMatrixIdentity();

    XMMATRIX wvp =
        world *
        view *
        projection;

    MatrixBuffer matrixData;

    matrixData.worldViewProjection =
        XMMatrixTranspose(wvp);

    context->UpdateSubresource(
        m_MatrixBuffer.Get(),
        0,
        nullptr,
        &matrixData,
        0,
        0
    );

    ID3D11Buffer* matrixBuffer =
        m_MatrixBuffer.Get();

    context->VSSetConstantBuffers(
        0,
        1,
        &matrixBuffer
    );

    // -------------------------------
    // Shader
    // -------------------------------

    context->VSSetShader(
        m_VertexShader.Get(),
        nullptr,
        0
    );

    context->PSSetShader(
        m_PixelShader.Get(),
        nullptr,
        0
    );

    // Wireframe
    context->RSSetState(
        m_WireframeState.Get()
    );

    // Draw
    context->DrawIndexed(
        m_IndexCount,
        0,
        0
    );
}