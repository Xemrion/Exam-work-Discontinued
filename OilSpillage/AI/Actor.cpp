#include "Actor.h"
#include "..//game.h"
void Actor::SetType(AItype type)
{
	this->type = type;
}

Actor::AItype& Actor::GetType()
{
	return this->type;
}

void AIPlayer::Update(float dt)
{
	//TODO: Call findPath every X frames
	nrOfFrames++;
	if (nrOfFrames % 100 == 0)
	{
		//findPath();
		nrOfFrames = 0;
	}
	followPath(dt);
	if (path.size() == 0)
	{
		if (counter == 0)
		{
			setTargetPos(Vector3(9, 0, -9));
		}
		if (counter == 1)
		{
			setTargetPos(Vector3(-9, 0, -9));
		}
		if (counter == 2)
		{
			setTargetPos(Vector3(0, 0, 0));
		}
		findPath();
		counter++;

	}
}

void AIPlayer::setTargetPos(Vector3 targetPos)
{
	this->targetPos = targetPos;
}

void AIPlayer::findPath()
{
	if (state == AIState::chasing)
	{
		path = aStar->algorithm(position, targetPos);
	}
}

void AIPlayer::followPath(float dt)
{
	if (path.size() > 0)
	{
		targetNode = DirectX::SimpleMath::Vector3(path.at(path.size() - 1)->GetXPos(), 0, path.at(path.size() - 1)->GetYPos());
		Vector3 dir = targetNode - position;
		dir.Normalize();
		Vector3 newPosition = position + dir * dt;
		for (int i = 0; i < boids.size(); i++) //Updating Boids
		{
			boids.at(i)->setDestination(targetNode);
			boids.at(i)->run(boids, dt);
		}

		if (newPosition.Distance(targetNode, newPosition) < 1)
		{
			path.pop_back();
		}

		this->setPosition(newPosition);

	}
}
AIPlayer::AIPlayer()
{
	aStar = new AStar(20, 20,DirectX::SimpleMath::Vector2(-10,10));
	setPosition(DirectX::SimpleMath::Vector3(-10, 0, 10));
	setTargetPos(DirectX::SimpleMath::Vector3(9,0,9));
	state = AIState::chasing;
	findPath();
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			//boids.push_back(new Boid(i, j));
		}

	}
	for (int i = 0; i < boids.size(); i++)
	{
		boids.at(i)->mesh = Game::getGraphics().getMeshPointer("Cube");
		Game::getGraphics().addToDraw(boids.at(i));
		boids.at(i)->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		boids.at(i)->setScale(Vector3(0.5f, 0.5f, 0.5f));
	}

}

AIPlayer::~AIPlayer()
{
	delete aStar;
	for (int i = 0; i < boids.size(); i++)
	{
		delete boids.at(i);
	}
	boids.clear();
}
