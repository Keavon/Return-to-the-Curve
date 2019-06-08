
#include "MatrixStack.h"

static const int MaxMatrixSize = 100;

MatrixStack::MatrixStack()
{
	stack.push(mat4(1.0));
}

void MatrixStack::pushMatrix()
{
	const mat4 &top = stack.top();
	stack.push(top);
	assert(stack.size() < MaxMatrixSize);
}

void MatrixStack::popMatrix()
{
	assert(!stack.empty());
	stack.pop();

	// There should always be one matrix left.
	assert(!stack.empty());
}

void MatrixStack::loadIdentity()
{
	mat4 &top = stack.top();
	top = mat4(1.f);
}

 void MatrixStack::perspective(float fovy, float aspect, float zNear, float zFar)
{
	mat4 &top = stack.top();
	top *= glm::perspective(fovy, aspect, zNear, zFar);
}

void MatrixStack::translate(const vec3 &offset)
{
	mat4 &top = stack.top();
	mat4 t = glm::translate(mat4(1.f), offset);
	top *= t;
}

void MatrixStack::scale(const vec3 &scaleV)
{
	mat4 &top = stack.top();
	mat4 s = glm::scale(mat4(1.f), scaleV);
	top *= s;
}

void MatrixStack::scale(float size)
{
	mat4 &top = stack.top();
	mat4 s = glm::scale(mat4(1.f), vec3(size));
	top *= s;
}

void MatrixStack::rotate(float angle, const vec3 &axis)
{
	mat4 &top = stack.top();
	mat4 r = glm::rotate(mat4(1.0), angle, axis);
	top *= r;
}

void MatrixStack::rotate(quat q)
{
	mat4 &top = stack.top();
	mat4 r = mat4_cast(q);
	top *= r;
}

void MatrixStack::multMatrix(const mat4 &matrix)
{
	mat4 &top = stack.top();
	top *= matrix;
}

void MatrixStack::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	assert(left != right);
	assert(bottom != top);
	assert(zFar != zNear);

	mat4 &ctm = stack.top();
	ctm *= glm::ortho(left, right, bottom, top, zNear, zFar);
}

void MatrixStack::frustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mat4 &ctm = stack.top();
	ctm *= glm::frustum(left, right, bottom, top, zNear, zFar);
}

void MatrixStack::lookAt(const vec3 &eye, const vec3 &target, const vec3 &up)
{
	mat4 &top = stack.top();
	top *= glm::lookAt(eye, target, up);
}

const mat4 &MatrixStack::topMatrix() const
{
	return stack.top();
}

void MatrixStack::print(const mat4 &mat, const char *name)
{
	if (name)
	{
		printf("%s = [\n", name);
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			printf("%- 5.2f ", mat[i][j]);
		}
		printf("\n");
	}

	if (name)
	{
		printf("];");
	}
	printf("\n");
}

void MatrixStack::print(const char *name) const
{
	print(stack.top(), name);
}
