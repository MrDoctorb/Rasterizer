// ReSharper disable CppClangTidyReadabilitySuspiciousCallArgument
#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include "Mat.h"
#include "Plane.h"
#include "CanvasBase.h"
#include "ModelInstance.h"

class Canvas final : public CanvasBase
{
	struct CorrespondingCoordinateLists
	{
		std::vector<float> left{};
		std::vector<float> right{};
	};

	static constexpr float viewport_size = 1.0;
	static constexpr float projection_plane_z = 1.0;
	static const     Plane clipping_planes[5];

public:
	Canvas(const char* window_title, size_t width, size_t height)
		: CanvasBase(window_title, width, height)
		, _camera_position(Vec3f{ 0, 0, 0 })
		, _camera_orientation(Mat::get_identity_matrix())
		, _camera_transform(Mat::get_identity_matrix())
	{
		_depth_buffer = std::vector<float>(_width * _height, 0.0f);

	}

	void clear() override
	{
		CanvasBase::clear();
		_depth_buffer = std::vector<float>(_width * _height, 0.0f);
	}

	void set_camera_position(const Vec3f& position)
	{
		_camera_position = position;
		compose_camera_transform();
	}

	void set_camera_orientation(const Mat& orientation)
	{
		_camera_orientation = orientation;
		compose_camera_transform();
	}

	void draw_simple_model(const ModelInstance& instance) 
	{
		auto overall_transform = _camera_transform * instance.get_transformation();

		auto clipped_model = clip_model(instance, overall_transform);

		if (clipped_model == nullptr) //No model to draw, leave
			return;

		std::vector<Vec2i> projected_vertices(clipped_model->vertices.size());
		for (size_t i = 0; i < clipped_model->vertices.size(); ++i)
			projected_vertices[i] = project_vertex(clipped_model->vertices[i]);

		for (auto& triangle : clipped_model->triangles)
		{
			//Cull all back facing triandles
			auto vertex = clipped_model->vertices[triangle.vertex_indices.x];
			auto normal = compute_triangle_normal(
				clipped_model->vertices[triangle.vertex_indices.x],
				clipped_model->vertices[triangle.vertex_indices.y],
				clipped_model->vertices[triangle.vertex_indices.z]
			);
			if (compute_dot_product(vertex, normal) <= 0)
			{
				continue;
			}

			draw_triangle_2d_filled(
				projected_vertices[triangle.vertex_indices.x],
				projected_vertices[triangle.vertex_indices.y],
				projected_vertices[triangle.vertex_indices.z],
				clipped_model->vertices[triangle.vertex_indices.x].z,
				clipped_model->vertices[triangle.vertex_indices.y].z,
				clipped_model->vertices[triangle.vertex_indices.z].z,
				triangle.color);
		}
	}

private:
	Vec3f _camera_position;
	Mat   _camera_orientation;
	Mat   _camera_transform;
	std::vector<float> _depth_buffer{};

	void compose_camera_transform()
	{
		_camera_transform = _camera_orientation.transpose()
			* Mat::get_translation_matrix(-_camera_position);
	}

