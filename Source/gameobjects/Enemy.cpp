#include "Enemy.h"

Enemy::Enemy(vector<vec3> enemyPath, quat orientation, shared_ptr<Shape> model, shared_ptr<Shape> legmodel, shared_ptr<Shape> footmodel, float radius) : PhysicsObject(enemyPath[0], orientation, vec3(1, 1, 1), model, make_shared<ColliderSphere>(radius)), radius(radius), legModel(legmodel), footModel(footmodel)
{
	if (enemyPath.size() < 2)
	{
		sentry = true;
		ballInRange = false;
		sentryIdlePath = curvePath->calcLinearPath( vec3(enemyPath[0].x, enemyPath[0].y + 3.0f, enemyPath[0].z) , 
													vec3(enemyPath[0].x + 6.0f, enemyPath[0].y + 3.0f, enemyPath[0].z)); 
		curvePath = new Pathing(sentryIdlePath);
		sentryHome = sentryIdlePath[0];
		state = 0;
	}
	else {
		sentry = false;
		ballInRange = false;
		state = 3;
		for (glm::vec3 pathVect : enemyPath)
		{
			defaultPath.push_back(vec3(pathVect.x, pathVect.y + 3.0f, pathVect.z));
		}
		curvePath = new Pathing(defaultPath);
	}
    speed = 0;
    material = 0;

	moveSpeed = 8;
	t = 0.1f;
	k = 0.1f;
	acceleration = vec3(0, 0, 0);
	direction = vec3(0);

	forward = true;
	pointReached = true;
}

void Enemy::init(WindowManager *windowManager)
{
	this->windowManager = windowManager;
}

void Enemy::update(vec3 ballPosition)
{
	clearCollisions();
	if (sentry)
	{
		//cout << "State : " << state << endl;
		if (distance(sentryHome, ballPosition) < 20)
		{
			ballInRange = true;
			//cout<< "Ball In Range" << endl;
			state = 1;
		}
		else{
			if (ballInRange)
			{
				sentryPathHome = curvePath->calcLinearPath(position, sentryHome);
				curvePath = new Pathing(sentryPathHome);
				state = 2;
				t = 0.1;
			}
			ballInRange = false;
		}
	}
	if (state == 1){ // Follow Player
		//cout << "Following Player" << endl; 
		sentryFollowPath = curvePath->calcLinearPath(position, vec3(ballPosition.x, ballPosition.y + 3.0f, ballPosition.z));
		curvePath = new Pathing(sentryFollowPath);
		t = 0.1;
	}
    if (pointReached) {
		curvePath->calcBezierCurveTarget(t);
		targetX = curvePath->getTargetPos().x;
		targetY = curvePath->getTargetPos().y;
		targetZ = curvePath->getTargetPos().z;
		if (t < 0)
		{
			forward = true;
			//printf("Switch to forward\n");
		}
        if (t > 1) 
		{
			if (state == 2){
				state = 0;
				curvePath = new Pathing(sentryIdlePath);
				t = 0.1;
			}
            forward = false;
			//printf("Switch to backward\n");
		}
		if (forward)
		{
			t += 0.02f;
			//printf("Incremented t to : %f\n", t);
		}
		else
		{
			t -= 0.02f;
			//printf("Decremented t to : %f\n", t);
		}
		pointReached = false;
	}
	float dX = targetX - position.x;
	float dZ = targetZ - position.z;
	float dY = targetY - position.y;
	direction = normalize(vec3(dX, dY, dZ));
	velocity.x = 0;
	velocity.z = 0;
	velocity.y = 0;
	//vec3 axis = vec3{0,1,0};
	//quat q = rotate(, axis);
	//orientation = q * orientation;
	velocity = direction * moveSpeed;
	//printf("Velocity: %f, %f, %f", velocity.x,velocity.y,velocity.z);
	position += velocity * Time.physicsDeltaTime;
	//printf("Position of Enemy: (%f,%f,%f)\n", position.x,position.y,position.z);
	if (sqrt(pow((targetX - position.x), 2) +
			 pow((targetZ - position.z), 2)) < 1.02)
	{
		pointReached = true;
	}
}

