#pragma once

#include <cmath>
#include <vector>

#include "Vec.h"
#include "Sphere.h"
#include "Triangle.h"

class Model
{
public:
    const std::vector<Vec3f>    vertices        ;
    const std::vector<Triangle> triangles       ;
    const Sphere                bounding_sphere ;

    Model( std::vector<Vec3f> vertices, std::vector<Triangle> triangles )
        : vertices( std::move( vertices ) )
        , triangles( std::move( triangles ) )
        , bounding_sphere( compute_bounding_sphere() )
    {
    }

private:
    Sphere compute_bounding_sphere() const
    {
        if( vertices.empty() )
            return { { 0, 0, 0 }, 0 } ;

        Vec3f mins { vertices[ 0 ].x, vertices[ 0 ].y, vertices[ 0 ].z } ;
        Vec3f maxs { vertices[ 0 ].x, vertices[ 0 ].y, vertices[ 0 ].z } ;

        for( size_t i = 1 ; i < vertices.size() ; ++i  )
        {
            if( vertices[ i ].x < mins.x ) mins.x = vertices[ i ].x ;
            if( vertices[ i ].y < mins.y ) mins.y = vertices[ i ].y ;
            if( vertices[ i ].z < mins.z ) mins.z = vertices[ i ].z ;
            if( vertices[ i ].x > maxs.x ) maxs.x = vertices[ i ].x ;
            if( vertices[ i ].y > maxs.y ) maxs.y = vertices[ i ].y ;
            if( vertices[ i ].z > maxs.z ) maxs.z = vertices[ i ].z ;
        }

        Vec3f centroid { ( mins.x + maxs.x ) / 2,
                         ( mins.y + maxs.y ) / 2 ,
                         ( mins.z + maxs.z ) / 2 } ;

        float radius_squared = 0 ;

        Vec3f corner3 { mins.x, mins.y, maxs.z } ;
        Vec3f corner4 { mins.x, maxs.y, maxs.z } ;
        Vec3f corner5 { mins.x, maxs.y, mins.z } ;
        Vec3f corner6 { maxs.x, mins.y, mins.z } ;
        Vec3f corner7 { maxs.x, mins.y, maxs.z } ;
        Vec3f corner8 { maxs.x, maxs.y, mins.z } ;

        radius_squared = get_max_distance_squared( centroid,    mins, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid,    maxs, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner3, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner4, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner5, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner6, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner7, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner8, radius_squared ) ;

        return { centroid, std::sqrt( radius_squared ) } ;
    }

    static float get_max_distance_squared(
        const Vec3f& centroid, const Vec3f& corner, float max_radius_squred_so_far )
    {
        auto dist_squared = ( corner.x - centroid.x ) * ( corner.x - centroid.x )
                          + ( corner.y - centroid.y ) * ( corner.y - centroid.y )
                          + ( corner.z - centroid.z ) * ( corner.z - centroid.z ) ;

        return std::max( dist_squared, max_radius_squred_so_far ) ;
    }

} ;
