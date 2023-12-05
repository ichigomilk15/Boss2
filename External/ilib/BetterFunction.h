#pragma once

#include<float.h>
#include <utility>
#include <DirectXMath.h>
#include <concepts>

//DirectX::operatorを使うにはusing namespace DirectX; を宣言してください


namespace std{

    template<typename T>
    inline T Clamp(T value, T Max, T Min)
    {
        return (std::min)((std::max)(value, Min), Max);
    }

    inline bool NearlyEqual(float left, float right)
    {
        return fabsf(left - right) <= FLT_EPSILON * fmaxf(1, fmaxf(fabsf(left), fabsf(right)));
    }
    inline bool NearlyEqual(double left, double right)
    {
        return fabs(left - right) <= DBL_EPSILON * fmax(1, fmax(fabs(left), fabs(right)));
    }
    inline bool NearlyEqual(long double left, long double right)
    {
        return fabsl(left - right) <= LDBL_EPSILON * fmaxl(1, fmaxl(fabsl(left), fabsl(right)));
    }
}

namespace DirectX
{
    namespace  //XMFLOAT2
    {
        inline DirectX::XMFLOAT2 Normalize(DirectX::XMFLOAT2 v)
        {
            float dir = sqrtf(v.x * v.x + v.y * v.y);
            return DirectX::XMFLOAT2{ v.x / dir,v.y / dir };
        }

//DirectX::operatorを使うにはusing namespace DirectX; を宣言してください
        inline namespace ope
        {

        inline XMFLOAT2 operator+(XMFLOAT2 left, XMFLOAT2 right)
        {
            return XMFLOAT2{ left.x + right.x,left.y + right.y };
        }
        inline XMFLOAT2 operator-(XMFLOAT2 left, XMFLOAT2 right)
        {
            return XMFLOAT2{ left.x - right.x,left.y - right.y };
        }
        inline XMFLOAT2 operator*(XMFLOAT2 left, XMFLOAT2 right)
        {
            return XMFLOAT2{ left.x * right.x,left.y * right.y };
        }
        inline XMFLOAT2 operator/(XMFLOAT2 left, XMFLOAT2 right)
        {
            return XMFLOAT2{ left.x / right.x,left.y / right.y };
        }
        inline XMFLOAT2 operator*(XMFLOAT2 v, float value)
        {
            return XMFLOAT2{ v.x * value,v.y * value};
        }
        inline XMFLOAT2 operator/(XMFLOAT2 v, float value)
        {
            return XMFLOAT2{ v.x / value,v.y / value, };
        }
        }
    }
    namespace   //XMFLOAT3
    {
        inline float Length(const XMFLOAT3 v)
        {
            return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        inline DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 v)
        {
            float dir = Length(v);
            return DirectX::XMFLOAT3{ v.x / dir,v.y / dir , v.z / dir };
        }

//DirectX::operatorを使うにはusing namespace DirectX; を宣言してください
        inline namespace ope
        {

        inline XMFLOAT3 operator+(XMFLOAT3 left, XMFLOAT3 right)
        {
            return XMFLOAT3{ left.x + right.x,left.y + right.y,left.z + right.z };
        }
        inline XMFLOAT3 operator-(XMFLOAT3 left, XMFLOAT3 right)
        {
            return XMFLOAT3{ left.x - right.x,left.y - right.y,left.z - right.z };
        }
        inline XMFLOAT3 operator*(XMFLOAT3 left, XMFLOAT3 right)
        {
            return XMFLOAT3{ left.x * right.x,left.y * right.y,left.z * right.z };
        }
        inline XMFLOAT3 operator/(XMFLOAT3 left, XMFLOAT3 right)
        {
            return XMFLOAT3{ left.x / right.x,left.y / right.y,left.z / right.z };
        }
        inline XMFLOAT3 operator*(XMFLOAT3 v, float value)
        {
            return XMFLOAT3{ v.x * value,v.y * value,v.z * value };
        }
        inline XMFLOAT3 operator/(XMFLOAT3 v, float value)
        {
            return XMFLOAT3{ v.x / value,v.y / value,v.z / value };
        }
        inline XMFLOAT3 operator-(XMFLOAT3 v)
        {
            return XMFLOAT3{ -v.x,-v.y,-v.z };
        }
        }
    }
    namespace   //XMFLOAT4
    {
        inline DirectX::XMFLOAT4 Normalize(DirectX::XMFLOAT4 v)
        {
            float dir = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z+ v.w * v.w);
            return DirectX::XMFLOAT4{ v.x / dir,v.y / dir , v.z / dir,v.w/dir };
        }

//DirectX::operatorを使うにはusing namespace DirectX; を宣言してください
        inline namespace ope
        {

        inline XMFLOAT4 operator+(XMFLOAT4 left, XMFLOAT4 right)
        {
            return XMFLOAT4{ left.x + right.x,left.y + right.y,left.z + right.z,left.w + right.w };
        }
        inline XMFLOAT4 operator-(XMFLOAT4 left, XMFLOAT4 right)
        {
            return XMFLOAT4{ left.x - right.x,left.y - right.y,left.z - right.z,left.w - right.w };
        }
        inline XMFLOAT4 operator*(XMFLOAT4 left, XMFLOAT4 right)
        {
            return XMFLOAT4{ left.x * right.x,left.y * right.y,left.z * right.z,left.w * right.w };
        }
        inline XMFLOAT4 operator/(XMFLOAT4 left, XMFLOAT4 right)
        {
            return XMFLOAT4{ left.x / right.x,left.y / right.y,left.z / right.z,left.w / right.w };
        }
        }
        
    }
}
