#include "Physics.h"
#include "Vehicle.h"
#include "AI/Actor.h"

Physics::Physics() :broadphase(new btDbvtBroadphase()),
collisionConfig(new btDefaultCollisionConfiguration()),
solver(new btSequentialImpulseConstraintSolver)
{
	
	dispatcher = new btCollisionDispatcher(collisionConfig);
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	this->world->setGravity(btVector3(0, -10, 0));
	//temp plane inf
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));


	////set shape for object
	plane = new btStaticPlaneShape(btVector3(0, 1, 0), -1.5f);
	
	//set motionshape aka set postion
	btMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	////body definition check doc
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);

	btRigidBody* body = new btRigidBody(info);
	body->setFriction(1);

	world->addRigidBody(body);
	bodies.push_back(body);

	gContactAddedCallback = callbackFunc;
}


Physics::~Physics()
{
	//os�ker p� om man deletar p� detta s�tt
	for (int i = 0; i < bodies.size(); i++)
	{
		btMotionState* motionState = bodies[i]->getMotionState();
		btCollisionShape* shape = bodies[i]->getCollisionShape();
		delete shape;
		delete motionState;
		this->world->removeRigidBody(bodies[i]);
		delete bodies[i];
	}
	for (int i = 0; i < pointJoints.size(); i++)
	{
		delete pointJoints[i];
	}
	delete dispatcher;
	delete collisionConfig;
	delete solver;
	delete broadphase;
	delete world;
	bodies.clear();

}

void Physics::update(float deltaTime)
{
	//this->world->stepSimulation(deltaTime);
	this->world->stepSimulation(deltaTime, 2, 1. / 120.);
	//this->world->stepSimulation(deltaTime, 0);
	//this->world->stepSimulation(btScalar(deltaTime));
}   

btRigidBody* Physics::addSphere(float radius, btVector3 Origin, float mass, void* obj)
{	//add object set transform
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btSphereShape * sphere = new btSphereShape(radius); //raduius

	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		sphere->calculateLocalInertia(mass, inertia);
	}

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);

	btRigidBody* body = new btRigidBody(info);

	this->world->addRigidBody(body);
	bodies.push_back(body);
	body->setUserPointer(obj);
	return body;
}

btRigidBody* Physics::addBox(btVector3 Origin, btVector3 size,float mass, void* objects)
{
	//add object set transform
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btBoxShape* box = new btBoxShape(size);
	

	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		box->calculateLocalInertia(mass, inertia);
	}
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	this->world->addRigidBody(body);
	bodies.push_back(body);
	body->setUserPointer(objects);
	return body;
}

btRigidBody* Physics::addCylinder(btVector3 Origin, btVector3 size, float mass)
{
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btCylinderShape* cylinder = new btCylinderShape(size); //raduius

	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		cylinder->calculateLocalInertia(mass, inertia);
	}

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, cylinder, inertia);

	btRigidBody* body = new btRigidBody(info);

	this->world->addRigidBody(body);
	bodies.push_back(body);
	return body;
}

btGeneric6DofSpring2Constraint* Physics::addSpring(btRigidBody* box1, btRigidBody* box2)
{
	btGeneric6DofSpring2Constraint* spring = new btGeneric6DofSpring2Constraint(
		*box1, *box2,
		btTransform(btQuaternion::getIdentity(), { 0.0f, -1.5f, 0.0f }),
		btTransform(btQuaternion::getIdentity(), { 0.0f,  0.0f, 0.0f }));
	this->world->addConstraint(spring);
	springs.push_back(spring);

	return spring;
}

btPoint2PointConstraint* Physics::addPointJoint(btRigidBody* box1, btRigidBody* box2)
{
	btPoint2PointConstraint* pointJoint = new btPoint2PointConstraint(*box1,*box2,btVector3(0,0.00f,0),btVector3(0, -0.55f, 0));
	//pointJoint->enableFeedback(true);
	/*btJointFeedback* hej = new btJointFeedback;
	hej->m_appliedForceBodyA = btVector3(1, 0, 1);
	pointJoint->setJointFeedback(hej);*/
	this->world->addConstraint(pointJoint);
	pointJoints.push_back(pointJoint);

	return pointJoint;
}

btRaycastVehicle* Physics::addVehicle(btRaycastVehicle* vehicle)
{
	this->world->addVehicle(vehicle);
	vehicles.push_back(vehicle);
	return vehicle;
}

bool Physics::DeleteRigidBody(btRigidBody * rb)
{
	
		for (int i = 0; i < this->bodies.size(); i++)
		{
			if (bodies[i] == rb)
			{
				this->world->removeRigidBody(bodies[i]);
				return true;
			}
		}
	return false;
}
bool Physics::deletePointJoint(btPoint2PointConstraint* pointJoint)
{
	for (int i = 0; i < this->pointJoints.size(); i++)
	{
		if (pointJoints[i] == pointJoint)
		{
			this->world->removeConstraint(pointJoint);
			return true;
		}
	}
	return false;
}

bool Physics::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, 
	int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	Vehicle* playerPtr = static_cast<Vehicle*>(obj1->getCollisionObject()->getUserPointer());
	Actor* enemyPtr = static_cast<Actor*>(obj2->getCollisionObject()->getUserPointer());
	
	if (playerPtr == nullptr || enemyPtr == nullptr ? true : !playerPtr->isPlayer())
	{
		playerPtr = static_cast<Vehicle*>(obj2->getCollisionObject()->getUserPointer());
		enemyPtr = static_cast<Actor*>(obj1->getCollisionObject()->getUserPointer());

		if (playerPtr == nullptr || enemyPtr == nullptr ? true : !playerPtr->isPlayer())
		{
			return false;
		}
	}

	if (enemyPtr != nullptr)
	{
		if (playerPtr->getPowerUpTimer(PowerUpType::Star) > 0.0)
		{
			enemyPtr->setHealth(0);
		}

		return true;
	}
	return false;
}

btStaticPlaneShape* Physics::getPlaneRigidBody()
{
	return plane;
}