	std::unique_ptr<Model> clip_model(const ModelInstance& instance, const Mat& transform) const
	{
		//----------------------------------------------------------------------------------------
		// Phase 1: Reject the model if it is clipped entirely
		//----------------------------------------------------------------------------------------

		// Get the transformed center and radius of the model's bounding sphere
		auto transformed_center = transform * instance.model.bounding_sphere.center;
		auto transformed_radius = instance.model.bounding_sphere.radius * instance.get_scale();

		// Discard instance if it is entirely outside of the viewing frustum
		for (auto& clipping_plane : clipping_planes)
		{
			auto distance = compute_dot_product(clipping_plane.normal, transformed_center)
				+ clipping_plane.distance;
			if (distance < -transformed_radius)//Entire sphere is outside of the plane
			{

				return nullptr;
			}
		}
		//----------------------------------------------------------------------------------------
		// Phase 2: Clip individual triangls in the model
		//----------------------------------------------------------------------------------------

		// Transform vertices
		std::vector<Vec3f> verticies(instance.model.vertices.size());
		for (size_t i = 0; i < instance.model.vertices.size(); ++i)
		{
			auto tv = transform * instance.model.vertices[i];
			verticies[i] = { tv.x, tv.y, tv.z };
		}
		// Clip each of the triangles (with transformed vertices) against each successive plane

		// Step 1.) Copy model triangles to vectors we will call "unclipped"
		std::vector<Triangle> unclipped_triangles{ instance.model.triangles };

		// Step 2.) Go through each of the clipping planes
		for (auto& clipping_plane : clipping_planes)
		{
			// Step 3.) Create empty vectors to hold the triangles after they are clipped
			std::vector<Triangle> clipped_traingles;

			// Step 4.) Go through each of the triangles (for the current clipping plane)
			for (auto& unclipped_triangle : unclipped_triangles)
			{
				// Step 5.) Add the clipped triangles to the clipped triangle vectors
				clip_triangle(clipping_plane, unclipped_triangle, verticies, clipped_traingles);
			}

			// Step 6.) The vectors now have triangles clipped relative to the current clipping plane.
			//          Copy them to the unclipped vectors because they have not yet been clipped relative
			//            to the next clipping plane.

			unclipped_triangles = std::vector<Triangle>{ clipped_traingles };

		}

		// Step 7.) There was not a next clipping plane, so the triangles that are in the "unclipped" vectors
		//            are actually fully clipped.  So, pass back a new model made up of the clipped triangles.
		return std::make_unique<Model>(Model{ verticies, unclipped_triangles });
	}

	void clip_triangle(const Plane& plane, const Triangle& triangle,
		std::vector<Vec3f>& vertices, std::vector<Triangle>& triangles) const
	{
		auto dist_from_plane_v1 = compute_dot_product(plane.normal,
			vertices[triangle.vertex_indices.x]) + plane.distance;
		auto dist_from_plane_v2 = compute_dot_product(plane.normal,
			vertices[triangle.vertex_indices.y]) + plane.distance;
		auto dist_from_plane_v3 = compute_dot_product(plane.normal,
			vertices[triangle.vertex_indices.z]) + plane.distance;

		auto count_in_plane = (dist_from_plane_v1 > 0 ? 1 : 0)
			+ (dist_from_plane_v2 > 0 ? 1 : 0)
			+ (dist_from_plane_v3 > 0 ? 1 : 0);

		// The triangle is fully in front of the plane.
		if (count_in_plane == 3)
		{
			triangles.push_back(triangle);
		}
		else if (count_in_plane == 1)
		{   // The triangle has one vertex in. Add one clipped triangle.

			// Set A to the vertex inside the frustrum and idx_a to it's index
			// Set B and C to the other two vertices
			auto a = vertices[triangle.vertex_indices.x];
			auto b = vertices[triangle.vertex_indices.y];
			auto c = vertices[triangle.vertex_indices.z];
			auto idx_a = triangle.vertex_indices.x;
			if (dist_from_plane_v2 > 0)
			{
				a = vertices[triangle.vertex_indices.y];
				b = vertices[triangle.vertex_indices.z];
				c = vertices[triangle.vertex_indices.x];
				idx_a = triangle.vertex_indices.y;
			}
			else if (dist_from_plane_v3 > 0)
			{
				a = vertices[triangle.vertex_indices.z];
				b = vertices[triangle.vertex_indices.x];
				c = vertices[triangle.vertex_indices.y];
				idx_a = triangle.vertex_indices.z;
			}

			// Create new vertices where AB and AC intersect the clipping plane
			auto new_b = compute_intersection(a, b, plane);
			auto new_c = compute_intersection(a, c, plane);

			// Add the new vertices to the vertices list and get their indexes
			vertices.push_back(new_b);
			vertices.push_back(new_c);
			auto idx_b = static_cast<int>(vertices.size()) - 2;
			auto idx_c = static_cast<int>(vertices.size()) - 1;

			// Add the new triangle made up of A, the new B, and the new C (and its color)
			triangles.push_back({ { idx_a, idx_b, idx_c }, triangle.color });
		}
		else if (count_in_plane == 2)
		{   // The triangle has two vertices in. Add two clipped triangles.

			// Set C to the vertex outside the frustrum
			// Set A and B to the other two vertices and idx_a and idx_b to their indices
			auto a = vertices[triangle.vertex_indices.x];
			auto b = vertices[triangle.vertex_indices.y];
			auto c = vertices[triangle.vertex_indices.z];
			auto idx_a = triangle.vertex_indices.x;
			auto idx_b = triangle.vertex_indices.y;
			if (dist_from_plane_v1 <= 0)
			{
				a = vertices[triangle.vertex_indices.y];
				b = vertices[triangle.vertex_indices.z];
				c = vertices[triangle.vertex_indices.x];
				idx_a = triangle.vertex_indices.y;
				idx_b = triangle.vertex_indices.z;
			}
			else if (dist_from_plane_v2 <= 0)
			{
				a = vertices[triangle.vertex_indices.z];
				b = vertices[triangle.vertex_indices.x];
				c = vertices[triangle.vertex_indices.y];
				idx_a = triangle.vertex_indices.z;
				idx_b = triangle.vertex_indices.x;
			}

			// Create new vertices where AC and BC intersect the clipping plane
			auto new_a = compute_intersection(a, c, plane);
			auto new_b = compute_intersection(b, c, plane);

			// Add the new vertices to the vertices list and get their indexes
			vertices.push_back(new_a);
			vertices.push_back(new_b);
			auto idx_new_a = static_cast<int>(vertices.size()) - 2;
			auto idx_new_b = static_cast<int>(vertices.size()) - 1;

			// Add the new triangle made up of A, B and the new A (and its color)
			triangles.push_back({ { idx_a, idx_b, idx_new_a }, triangle.color });

			// Add the new triangle made up of the new A, B and the new B (and its color)
			triangles.push_back({ { idx_new_a, idx_b, idx_new_b }, triangle.color });
		}
	}

