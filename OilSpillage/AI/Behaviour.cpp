#include "Behaviour.h"
/*  A sequence will visit each child in order,
starting with the first, and when that succeeds will call the second,
and so on down the list of children.
If any child fails it will immediately return failure to the parent.
If the last child in the sequence succeeds,
then the sequence will return success to its parent.
*/

void Sequence::addChildren(Behavior& child)
{
	children.emplace_back(&child);
}

Status Sequence::func()
{
	Status status = Status::INVALID;

	for (size_t i = 0; i < children.size() &&
		status != Status::FAILURE &&// if fail dont go further down
		status != Status::RUNNING; i++) //if Running, we are not done with the task til its succeeded
	{
		Selector* selector = dynamic_cast<Selector*>(children[i]);
		if (selector)
		{
			status = selector->func(); // safe to call
		}
		else
		{//we got a leaf

			status = children[i]->execute();
		}

	}
	return status;
}

Sequence::~Sequence()
{
}



/*Where a sequence is an AND,
requiring all children to succeed to return a success,
a selector will return a success if any of its children succeed and not process any further children.
It will process the first child, and if it fails will process the second,
and if that fails will process the third, until a success is reached,
at which point it will instantly return success. It will fail if all children fail.*/

void Selector::addChildren(Behavior& child)
{
	children.emplace_back(&child);

}

Status Selector::func()
{
	Status status = Status::INVALID;

	for (size_t i = 0; i < children.size() &&
		status != Status::SUCCESS &&// if we succeed lets return it
		status != Status::RUNNING; i++) //if Running, we are not done with the task til its succeeded
	{
		Sequence* sequence = dynamic_cast<Sequence*>(children[i]);
		if (sequence)
		{
			status = sequence->func(); // safe to call
		}
		else
		{//we got a leaf

			status = children[i]->execute();
		}

	}
	return status;
}

Selector::~Selector()
{
}

Sequence& BT::getSequence()
{
	Sequence* temp = new Sequence();
	sequence.emplace_back(temp);
	return *temp;
}

Selector& BT::getSelector()
{
	Selector* temp = new Selector();
	selector.emplace_back(temp);
	return *temp;
}

Behavior& BT::getAction()
{
	Behavior* temp = new Behavior();
	action.emplace_back(temp);
	return *temp;
}

BT::~BT()
{
	for (vector<Sequence*>::iterator pObj = sequence.begin();
		pObj != sequence.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}

	for (vector<Selector*>::iterator pObj = selector.begin();
		pObj != selector.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}

	for (vector<Behavior*>::iterator pObj = action.begin();
		pObj != action.end(); ++pObj) {
		delete* pObj; // Note that this is deleting what pObj points to,
					  // which is a pointer
	}
}

Behavior::Behavior()
{
}

Behavior::Behavior(ActionCallback func) :callback(func)
{
}

void Behavior::addAction(ActionCallback func)
{
	this->callback = func;
}

Status Behavior::execute()
{
	Status status;
	//check if its empty(no function stored)
	if (callback)
	{
		status = callback();
	}

	else
	{
		status = Status::FAILURE;
	}
	return status;

}
