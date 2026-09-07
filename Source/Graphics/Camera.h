#pragma once

#include <DirectXMath.h>

class Camera
{
public:
    Camera();

    void SetPosition(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);

    void SetProjection(
        float fovDegrees,
        float aspectRatio,
        float nearZ,
        float farZ
    );

    void Update();

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

private:
    DirectX::XMFLOAT3 m_Position;
    DirectX::XMFLOAT3 m_Rotation;

    DirectX::XMFLOAT4X4 m_ViewMatrix;
    DirectX::XMFLOAT4X4 m_ProjectionMatrix;
};