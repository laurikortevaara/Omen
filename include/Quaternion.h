#pragma once
#include <math.h>
#include <glm/gtc/quaternion.hpp>
/**
	rotation around x-axis = pitch = attitude
	rotation around y-axis = yaw = heading
	rotation around z-axis = roll = bank
*/

namespace omen 
{
	class Quaternion
	{
	public:
		Quaternion(float x, float y, float z, float w) : qx(x), qy(y), qz(z), qw(w) {};
		static Quaternion fromEulerAngles(float yaw, float pitch, float roll) {
			// Assuming the angles are in radians.
			double c1 = cos(yaw);
			double s1 = sin(yaw);
			double c2 = cos(pitch);
			double s2 = sin(pitch);
			double c3 = cos(roll);
			double s3 = sin(roll);
			double w = sqrt(1.0 + c1 * c2 + c1*c3 - s1 * s2 * s3 + c2*c3) / 2.0;
			double w4 = (4.0 * w);
			double x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4;
			double y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4;
			double z = (-s1 * s3 + c1 * s2 * c3 + s2) / w4;
			return Quaternion(x, y, z, w);
		};
	protected:
	private:
		float qx, qy, qz, qw;
		
	};
}
