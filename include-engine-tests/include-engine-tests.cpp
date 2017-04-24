#include "transform.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

void require_approx_equal(const float3 & a, const float3 & b)
{
    REQUIRE(a.x == Approx(b.x));
    REQUIRE(a.y == Approx(b.y));
    REQUIRE(a.z == Approx(b.z));
}

template<class Transform> void test_affine_transform(const Transform & t)
{
    // Start by transforming three points using the supplied transformation
    const float3 p0 {1,2,3}, p1 {4,-5,2}, p2 {-3,1,-4};
    const float3 pp0 = transform_point(t, p0);
    const float3 pp1 = transform_point(t, p1);
    const float3 pp2 = transform_point(t, p2);

    // Test transform_vector(...)
    const float3 e1 = p1 - p0, e2 = p2 - p0;
    const float3 ee1 = pp1 - pp0, ee2 = pp2 - pp0;
    require_approx_equal(transform_vector(t, e1), ee1);
    require_approx_equal(transform_vector(t, e2), ee2);

    // Test transform_tangent(...)
    require_approx_equal(transform_tangent(t, normalize(e1)), normalize(ee1));
    require_approx_equal(transform_tangent(t, normalize(e2)), normalize(ee2));

    // Test transform_normal(...)
    const float3 n = normalize(cross(e1, e2));
    const float3 nn = normalize(cross(ee1, ee2));
    require_approx_equal(transform_normal(t, n), nn);
}

template<class Transform> void test_rigid_transform(const Transform & t)
{
    test_affine_transform(t);

    // Start by transforming three points using the supplied transformation
    const float3 p0 {1,2,3}, p1 {4,-5,2}, p2 {-3,1,-4};
    const float3 pp0 = transform_point(t, p0);
    const float3 pp1 = transform_point(t, p1);
    const float3 pp2 = transform_point(t, p2);
    const float3 e1 = p1 - p0, e2 = p2 - p0;
    const float3 ee1 = pp1 - pp0, ee2 = pp2 - pp0;

    // Test transform_quat(...)
    const float4 q = rotation_quat(float3{0,0,1}, 1.0f);
    const float4 qq = transform_quat(t, q);
    require_approx_equal(qrot(qq, ee1), transform_vector(t, qrot(q, e1)));
    require_approx_equal(qrot(qq, ee2), transform_vector(t, qrot(q, e2)));

    // Test transform_matrix(...)
    const float4x4 m {{1,0,0,0},{0,0,1,0},{0,1,0,0},{0,0,0,1}};
    const float4x4 mm = transform_matrix(t, m);
    require_approx_equal(transform_vector(mm, ee1), transform_vector(t, transform_vector(m, e1)));
    require_approx_equal(transform_vector(mm, ee2), transform_vector(t, transform_vector(m, e2)));
}

template<class Transform> void test_scale_preserving_transform(const Transform & t)
{
    test_rigid_transform(t);

    // Start by transforming three points using the supplied transformation
    const float3 p0 {1,2,3}, p1 {4,-5,2}, p2 {-3,1,-4};
    const float3 pp0 = transform_point(t, p0);
    const float3 pp1 = transform_point(t, p1);
    const float3 pp2 = transform_point(t, p2);
    const float3 e1 = p1 - p0, e2 = p2 - p0;
    const float3 ee1 = pp1 - pp0, ee2 = pp2 - pp0;

    // Test transform_scaling(...)
    const float3 s {1,1,2};
    const float3 ss = transform_scaling(t, s);
    require_approx_equal(ss * ee1, transform_vector(t, s * e1));
    require_approx_equal(ss * ee2, transform_vector(t, s * e2));
}

TEST_CASE("transform functions", "[transform]")
{
    // Test a transform which apply non-uniform scales in one axis
    test_affine_transform(float3x3{{1,0,0},{0,2,0},{0,0,1}});
    test_affine_transform(float3x3{{2,0,0},{0,3,0},{0,0,4}});
    test_affine_transform(float4x4{{-5,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}});

    // Test rigid transforms
    test_rigid_transform(mul(rotation_matrix(normalize(float4{1,2,3,4})), translation_matrix(float3{2,5,3})));
    test_rigid_transform(rigid_pose{{1,2,3}, normalize(float4{8,0,0,6})});

    // Test transform which rearranges axes, some of which involve a handedness transform
    test_scale_preserving_transform(float3x3{{1,0,0},{0,0,-1},{0,1,0}}); // rotation
    test_scale_preserving_transform(float3x3{{1,0,0},{0,0,1},{0,1,0}}); // mirror
    test_scale_preserving_transform(float4x4{{0,0,1,0},{0,1,0,0},{-1,0,0,0},{0,0,0,1}}); // rotation
    test_scale_preserving_transform(float4x4{{0,1,0,0},{0,0,1,0},{1,0,0,0},{0,0,0,1}}); // rotation
    test_scale_preserving_transform(float4x4{{-1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}); // mirror
}