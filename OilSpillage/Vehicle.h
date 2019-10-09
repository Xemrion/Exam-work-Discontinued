#ifndef VEHICLE_H
#define VEHICLE_H

#include "GameObject.h"
#include "VehicleWeapon.h"
#include "VehicleStats.h"

using namespace DirectX::SimpleMath;

class Vehicle
{
private:
	GameObject* vehicle;
	GameObject* bodyRotation;
	GameObject* bodyRotationPoint;

	int health;

	static const int bulletCount = 16;
	float leftoverTime;
	Weapon weapon;
	Stats defaultStats;
	Stats updatedStats;

	struct Bullet
	{
		Vector3 dir;
		float speed = 0.0f;
		float timeLeft = 0.0f;
		GameObject* obj = nullptr;
	} bullets[bulletCount];

	Vector3 bodyPivot;
	DirectX::XMFLOAT2 velocity;
	float strength;
	float add;

	DirectX::XMFLOAT3 accelerator;
	float acceleratorTempX;
	float acceleratorTempZ;

	float targetRotation;
	int drivingMode;
	float topSpeed;
	float counter = 0.0f;
	float rotateAcceleration = 0.0f;
	float rotationSmoother = 1.0f;
	float tempTargetRotation;
	bool rotationDirection;


	Vector2 currentDir;
	float velocitySimple;
	float velocitySpeed;
public:
	Vehicle();
	virtual ~Vehicle();

	void init();
	void update(float deltaTime);
	GameObject* getVehicle() { return this->vehicle; }
	float getAcceleratorX();

	void setDrivingMode(int i);
	bool getDrivingMode();
	Vector3 getVelocity();
	float getVelocitySpeed();
	//void onCollision(Vector2 direction);

	const int& getHealthRef() const;
	int getHealth() const;
	int getMaxHealth() const;
	void setHealth(int health);
	void setMaxHealth(int maxHealth);
	void resetHealth();
	void changeHealth(int amount);
	bool isDead() const;
};

#endif // !VEHICLE_H