	void draw_triangle_2d(Vec2i pt1, Vec2i pt2, Vec2i pt3, const Color& color) const
	{
		draw_line_2d(pt1, pt2, color);
		draw_line_2d(pt2, pt3, color);
		draw_line_2d(pt3, pt1, color);
	}

	void draw_triangle_2d_filled(Vec2i pt1, Vec2i pt2, Vec2i pt3, 
		float pt1z, float pt2z, float pt3z, const Color& color) 
	{
		// Sort the points from bottom to top.
		if (pt2.y < pt1.y)
		{
			std::swap(pt1, pt2);
			std::swap(pt1z, pt2z);
		}
			
		if (pt3.y < pt1.y)
		{
			std::swap(pt1, pt3);
			std::swap(pt1z, pt3z);
		}

		if (pt3.y < pt2.y)
		{
			std::swap(pt2, pt3);
			std::swap(pt2z, pt3z);
		}


		// Interpolate X coordinates from left to right for each y coordinate
		auto x_coords_for_each_y = interpolate_between_triangle_edges(
			pt1.y, static_cast<float>(pt1.x),
			pt2.y, static_cast<float>(pt2.x),
			pt3.y, static_cast<float>(pt3.x));

		// Interpolate inverse Z coordinates from left to right for each Y coordinate
		auto inv_z_coords_for_each_y = interpolate_between_triangle_edges(
			pt1.y, 1.0f / pt1z,
			pt2.y, 1.0f / pt2z,
			pt3.y, 1.0f / pt3z);

		// Draw horizontal segments
		for (auto y = pt1.y; y <= pt3.y; ++y)
		{
			auto idx_into_lists = y - pt1.y;

			// Get the (projected) x coordinates on the left and right for this y line
			auto x_start = static_cast<int>(x_coords_for_each_y.left[idx_into_lists]);
			auto x_stop = static_cast<int>(x_coords_for_each_y.right[idx_into_lists]);

			//Get the inverse z coordinates on the left and right for this y line
			auto zl = inv_z_coords_for_each_y.left[idx_into_lists];
			auto zr = inv_z_coords_for_each_y.right[idx_into_lists];

			//interpolate the inverse z values from x on the left to x on the right
			auto zscan = interpolate(x_start, zl, x_stop, zr);

			for (auto x = x_start; x <= x_stop; ++x)
			{
				if(check_and_update_depth_buffer(x,y,zscan[x - x_start]))
				put_pixel({ x, y }, color);
			}
		}
	}

