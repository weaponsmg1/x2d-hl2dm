#pragma once

struct Vector3 { float x, y, z; };

bool WorldToScreen(Vector3 pos, Vector3& screen, float matrix[16], int windowWidth, int windowHeight)
{
	float clip_w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clip_w < 0.01f) return false;

	float clip_x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	float clip_y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];

	float ndc_x = clip_x / clip_w;
	float ndc_y = clip_y / clip_w;

	screen.x = (windowWidth / 2 * ndc_x) + (ndc_x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * ndc_y) + (ndc_y + windowHeight / 2);
	screen.z = clip_w;

	return true;
}