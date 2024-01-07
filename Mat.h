#pragma once

#include <array>
#include <cmath>

#include "misc.h"
#include "Vec.h"

class Mat
{
public:
    std::array<float, 16> elements = { 0 } ;

    static Mat get_identity_matrix()
    {
        Mat output ;
        output.elements[  0 ] = 1 ;
        output.elements[  5 ] = 1 ;
        output.elements[ 10 ] = 1 ;
        output.elements[ 15 ] = 1 ;
        return output ;
    }

    static Mat get_scale_matrix( float scale )
    {
        Mat output ;
        output.elements[  0 ] = scale ;
        output.elements[  5 ] = scale ;
        output.elements[ 10 ] = scale ;
        output.elements[ 15 ] = 1 ;
        return output ;
    }

    static Mat get_rotation_matrix( float degrees, const Vec3f& around )
    {
        // Normalize "around" vector
        auto vec_mag = std::sqrt( ( around.x * around.x ) + ( around.y * around.y ) + ( around.z * around.z ) ) ;
        auto u = Vec3f { around.x / vec_mag, around.y / vec_mag, around.z / vec_mag } ;

        auto sin = std::sin( degrees * pi / 180.0f ) ;
        auto cos = std::cos( degrees * pi / 180.0f ) ;

        Mat output ;
        output.elements[  0 ] = cos + u.x * u.x * ( 1 - cos ) ;
        output.elements[  1 ] = u.x * u.y * ( 1 - cos ) - u.z * sin ;
        output.elements[  2 ] = u.x * u.z * ( 1 - cos ) + u.y * sin ;
        output.elements[  4 ] = u.y * u.x * ( 1 - cos ) + u.z * sin ;
        output.elements[  5 ] = cos + u.y * u.y * ( 1 - cos ) ;
        output.elements[  6 ] = u.y * u.z * ( 1 - cos ) - u.x * sin ;
        output.elements[  8 ] = u.z * u.x * ( 1 - cos ) - u.y * sin ;
        output.elements[  9 ] = u.z * u.y * ( 1 - cos ) + u.x * sin ;
        output.elements[ 10 ] = cos + u.z * u.z * ( 1 - cos ) ;
        output.elements[ 15 ] = 1 ;
        return output ;
    }

    static Mat get_translation_matrix( Vec3f translation )
    {
        Mat output ;
        output.elements[  0 ] = 1 ;
        output.elements[  3 ] = translation.x ;
        output.elements[  5 ] = 1 ;
        output.elements[  7 ] = translation.y ;
        output.elements[ 10 ] = 1 ;
        output.elements[ 11 ] = translation.z ;
        output.elements[ 15 ] = 1 ;
        return output ;
    }

    //Rows to collumns, collumns to row
    Mat transpose() const
    {
        Mat output ;
        for( size_t i = 0 ; i < 4 ; ++i )
        for( size_t j = 0 ; j < 4 ; ++j )
            output.elements[ i * 4 + j ] = elements[ j * 4 + i ] ;
        return output ;
    }

    Mat multiply( const Mat& other ) const
    {
        Mat output ;
        for( size_t i = 0 ; i < 4 ; ++i )
        for( size_t j = 0 ; j < 4 ; ++j )
        for( size_t k = 0 ; k < 4 ; ++k )
            output.elements[ i * 4 + j ] += elements[ i * 4 + k ] * other.elements[ k * 4 + j ] ;
        return output ;
    }

    //This should 
    Vec4f multiply( const Vec3f& vector ) const
    {
        Vec4f output ;
        output.x = elements[  0 ] * vector.x
                 + elements[  1 ] * vector.y
                 + elements[  2 ] * vector.z
                 + elements[  3 ] ;
        output.y = elements[  4 ] * vector.x
                 + elements[  5 ] * vector.y
                 + elements[  6 ] * vector.z
                 + elements[  7 ] ;
        output.z = elements[  8 ] * vector.x
                 + elements[  9 ] * vector.y
                 + elements[ 10 ] * vector.z
                 + elements[ 11 ] ;
        output.w = elements[ 12 ] * vector.x
                 + elements[ 13 ] * vector.y
                 + elements[ 14 ] * vector.z
                 + elements[ 15 ] ;
        return output ;
    }

    Vec4f multiply( const Vec4f& vector ) const
    {
        Vec4f output ;
        output.x = elements[  0 ] * vector.x
                 + elements[  1 ] * vector.y
                 + elements[  2 ] * vector.z
                 + elements[  3 ] * vector.w ;
        output.y = elements[  4 ] * vector.x
                 + elements[  5 ] * vector.y
                 + elements[  6 ] * vector.z
                 + elements[  7 ] * vector.w ;
        output.z = elements[  8 ] * vector.x
                 + elements[  9 ] * vector.y
                 + elements[ 10 ] * vector.z
                 + elements[ 11 ] * vector.w ;
        output.w = elements[ 12 ] * vector.x
                 + elements[ 13 ] * vector.y
                 + elements[ 14 ] * vector.z
                 + elements[ 15 ] * vector.w ;
        return output ;
    }

    friend Mat operator*( const Mat& a, const Mat& b )
    {
        return a.multiply( b ) ;
    }

    friend Mat operator*=( Mat& a, const Mat& b )
    {
        a = a.multiply( b ) ;
        return a ;
    }

    friend Vec4f operator*( const Mat& m, const Vec3f& v )
    {
        return m.multiply( v ) ;
    }

    friend Vec4f operator*( const Mat& m, const Vec4f& v )
    {
        return m.multiply( v ) ;
    }
} ;
