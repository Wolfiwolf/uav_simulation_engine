#include "../../uav.hpp"

class QuadCopter : public UAV {
	public:
		QuadCopter();
		~QuadCopter();

	protected:
		void specific_init();

		void control_update(float delta_t);

		void forces_update(float delta_t);

		void moments_update(float delta_t);
};
