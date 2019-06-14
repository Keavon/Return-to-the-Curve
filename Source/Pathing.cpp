#include "Pathing.h"

Pathing::Pathing() {
    targetPos = vec3(0);
}

Pathing::Pathing(vector<vec3> ctrlPts):
    controlPoints(ctrlPts)
{
    targetPos = vec3(0);
}

Pathing::~Pathing()
{
}

void Pathing::calcBezierCurveTarget(float t) {
    //printf("Calling Pathing with t: %f\n", t);
    int curveNum = static_cast<int>(t);
    if (curveNum*4 < controlPoints.size()) {
        targetPos.x = cubicBezierPoint(controlPoints[curveNum*4].x, controlPoints[curveNum*4 + 1].x, controlPoints[curveNum*4 + 2].x, controlPoints[curveNum*4 + 3].x, t);
        targetPos.y = cubicBezierPoint(controlPoints[curveNum*4].y, controlPoints[curveNum*4 + 1].y, controlPoints[curveNum*4 + 2].y, controlPoints[curveNum*4 + 3].y, t);
        targetPos.z = cubicBezierPoint(controlPoints[curveNum*4].z, controlPoints[curveNum*4 + 1].z, controlPoints[curveNum*4 + 2].z, controlPoints[curveNum*4 + 3].z, t);
    }
}

// Given control points and detail, returns a vector of target points for the object
std::vector<glm::vec3> Pathing::calcBezierPath(std::vector<glm::vec3> controls, float detail)
{
    std::vector<glm::vec3> renderPoints;
    if ( detail > 1 || detail < 0 )
    {
		
	}
	vector<vec3> renderingPoints = {};
	vector<vec3> controlPoints = {};
	//generate the end and control points

	for ( int i = 1; i < controls.size() - 1; i+=2 ){
		controlPoints.push_back(midPoint(controls[i-1], controls[i]));
		controlPoints.push_back(controls[i]);
		controlPoints.push_back(controls[i+1]);

		if ( i+2 < controls.size() - 1 ){

			controlPoints.push_back(midPoint(controls[i+1], controls[i+2]));

		}

	}
	//Generate the detailed points. 
	vec3 a0, a1, a2, a3;

	for ( int i = 0; i < controlPoints.size() - 2; i+=4 )
    {
		a0 = controlPoints.at(i);
		a1 = controlPoints.at(i+1);
		a2 = controlPoints.at(i+2);

		if ( i + 3 > controlPoints.size() - 1 )
        {
			//quad
			for ( double j = 0; j < 1; j += detail){
				renderingPoints.push_back(quadBezier(a0, a1, a2, (float)j));
			}
		}
        else
        {
			//cubic
			a3 = controlPoints.at(i+3);
			for ( double j = 0; j < 1; j += detail){
				renderingPoints.push_back(cubicBezier(a0, a1, a2, a3, (float)j));
			}
		}
	}

    return renderPoints;
}
void Pathing::calcCircPos(float theta, float radius, char axis, float axisValue) {
    if (axis == 'x'){
        targetPos.y = radius * cos(theta);
        targetPos.z = radius * sin(theta);
        targetPos.x = axisValue;
    }
    else if (axis == 'y'){
        targetPos.x = radius * cos(theta);
        targetPos.z = radius * sin(theta);
        targetPos.y = axisValue;
    }
    else if (axis == 'z'){
        targetPos.x = radius * cos(theta);
        targetPos.y = radius * sin(theta);
        targetPos.z = axisValue;
    }
}

std::vector<glm::vec3> Pathing::calcLinearPath(glm::vec3 point1, glm::vec3 point2){
    vector<vec3> linearPath;
    linearPath = {
        point1,
        vec3(point1.x + ((point2.x - point1.x)/3.0), 
             point1.y + ((point2.y - point1.y)/3.0), 
             point1.z + ((point2.z - point1.z)/ 3.0) ),
        vec3(point1.x + 2.0*((point2.x - point1.x)/3.0), 
             point1.y + 2.0*((point2.y - point1.y)/3.0), 
             point1.z + 2.0*((point2.z - point1.z) / 3.0)),
        point2
    };
    return linearPath;
}

glm::vec3 Pathing::getTargetPos() {
    return targetPos;
}
glm::vec3 Pathing::cubicBezier(vec3 v0, vec3 v1, vec3 v2, vec3 v3, float t)
{
    vec3 cubicBez = { 
        cubicBezierPoint(v0.x, v1.x, v2.x, v3.x, t),
        cubicBezierPoint(v0.y, v1.y, v2.y, v3.y, t),
        cubicBezierPoint(v0.z, v1.z, v2.z, v3.z, t)
    };
    return cubicBez;
}
glm::vec3 Pathing::quadBezier(vec3 v0, vec3 v1, vec3 v2, float t)
{
    vec3 quadBez = {
        quadBezierPoint(v0.x, v1.x, v2.x, t),
        quadBezierPoint(v0.y, v1.y, v2.y, t),
        quadBezierPoint(v0.z, v1.z, v2.z, t),
    };
    return quadBez;
}

float Pathing::quadBezierPoint(float p0, float p1, float p2, float t)
{
    return pow(1-t, 2) * p0 + 2*(1-t)*t*p1 + pow(t,2)*p2;
}

float Pathing::cubicBezierPoint(float p0, float p1, float p2, float p3, float t)
{

	return pow(1-t, 3) * p0 + 3*pow(1-t, 2) * t * p1 + 3*(1-t) * pow(t, 2) * p2 + pow(t, 3) * p3;
}

glm::vec3 Pathing::midPoint(glm::vec3 point1, glm::vec3 point2)
{
    vec3 midPt = { (point2.x + point1.x) / 2.0f,
                   (point2.y + point1.y) / 2.0f,
                   (point2.z + point1.z) / 2.0f };
    return midPt;
}