#pragma once
#include "Mat.h"
#include "Vec.h"
#include "Model.h"

class ModelInstance
{
public: 
	const Model& model;

	explicit ModelInstance(const Model& model,
		const Vec3f& translation = { 0, 0, 0 },
		float scale = 1,
		float rotation_angle = 0,
		const Vec3f& rotation_axis = { 1,0,0 })
		: model(model),
		_translation(translation),
		_scale(scale),
		_rotation_angle(rotation_angle),
		_rotation_axis(rotation_axis),
		_transform(Mat::get_identity_matrix())
	{
		compute_transform();
	}

	const Mat& get_transformation() const
	{
		return _transform;
	}

	const Vec3f& get_translation() const
	{
		return _translation;
	}

	void set_translation(const Vec3f& translation) 
	{
		_translation = translation;
		compute_transform();
	}

	float get_scale() const
	{
		return _scale;
	}

	float set_scale(float scale)
	{
		_scale = scale; 
		compute_transform();
	}

	float get_rotation_angle() const
	{
		return _rotation_angle;
	}

	const Vec3f& get_rotation_axis() const
	{
		return _rotation_axis;
	}

	void set_rotation(float rotation_angle, const Vec3f& rotation_axis)
	{
		_rotation_angle = rotation_angle;
		_rotation_axis = rotation_axis;
		compute_transform();
	}

private:
	Vec3f _translation;
	float _scale; 
	float _rotation_angle;
	Vec3f _rotation_axis;
	Mat	  _transform;

	void compute_transform() 
	{
		_transform = Mat::get_translation_matrix(_translation)
			* Mat::get_scale_matrix(_scale)
			* Mat::get_rotation_matrix(_rotation_angle, _rotation_axis);
	}
};