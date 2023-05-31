#pragma once

#include "../uav_math/uav_math.hpp"
#include <unordered_map>
#include <string>

class UAV {
private:
	float _mass;
	struct Matrix _inertia_matrix;

	struct Matrix _position;
	struct Matrix _velocity;
	struct Matrix _angular_velocity;
	struct Matrix _orientation;
	struct Matrix _orientation_euler_angles;

	struct Matrix _forces;
	struct Matrix _moments;


protected:
	std::unordered_map<std::string, float> _actuators;

public:
	UAV();
	virtual ~UAV() = 0;

	void update(float delta_t);

	struct Matrix get_position();

	struct Matrix get_velocity();

	struct Matrix get_angular_velocity();

	struct Matrix get_orientation_q();

	struct Matrix get_orientation_euler_angles_ZYX();

protected:
	virtual void control_update(float delta_t) = 0;

	virtual void forces_update(float delta_t) = 0;

	virtual void moments_update(float delta_t) = 0;

	void add_force(struct Matrix *force);

	void add_moment(struct Matrix *moment);

private:
	void physics_update(float delta_t);




};