void Enemy::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	if (!inView && cull)
	{
		return;
	}

	shared_ptr<MatrixStack> M2 = M;
	{
		k += Time.deltaTime / 2;
		quat r = rotate(quat(1, 0, 0, 0), 90.0f, vec3(1, 0, 0));
		MatrixStack BaseMat;
		MatrixStack uLeg1;
		MatrixStack lLeg1;
		MatrixStack Foot1;
		MatrixStack uLeg2;
		MatrixStack lLeg2;
		MatrixStack Foot2;
		MatrixStack uLeg3;
		MatrixStack lLeg3;
		MatrixStack Foot3;
		MatrixStack uLeg4;
		MatrixStack lLeg4;
		MatrixStack Foot4;
		M2 = M;
		scale = vec3(2, 2, 2);
		;
		M->pushMatrix();
		M->translate(position - vec3(0, 2.5, 0));
		M->translate(vec3(0, (3.5 + sin(k * 4) / 3.0f), 0));
		M->rotate(orientation);
		M->scale(scale);
		BaseMat = *M;
		M->rotate(rotate(quat(1, 0, 0, 0), t, vec3(0, 1, 0)));
		//M->rotate(quat(1, 0, 1, 0));
		//M->rotate(quat(sqrt(0.5), sqrt(0.5), 0, 0));
		//M->rotate(quat(sqrt(0.5), 0, 0, sqrt(0.5)));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		model->draw(prog);
		uLeg1 = BaseMat;
		uLeg1.scale(vec3(0.5f, 0.5f, 0.5f));
		uLeg1.translate(vec3(0.7f, -2, 0.7f));
		uLeg1.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 1.25f, vec3(0, 1, 0)));
		uLeg1.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -3, vec3(1, 0, 0)));
		uLeg1.rotate(rotate(quat(1, 0, 0, 0), sin(k * 8) / 2, vec3(1, 0, 0)));
		uLeg1.translate(vec3(0, 0, -0.8f));
		lLeg1 = setlLeg(uLeg1, 0, k);
		//lLeg1 = uLeg1;
		//lLeg1.translate(vec3(0, 0, -0.9f));
		//lLeg1.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.625f, vec3(1, 0, 0)));
		//lLeg1.rotate(rotate(quat(1, 0, 0, 0), cos(t*8)/2, vec3(1, 0, 0)));
		//lLeg1.translate(vec3(0, 0, -0.9f));
		Foot1 = setFoot(lLeg1, 0, k);
		//Foot1 = lLeg1;
		//Foot1.translate(vec3(0, 0, -0.9f));
		//Foot1.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -4, vec3(1, 0, 0)));
		//Foot1.rotate(rotate(quat(1, 0, 0, 0), sin(t*8)/4, vec3(1, 0, 0)));
		//Foot1.translate(vec3(0, -0.9f, 0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(uLeg1.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(lLeg1.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Foot1.topMatrix()));
		footModel->draw(prog);
		uLeg2 = BaseMat;
		uLeg2.scale(vec3(0.5f, 0.5f, 0.5f));
		uLeg2.translate(vec3(-0.7f, -2.0f, 0.7f));
		uLeg2.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.75f, vec3(0, 1, 0)));
		uLeg2.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -3, vec3(1, 0, 0)));
		uLeg2.rotate(rotate(quat(1, 0, 0, 0), sin((k * 8) + (float)M_PI_2) / 2, vec3(1, 0, 0)));
		uLeg2.translate(vec3(0, 0, -0.8f));
		lLeg2 = setlLeg(uLeg2, (float)M_PI_2, k);
		//lLeg2.translate(vec3(0, 0, -0.9f));
		//lLeg2.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.625f, vec3(1, 0, 0)));
		//lLeg2.rotate(rotate(quat(1, 0, 0, 0), cos((t * 8) + (float)M_PI_2) / 2, vec3(1, 0, 0)));
		//lLeg2.translate(vec3(0, 0, -0.9f));
		Foot2 = setFoot(lLeg2, (float)M_PI_2, k);
		//Foot2.translate(vec3(0, 0, -0.9f));
		//Foot2.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -4, vec3(1, 0, 0)));
		//Foot2.rotate(rotate(quat(1, 0, 0, 0), sin((t * 8) + (float)M_PI_2)/4, vec3(1, 0, 0)));
		//Foot2.translate(vec3(0, -0.9f, 0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(uLeg2.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(lLeg2.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Foot2.topMatrix()));
		footModel->draw(prog);
		uLeg3 = BaseMat;
		uLeg3.scale(vec3(0.5f, 0.5f, 0.5f));
		uLeg3.translate(vec3(-0.7f, -2, -0.7f));
		uLeg3.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.25f, vec3(0, 1, 0)));
		uLeg3.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -3, vec3(1, 0, 0)));
		uLeg3.rotate(rotate(quat(1, 0, 0, 0), sin(k * 8) / 2, vec3(1, 0, 0)));
		uLeg3.translate(vec3(0, 0, -0.8f));
		lLeg3 = setlLeg(uLeg3, 0, k);
		//lLeg3.translate(vec3(0, 0, -0.9f));
		//lLeg3.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.625f, vec3(1, 0, 0)));
		//lLeg1.rotate(rotate(quat(1, 0, 0, 0), cos(t * 8) / 2, vec3(1, 0, 0)));
		//lLeg3.translate(vec3(0, 0, -0.9f));
		Foot3 = setFoot(lLeg3, 0, k);
		//Foot3.translate(vec3(0, 0, -0.9f));
		//Foot3.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -4, vec3(1, 0, 0)));
		//Foot3.rotate(rotate(quat(1, 0, 0, 0), sin(t * 8)/4, vec3(1, 0, 0)));
		//Foot3.translate(vec3(0, -0.9f, 0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(uLeg3.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(lLeg3.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Foot3.topMatrix()));
		footModel->draw(prog);
		uLeg4 = BaseMat;
		uLeg4.scale(vec3(0.5f, 0.5f, 0.5f));
		uLeg4.translate(vec3(0.7f, -2.0f, -0.7f));
		uLeg4.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 1.75f, vec3(0, 1, 0)));
		uLeg4.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -3, vec3(1, 0, 0)));
		uLeg4.rotate(rotate(quat(1, 0, 0, 0), sin((k * 8) + (float)M_PI_2) / 2, vec3(1, 0, 0)));
		uLeg4.translate(vec3(0, 0, -0.8f));
		lLeg4 = setlLeg(uLeg4, (float)M_PI_2, k);
		//lLeg4.translate(vec3(0, 0, -0.9f));
		//lLeg4.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.625f, vec3(1, 0, 0)));
		//lLeg4.rotate(rotate(quat(1, 0, 0, 0), cos((t * 8) + (float)M_PI_2) / 2, vec3(1, 0, 0)));
		//lLeg4.translate(vec3(0, 0, -0.9f));
		Foot4 = setFoot(lLeg4, (float)M_PI_2, k);
		//Foot4.translate(vec3(0, 0, -0.9f));
		//Foot4.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -4, vec3(1, 0, 0)));
		//Foot4.rotate(rotate(quat(1, 0, 0, 0), sin((t * 8) + (float)M_PI_2)/4, vec3(1, 0, 0)));
		//Foot4.translate(vec3(0, -0.9f, 0));
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(uLeg4.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(lLeg4.topMatrix()));
		legModel->draw(prog);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Foot4.topMatrix()));
		footModel->draw(prog);
		M->popMatrix();
		/*scale = vec3(7, 0.5, 7);
		M->pushMatrix();
		M->translate(position);
		M->rotate(orientation);
		M->scale(scale);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		model->draw(prog);
		M->popMatrix();*/
	}
}

MatrixStack Enemy::setlLeg(MatrixStack uLeg, float offset, float k)
{
	MatrixStack lLeg = uLeg;
	lLeg.translate(vec3(0, 0, -0.9f));
	lLeg.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI * 0.625f, vec3(1, 0, 0)));
	lLeg.rotate(rotate(quat(1, 0, 0, 0), cos((k * 8) + offset) / 2, vec3(1, 0, 0)));
	lLeg.translate(vec3(0, 0, -0.9f));
	return lLeg;
}

MatrixStack Enemy::setFoot(MatrixStack lLeg, float offset, float t)
{
	MatrixStack foot = lLeg;
	foot.translate(vec3(0, 0, -0.9f));
	foot.rotate(rotate(quat(1, 0, 0, 0), (float)M_PI / -4, vec3(1, 0, 0)));
	foot.rotate(rotate(quat(1, 0, 0, 0), sin((k * 8) + offset) / 4, vec3(1, 0, 0)));
	foot.translate(vec3(0, -0.9f, 0));
	return foot;
}