	bool check_and_update_depth_buffer(int x, int y, float inverse_z) 
	{
		auto w = static_cast<int>(_width);
		auto h = static_cast<int>(_height);

		//convert the x and y coordinatesfrom (0,0) is center to (0,0) is top left
		x = w / 2 + x;
		y = h / 2 - y;

		//If we're out of bounds, don't display
		if (x < 0 || x >= w || y < 0 || y >= h)
		{
			return false;
		}

		auto offset = w * y + x;
		if (_depth_buffer[offset] < inverse_z)
		{
			_depth_buffer[offset] = inverse_z;
			return true;
		}
		return false;
	}

	void draw_line_2d(Vec2i pt1, Vec2i pt2, const Color& color) const
	{
		auto dx = pt2.x - pt1.x;
		auto dy = pt2.y - pt1.y;

		if (std::abs(dx) > std::abs(dy))
		{
			// The line is horizontal-ish. Make sure it's left to right.
			if (dx < 0)
				std::swap(pt1, pt2);

			// Compute the Y values and draw.
			auto ys = interpolate(
				pt1.x, static_cast<float>(pt1.y),
				pt2.x, static_cast<float>(pt2.y));

			for (int x = pt1.x; x <= pt2.x; ++x)
				put_pixel({ x, static_cast<int>(ys[x - pt1.x]) }, color);
		}
		else
		{
			// The line is verical-ish. Make sure it's bottom to top.
			if (dy < 0)
				std::swap(pt1, pt2);

			// Compute the X values and draw.
			auto xs = interpolate(
				pt1.y, static_cast<float>(pt1.x),
				pt2.y, static_cast<float>(pt2.x));

			for (int y = pt1.y; y <= pt2.y; ++y)
				put_pixel({ static_cast<int>(xs[y - pt1.y]), y }, color);
		}
	}

	Vec2i viewport_to_canvas(const Vec2f& pt) const
	{
		return {
			static_cast<int>(pt.x * static_cast<float>(_width) / viewport_size),
			static_cast<int>(pt.y * static_cast<float>(_height) / viewport_size) };
	}

	Vec2i project_vertex(const Vec3f& v) const
	{
		return viewport_to_canvas({
			v.x * projection_plane_z / v.z,
			v.y * projection_plane_z / v.z });
	}

	Vec2i project_vertex(const Vec4f& v) const
	{
		return viewport_to_canvas({
			v.x * projection_plane_z / v.z,
			v.y * projection_plane_z / v.z });
	}

	static std::vector<float> interpolate(int i0, float d0, int i1, float d1)
	{
		if (i0 == i1)
			return std::vector<float>{ d0 };

		auto values = std::vector<float>();
		auto a = (d1 - d0) / static_cast<float>(i1 - i0);
		auto d = d0;
		for (int i = i0; i <= i1; ++i)
		{
			values.push_back(d);
			d += a;
		}

		return values;
	}

	// Assumes: y0 > y1 < y2
	static CorrespondingCoordinateLists
		interpolate_between_triangle_edges(int y0, float v0, int y1, float v1, int y2, float v2)
	{
		// Compute dependent (X or Z in all cases as of now) coordinates of the edges.
		auto x01 = interpolate(y0, v0, y1, v1);
		auto x12 = interpolate(y1, v1, y2, v2);
		auto x02 = interpolate(y0, v0, y2, v2);

		// Merge the two short sides.
		x01.pop_back();
		x01.insert(x01.end(), x12.begin(), x12.end());

		auto m = x02.size() / 2;
		auto left = x01;
		auto right = x02;
		if (left[m] > right[m])
			std::swap(left, right);

		return { left, right };
	}

};
