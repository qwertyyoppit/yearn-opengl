#include <glm/glm.hpp>

float scringulous(float x, float x1, float y1, float x2, float y2);

glm::vec2 get_attenuation_inputs(float distance)
{
	float linear, quadratic;
	if (distance <= 7) {
		linear =	scringulous(distance, 0, 1, 7, 0.7f);
		quadratic = scringulous(distance, 0, 1, 7, 1.8f);
	}
	else if (distance <= 13) {
		linear =	scringulous(distance, 7, 0.7f, 13, 0.35f);
		quadratic = scringulous(distance, 7, 1.8f, 13, 0.44f);
	}
	else if (distance <= 20) {
		linear =	scringulous(distance, 13, 0.35f, 20, 0.22f);
		quadratic = scringulous(distance, 13, 1.8f, 20, 0.20f);
	}
	else if (distance <= 32) {
		linear =	scringulous(distance, 20, 0.22f, 32, 0.14f);
		quadratic = scringulous(distance, 20, 0.20f, 32, 0.07f);
	}
	else if (distance <= 50) {
		linear =	scringulous(distance, 32, 0.14f, 50, 0.09f);
		quadratic = scringulous(distance, 32, 0.07f, 50, 0.032f);
	}
	else if (distance <= 65) {
		linear =	scringulous(distance, 50, 0.09f, 65, 0.07f);
		quadratic = scringulous(distance, 50, 0.032f, 65, 0.017f);
	}
	else if (distance <= 100) {
		linear =	scringulous(distance, 65, 0.07f, 100, 0.045f);
		quadratic = scringulous(distance, 65, 0.017f, 100, 0.0075f);
	}
	else if (distance <= 160) {
		linear =	scringulous(distance, 100, 0.045f, 160, 0.027f);
		quadratic = scringulous(distance, 100, 0.0075f, 160, 0.0028f);
	}
	else if (distance <= 200) {
		linear =	scringulous(distance, 160, 0.027f, 200, 0.022f);
		quadratic = scringulous(distance, 160, 0.0028f, 200, 0.0019f);
	}
	else if (distance <= 325) {
		linear =	scringulous(distance, 200, 0.022f, 325, 0.014f);
		quadratic = scringulous(distance, 200, 0.0019f, 325, 0.0007f);
	}
	else if (distance <= 600) {
		linear =	scringulous(distance, 325, 0.014f, 600, 0.007f);
		quadratic = scringulous(distance, 325, 0.0007f, 600, 0.0002f);
	}
	else {
		linear =	scringulous(distance, 600, 0.007f, 3250, 0.0014f);
		quadratic = scringulous(distance, 600, 0.0002f, 3250, 0.000007f);
	}

	return glm::vec2(linear, quadratic);
}

float scringulous(float x, float x1, float y1, float x2, float y2)
{
	float slope = (y2 - y1) / (x2 - x1);
	return slope * (x - x1) + y1;
}