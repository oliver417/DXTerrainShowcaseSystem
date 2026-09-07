#include "Camera.h"

using namespace DirectX;

Camera::Camera()
    : m_Position(0.0f, 5.0f, -10.0f),
    m_Rotation(0.0f, 0.0f, 0.0f)
{
    XMStoreFloat4x4(
        &m_ViewMatrix,
        XMMatrixIdentity()
    );

    XMStoreFloat4x4(
        &m_ProjectionMatrix,
        XMMatrixIdentity()
    );
}

void Camera::SetPosition(float x, float y, float z)
{
    m_Position = XMFLOAT3(x, y, z);
}

void Camera::SetRotation(float pitch, float yaw, float roll)
{
    m_Rotation = XMFLOAT3(pitch, yaw, roll);
}

void Camera::SetProjection(
    float fovDegrees,
    float aspectRatio,
    float nearZ,
    float farZ)
{
    float fovRadians =
        XMConvertToRadians(fovDegrees);

    XMMATRIX projection =
        XMMatrixPerspectiveFovLH(
            fovRadians,
            aspectRatio,
            nearZ,
            farZ
        );

    XMStoreFloat4x4(
        &m_ProjectionMatrix,
        projection
    );
}

void Camera::Update()
{
    XMVECTOR position =
        XMLoadFloat3(&m_Position);

    float pitch = XMConvertToRadians(m_Rotation.x);
    float yaw = XMConvertToRadians(m_Rotation.y);
    float roll = XMConvertToRadians(m_Rotation.z);

    XMMATRIX rotation =
        XMMatrixRotationRollPitchYaw(
            pitch,
            yaw,
            roll
        );

    XMVECTOR forward =
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    forward =
        XMVector3TransformCoord(
            forward,
            rotation
        );

    XMVECTOR target =
        XMVectorAdd(position, forward);

    XMVECTOR up =
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view =
        XMMatrixLookAtLH(
            position,
            target,
            up
        );

    XMStoreFloat4x4(
        &m_ViewMatrix,
        view
    );
}

XMMATRIX Camera::GetViewMatrix() const
{
    return XMLoadFloat4x4(&m_ViewMatrix);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
    return XMLoadFloat4x4(&m_ProjectionMatrix);
}