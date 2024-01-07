#include "misc.h"
#include "Canvas.h"
#include "A3DBModel.h"
#include<iostream>

int main(int argc, char* argv[])
{

	Canvas c("", 600, 600);

	auto cube = A3DBModel::load("cube.a3db");

	if (cube == nullptr)
	{
		std::cout << "Failed to load model!" << std::endl;
		return -1;
	}

	ModelInstance cube_instance_1{ *cube, {-1.5, 0, 7}, .75 };

	ModelInstance cube_instance_2{ *cube, {1.25, 2.5, 7.5}, 1, 195, {0,1,0} };

	ModelInstance cube_instance_3{ *cube, {-1.5, 1, 0 } };

	c.set_camera_position({ -3, 1, 2 });
	c.set_camera_orientation(Mat::get_rotation_matrix(30, { 0,1,0 }));
	/*c.set_camera_position({ 0, 1, 1 });
	c.set_camera_orientation(Mat::get_rotation_matrix(90, { 0,0,1 }));*/


	float rotator = 0;

	while (should_keep_rendering())
	{
		c.clear();
		//C++ automatically figures out that with 2 parameters it can make a Vec2i because it expects one

		rotator += .25f;

		cube_instance_1.set_rotation(cube_instance_1.get_rotation_angle() + 2, {1,1,1});
		cube_instance_1.set_translation({ -1.5, static_cast<float>(std::sin(rotator)/2), 7 });

		cube_instance_2.set_rotation(cube_instance_2.get_rotation_angle() + 2, { 0,0,1 });

		//Order super matters on matrix math!!!
		c.draw_simple_model(cube_instance_1);
		c.draw_simple_model(cube_instance_2);
		c.draw_simple_model(cube_instance_3);


		


		c.present();
	}
	return 0;
}

/*
		{//Roof
			c.draw_triangle_2d_filled({ 0, 150 }, { -200, 0 }, { 200, 0 }, Color::white);

			//House Body
			c.draw_triangle_2d_filled({ -150,0 }, { 150,0 }, { -150, -250 }, Color::white);
			c.draw_triangle_2d_filled({ 150,-250 }, { 150,0 }, { -150, -250 }, Color::white);

			//left window

			c.draw_triangle_2d_filled({ -100, -50 }, { -50, -50 }, { -100, -100 }, Color::orange);
			c.draw_triangle_2d_filled({ -50, -100 }, { -50, -50 }, { -100, -100 }, Color::orange);


			//Right Window
			c.draw_triangle_2d_filled({ 100, -50 }, { 50, -50 }, { 100, -100 }, Color::orange);
			c.draw_triangle_2d_filled({ 50, -100 }, { 50, -50 }, { 100, -100 }, Color::orange);

			//Door
			c.draw_triangle_2d_filled({ -30, -150 }, { 30, -150 }, { -30, -250 }, Color::orange);
			c.draw_triangle_2d_filled({ 30, -250 }, { 30, -150 }, { -30, -250 }, Color::orange);
		}
		*/



		/*
		Vec3f ftl = { -2, -.5,  5 };
		Vec3f ftr = { -2,  .5,  5 };
		Vec3f fbl = { -1, -.5,  5 };
		Vec3f fbr = { -1,  .5,  5 };
		Vec3f btl = { -2, -.5,  6 };
		Vec3f btr = { -2,  .5,  6 };
		Vec3f bbl = { -1, -.5,  6 };
		Vec3f bbr = { -1,  .5,  6 };



		c.draw_line_3d(btl, btr, Color::dim_gray);
		c.draw_line_3d(btl, bbl, Color::dim_gray);
		c.draw_line_3d(bbr, btr, Color::dim_gray);
		c.draw_line_3d(bbr, bbl, Color::dim_gray);

		c.draw_line_3d(ftl, btl, Color::gray);
		c.draw_line_3d(ftr, btr, Color::gray);
		c.draw_line_3d(fbl, bbl, Color::gray);
		c.draw_line_3d(fbr, bbr, Color::gray);

		c.draw_line_3d(ftl, ftr, Color::white);
		c.draw_line_3d(ftl, fbl, Color::white);
		c.draw_line_3d(fbr, ftr, Color::white);
		c.draw_line_3d(fbr, fbl, Color::white);
*/