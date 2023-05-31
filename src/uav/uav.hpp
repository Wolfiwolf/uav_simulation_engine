#pragma once

#include "../uav_math/uav_math.hpp"

class UAV {
private:
	float _mass;
	struct Matrix _inertia_matrix;

	struct Matrix _position;
	struct Matrix _velocity;
	struct Matrix _angular_velocity;
	struct Matrix _orientation;

	struct Matrix _forces;
	struct Matrix _moments;

public:
	virtual ~UAV() = default;

	void init();

	void update(float delta_t);


protected:
	virtual void specific_init() = 0;

	virtual void control_update(float delta_t) = 0;

	virtual void forces_update(float delta_t) = 0;

	virtual void moments_update(float delta_t) = 0;

	void add_force(struct Matrix *force);

	void add_moment(struct Matrix *moment);

private:
	void physics_update(float delta_t);




};
