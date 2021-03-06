#include "Entity.h"

Entity::Entity(int graphicsKey)
{
	this->mPos = this->mRot = this->mOffsetRot =
		DirectX::SimpleMath::Vector3(0, 0, 0);
	this->mAABB = nullptr;
	this->solid = false;

	this->mGraphicsKey = graphicsKey;
}

Entity::Entity(int graphicsKey, AABB *aabb, bool solid) {
	this->mPos = this->mRot = this->mOffsetRot =
		DirectX::SimpleMath::Vector3(0, 0, 0);
	this->mAABB = aabb;
	this->solid = solid;

	this->mGraphicsKey = graphicsKey;
}

Entity::~Entity()
{
	if (this->mAABB)
		delete this->mAABB;
}

void Entity::move(DirectX::SimpleMath::Vector3 offset)
{
	this->mPos += offset;
}

void Entity::createAABB(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 U, DirectX::SimpleMath::Vector3 V, DirectX::SimpleMath::Vector3 W)
{
	if (this->mAABB != nullptr)
		delete this->mAABB;

	this->mAABB = new AABB(pos, U, V, W);
}

void Entity::setPosition(DirectX::SimpleMath::Vector3 pos)
{
	this->mPos = pos;
}

void Entity::setRotation(DirectX::SimpleMath::Vector3 rot) {
	this->mRot = rot;
}

void Entity::setScale(DirectX::SimpleMath::Vector3 scale) {
	this->mScale = scale;
}

void Entity::setOffsetRotation(DirectX::SimpleMath::Vector3 offsetRot) {
	this->mOffsetRot = offsetRot;
}

DirectX::SimpleMath::Vector3 Entity::getPosition() const
{
	return this->mPos;
}

AABB* Entity::getAABB()
{
	return this->mAABB;
}

void Entity::updateTransformBuffer(ID3D11DeviceContext* context, GraphicsData* gData)
{
	using namespace DirectX::SimpleMath;
	Matrix offsetMat = Matrix::CreateTranslation(this->mOffsetRot);
	Matrix posMat = offsetMat * Matrix::CreateRotationZ(mRot.z) * Matrix::CreateRotationX(mRot.x) *
					Matrix::CreateRotationY(mRot.y) * offsetMat.Invert() * Matrix::CreateScale(mScale) *
		Matrix::CreateTranslation(this->mPos);
	posMat = posMat.Transpose();

	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(D3D11_MAPPED_SUBRESOURCE));

	context->Map(gData->getConstantBuffer(this->mGraphicsKey), 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

	memcpy(data.pData, &posMat, sizeof(Matrix));

	context->Unmap(gData->getConstantBuffer(this->mGraphicsKey), 0);
}

int Entity::getKey() const
{
	return this->mGraphicsKey;
}

bool Entity::isSolid() const {
	return solid;
}

DirectX::SimpleMath::Matrix Entity::getTransformation() const {
	return DirectX::SimpleMath::Matrix::CreateTranslation(mPos);
}

DirectX::SimpleMath::Matrix Entity::getScale() const {
	return DirectX::SimpleMath::Matrix::CreateScale(mScale);
}