#pragma once

#include "../uav_math/uav_math.hpp"
#include "sensor/sensor.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include "../data_link/data_link.hpp"

class UAV {
	private:
		float _mass;
		struct Matrix _inertia_matrix;
		struct Matrix _inverse_inertia_matrix;

		struct Matrix _position;
		struct Matrix _position_geodetic;
		struct Matrix _velocity;
		struct Matrix _angular_velocity;
		struct Matrix _orientation;
		struct Matrix _orientation_euler_angles;

		struct Matrix _forces;
		struct Matrix _moments;


		float _start_lat;
		float _start_lon;
		float _start_alt;

		float _lat;
		float _lon;
		float _alt;


	protected:
		DataLink *_data_link;

		std::unordered_map<std::string, float> _actuators;
		std::vector<Sensor*> _sensors;

	public:
		UAV();
		virtual ~UAV() = 0;

		void update(uint64_t t, float delta_t);

		virtual void communication_thread() = 0;

		virtual void communication_non_blocking_thread() = 0;

		float get_mass();

		struct Matrix get_position();

		struct Matrix get_position_geodetic();

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

		void sensors_update(float delta_t);

		void update_position(float delta_t);

		void update_velocity(float delta_t);

		void update_orientation(float delta_t);

		void update_angular_velocity(float delta_t);
};
