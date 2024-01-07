#include "Canvas.h"

// These clipping planes result in a 45 degree field of view
const Plane Canvas::clipping_planes[] =
{
    { {                   0,                   0,                  1 }, 1 }, // Near   clipping plane
    { {  square_root_of_two,                   0, square_root_of_two }, 0 }, // Left   clipping plane
    { { -square_root_of_two,                   0, square_root_of_two }, 0 }, // Right  clipping plane
    { {                   0, -square_root_of_two, square_root_of_two }, 0 }, // Top    clipping plane
    { {                   0,  square_root_of_two, square_root_of_two }, 0 }, // Bottom clipping plane
} ;
