#include <glm/glm.hpp>

glm::vec3 hsv_to_rgb(glm::vec3 hsv)
{
	float H = fmodf(hsv.x, 360);
	float S = fmin(hsv.y, 1);
	float V = fmin(hsv.z, 1);
	float C = V * S;

	float Ht = H / 60;
	float X = C * (1 - abs(fmodf(Ht, 2) - 1));

	glm::vec3 rgb1 = glm::vec3(0.0f);
	if		(Ht < 1) rgb1 = glm::vec3(C, X, 0);
	else if (Ht < 2) rgb1 = glm::vec3(X, C, 0);
	else if (Ht < 3) rgb1 = glm::vec3(0, C, X);
	else if (Ht < 4) rgb1 = glm::vec3(0, X, C);
	else if (Ht < 5) rgb1 = glm::vec3(X, 0, C);
	else if (Ht < 6) rgb1 = glm::vec3(C, 0, X);

	float m = V - C;
	glm::vec3 rgb = glm::vec3(rgb1.x + m, rgb1.y + m, rgb1.z + m);
	return rgb;
}