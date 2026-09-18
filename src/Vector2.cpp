#include "Vector2.h"

using namespace Tepeyo;

void Vector2::normalize()
{
	int12 Norm = norm();
	x = div12(x, Norm);
	y = div12(y, Norm);
}

int12 Vector2::norm()
{
	return sqrt12(squaredNorm());
}

int12 Vector2::squaredNorm()
{
	return add12(mul12(x, x), mul12(y, y));
}
