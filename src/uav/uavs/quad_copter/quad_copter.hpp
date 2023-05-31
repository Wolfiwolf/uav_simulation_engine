#include "../../uav.hpp"

class QuadCopter : public UAV {
	private:
		float _power;


	public:
		QuadCopter();
		~QuadCopter();

	protected:
		void control_update(float delta_t);

		void forces_update(float delta_t);

		void moments_update(float delta_t);
};
