#pragma once

#include <cstdint>

// Found the list of 48 named colors at:
//   https://simple.wikipedia.org/wiki/Template:Web_colors

class Color final
{
public:
    static Color pink ;
    static Color crimson ;
    static Color red ;
    static Color maroon ;
    static Color brown ;
    static Color misty_rose ;
    static Color salmon ;
    static Color coral ;
    static Color orange_red ;
    static Color chocolate ;
    static Color orange ;
    static Color gold ;
    static Color ivory ;
    static Color yellow ;
    static Color olive ;
    static Color yellow_green ;
    static Color lawn_green ;
    static Color chartreuse ;
    static Color lime ;
    static Color green ;
    static Color spring_green ;
    static Color aquamarine ;
    static Color turquoise ;
    static Color azure ;
    static Color cyan ;
    static Color teal ;
    static Color lavender ;
    static Color blue ;
    static Color navy ;
    static Color blue_violet ;
    static Color indigo ;
    static Color dark_violet ;
    static Color plum ;
    static Color magenta ;
    static Color purple ;
    static Color red_violet ;
    static Color tan ;
    static Color beige ;
    static Color slate_gray ;
    static Color dark_slate_gray ;
    static Color white ;
    static Color white_smoke ;
    static Color light_gray ;
    static Color silver ;
    static Color dark_gray ;
    static Color gray ;
    static Color dim_gray ;
    static Color black ;
    static Color zane_brown; 

    const uint8_t r ;
    const uint8_t g ;
    const uint8_t b ;

    static Color custom( uint8_t r, uint8_t g, uint8_t b )
    {
        return { r, g, b } ;
    }

private:
    Color( uint8_t r, uint8_t g, uint8_t b )
        : r( r ), g( g ), b( b )
    {}

} ;
