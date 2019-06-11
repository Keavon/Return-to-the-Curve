#include "Frustum.h"

void Frustum::extractPlanes(mat4 P, mat4 V)
{
    mat4 comp = transpose(P * V);
    vec4 left, right, bottom, top, near, far;

    left.x = comp[3][0] + comp[0][0];
    left.y = comp[3][1] + comp[0][1];
    left.z = comp[3][2] + comp[0][2];
    left.w = comp[3][3] + comp[0][3];
    planes[0] = left / length(vec3(left));

    right.x = comp[3][0] - comp[0][0];
    right.y = comp[3][1] - comp[0][1];
    right.z = comp[3][2] - comp[0][2];
    right.w = comp[3][3] - comp[0][3];
    planes[1] = right / length(vec3(right));

    bottom.x = comp[3][0] + comp[1][0];
    bottom.y = comp[3][1] + comp[1][1];
    bottom.z = comp[3][2] + comp[1][2];
    bottom.w = comp[3][3] + comp[1][3];
    planes[2] = bottom / length(vec3(bottom));

    top.x = comp[3][0] - comp[1][0];
    top.y = comp[3][1] - comp[1][1];
    top.z = comp[3][2] - comp[1][2];
    top.w = comp[3][3] - comp[1][3];
    planes[3] = top / length(vec3(top));

    near.x = comp[3][0] + comp[2][0];
    near.y = comp[3][1] + comp[2][1];
    near.z = comp[3][2] + comp[2][2];
    near.w = comp[3][3] + comp[2][3];
    planes[4] = near / length(vec3(near));

    far.x = comp[3][0] - comp[2][0];
    far.y = comp[3][1] - comp[2][1];
    far.z = comp[3][2] - comp[2][2];
    far.w = comp[3][3] - comp[2][3];
    planes[5] = far / length(vec3(far));
}

bool Frustum::checkAABB(const vec3 &mins, const vec3 &maxs)
{
    // https://gist.github.com/Kinwailo/d9a07f98d8511206182e50acda4fbc9b

    vec3 vmin;

    for (int i = 0; i < 6; i++)
    {
        // X axis
        if (planes[i].x < 0)
        {
            vmin.x = mins.x;
        }
        else
        {
            vmin.x = maxs.x;
        }
        // Y axis
        if (planes[i].y < 0)
        {
            vmin.y = mins.y;
        }
        else
        {
            vmin.y = maxs.y;
        }
        // Z axis
        if (planes[i].z < 0)
        {
            vmin.z = mins.z;
        }
        else
        {
            vmin.z = maxs.z;
        }
        if (dot(vec3(planes[i]), vmin) + planes[i].w <= 0)
        {
            return false;
        }
    }

    return true;
}

bool Frustum::checkSphere(const vec3 &center, float radius)
{
    for (int i = 0; i < 6; i++)
    {
        float dist = center.x * planes[i].x + center.y * planes[i].y + center.z * planes[i].z + planes[i].w;
        if (dist < -radius)
        {
            return false;
        }
    }
    
    return true;
}