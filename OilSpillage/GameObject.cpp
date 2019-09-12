#include"GameObject.h"
Matrix GameObject::getTransform() {

	Quaternion qt = Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	Matrix mtr = Matrix::CreateFromQuaternion(qt);
	Matrix translate = Matrix::CreateTranslation(position);
	Matrix scaleM = Matrix::CreateScale(scale);
	return   mtr * scaleM * translate;

};

void GameObject::setPosition(Vector3 newPos)
{
	this->position = newPos;
};

void GameObject::move(Vector3 addPos)
{
	this->position += addPos;
};

void GameObject::addRotation(Vector3 addRotaiton)
{
	this->rotation += addRotaiton;
	this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
	this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
	this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);

};

void GameObject::setRotation(Vector3 newRotation)
{
	this->rotation = newRotation;
	this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
	this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
	this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);
};

void GameObject::setScale(Vector3 newScale)
{
	this->scale = newScale;
};

Texture* GameObject::getTexture()
{
	return this->texture;
};

void GameObject::setTexture(Texture* aTexture)
{
	this->texture = aTexture;
};

void GameObject::setColor(Vector4 aColor)
{
	this->color = aColor;
};

Vector4 GameObject::getColor()const
{
	return this->color;
};