#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "linalg.h"
using namespace linalg::aliases;

// Composable, invertible value type representing a 6DOF rigid transform in 3D space
struct rigid_pose { float3 position {0,0,0}; float4 orientation {0,0,0,1}; };
inline rigid_pose mul       (const rigid_pose & a, const rigid_pose & b)          { return {a.position + qrot(a.orientation, b.position), qmul(a.orientation, b.orientation)}; }
inline rigid_pose nlerp     (const rigid_pose & a, const rigid_pose & b, float t) { return {lerp(a.position, b.position, t), nlerp(a.orientation, b.orientation, t)}; }
inline rigid_pose slerp     (const rigid_pose & a, const rigid_pose & b, float t) { return {lerp(a.position, b.position, t), slerp(a.orientation, b.orientation, t)}; }
inline rigid_pose inverse   (const rigid_pose & p)                                { auto q = qconj(p.orientation); return {qrot(q,-p.position), q}; }
inline float4x4 pose_matrix (const rigid_pose & p)                                { return pose_matrix(p.orientation, p.position); }

// A vector is the difference between two points in 3D space, possessing both direction and magnitude
inline float3 transform_vector   (const float4x4   & m, const float3 & vector)   { return mul(m, float4{vector,0}).xyz(); }
inline float3 transform_vector   (const float3x3   & m, const float3 & vector)   { return mul(m, vector); }
inline float3 transform_vector   (const rigid_pose & p, const float3 & vector)   { return qrot(p.orientation, vector); }

// A point is a specific location within a 3D space
inline float3 transform_point    (const float4x4   & m, const float3 & point)    { auto r=mul(m, float4{point,1}); return r.xyz()/r.w; }
inline float3 transform_point    (const float3x3   & m, const float3 & point)    { return transform_vector(m, point); }
inline float3 transform_point    (const rigid_pose & p, const float3 & point)    { return p.position + transform_vector(p, point); }

// A tangent is a unit-length vector which is parallel to a piece of geometry, such as a surface or a curve
inline float3 transform_tangent  (const float4x4   & m, const float3 & tangent)  { return normalize(transform_vector(m, tangent)); }
inline float3 transform_tangent  (const float3x3   & m, const float3 & tangent)  { return normalize(transform_vector(m, tangent)); }
inline float3 transform_tangent  (const rigid_pose & p, const float3 & tangent)  { return transform_vector(p, tangent); }

// A normal is a unit-length bivector which is perpendicular to a piece of geometry, such as a surface or a curve
inline float3 transform_normal   (const float4x4   & m, const float3 & normal)   { return normalize(transform_vector(inverse(transpose(m)), normal)) * (determinant(m) < 0 ? -1.0f : 1.0f); }
inline float3 transform_normal   (const float3x3   & m, const float3 & normal)   { return normalize(transform_vector(inverse(transpose(m)), normal)) * (determinant(m) < 0 ? -1.0f : 1.0f); }
inline float3 transform_normal   (const rigid_pose & p, const float3 & normal)   { return transform_vector(p, normal); }

// A quaternion can describe both a rotation and a uniform scaling in 3D space
inline float4 transform_quat     (const float4x4   & m, const float4 & quat)     { return {transform_vector(m, quat.xyz()) * (determinant(m) < 0 ? -1.0f : 1.0f), quat.w}; }
inline float4 transform_quat     (const float3x3   & m, const float4 & quat)     { return {transform_vector(m, quat.xyz()) * (determinant(m) < 0 ? -1.0f : 1.0f), quat.w}; }
inline float4 transform_quat     (const rigid_pose & p, const float4 & quat)     { return {transform_vector(p, quat.xyz()), quat.w}; }

// A matrix can describe a general transformation of homogeneous coordinates in projective space
inline float4x4 transform_matrix (const float4x4   & m, const float4x4 & matrix) { return mul(m, matrix, inverse(m)); }
inline float4x4 transform_matrix (const float3x3   & m, const float4x4 & matrix) { return transform_matrix({{m.x,0},{m.y,0},{m.z,0},{0,0,0,1}}, matrix); }
inline float4x4 transform_matrix (const rigid_pose & p, const float4x4 & matrix) { return transform_matrix(pose_matrix(p), matrix); }

// Scaling factors are not a vector, they are a compact representation of a scaling matrix
inline float3 transform_scaling  (const float4x4   & m, const float3 & scaling)  { return diagonal(transform_matrix(m, scaling_matrix(scaling))).xyz(); }
inline float3 transform_scaling  (const float3x3   & m, const float3 & scaling)  { return diagonal(transform_matrix(m, scaling_matrix(scaling))).xyz(); }
inline float3 transform_scaling  (const rigid_pose & p, const float3 & scaling)  { return transform_scaling(pose_matrix(p), scaling); }

#endif