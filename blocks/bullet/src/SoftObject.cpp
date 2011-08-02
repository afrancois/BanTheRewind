// Include header
#include "SoftObject.h"

namespace cinder
{

	namespace bullet
	{

		// Constructor
		SoftObject::SoftObject(DynamicsWorldRef world, const Vec3f & position, const Quatf & rotation) 
		{

			// Define internal object
			mObj = shared_ptr<Obj>(new Obj());
			mObj->mInfo = & (world->getInfo());
			mObj->mRotation = rotation;
			mObj->mWorld = world->getWorld();
			mObj->mLifespan = 0.0;
			mObj->mLifetime = 0.0;

		}

		// Destructor
		SoftObject::~SoftObject() 
		{
			mObj.reset();
		}

		// Internal object constructor
		SoftObject::Obj::Obj()
		{

			// Initialize VBO layout
			mVboLayout.setStaticIndices();
			mVboLayout.setDynamicNormals();
			mVboLayout.setDynamicPositions();

		}

		// Internal object destructor
		SoftObject::Obj::~Obj()
		{

			// Clean up
			((btSoftRigidDynamicsWorld *)mWorld)->removeSoftBody(mBody);
			delete mBody;

		}

		// Draw
		void SoftObject::draw(bool wireframe) const
		{

			// Draw VBO mesh
			glPushMatrix();
			if (wireframe)
				gl::enableWireframe();
			glMultMatrixf(bullet::getWorldTransform(mObj->mBody).m);
			gl::draw(mObj->mVboMesh);
			if (wireframe)
				gl::disableWireframe();
			glPopMatrix();

		}

		// Getters
		btSoftBody * SoftObject::getBody() { return mObj->mBody; }
		btSoftBody * SoftObject::getBody() const { return mObj->mBody; }
		int32_t SoftObject::getId() { return mObj->mId; }
		int32_t SoftObject::getId() const { return mObj->mId; }
		btSoftBodyWorldInfo * SoftObject::getInfo() { return mObj->mInfo; }
		btSoftBodyWorldInfo * SoftObject::getInfo() const { return mObj->mInfo; }
		double SoftObject::getLifespan() { return mObj->mLifespan; }
		double SoftObject::getLifespan() const { return mObj->mLifespan; }
		double SoftObject::getLifetime() { return mObj->mLifetime; }
		double SoftObject::getLifetime() const { return mObj->mLifetime; }
		Vec3f SoftObject::getPosition() { return mObj->mPosition; }
		Vec3f SoftObject::getPosition() const { return mObj->mPosition; }
		Quatf SoftObject::getRotation() { return mObj->mRotation; }
		Quatf SoftObject::getRotation() const { return mObj->mRotation; }
		gl::VboMesh SoftObject::getVboMesh() { return mObj->mVboMesh; }
		gl::VboMesh SoftObject::getVboMesh() const { return mObj->mVboMesh; }
		btDynamicsWorld * SoftObject::getWorld() { return mObj->mWorld; }
		btDynamicsWorld * SoftObject::getWorld() const { return mObj->mWorld; }

		// Setters
		void SoftObject::setId(int32_t id) { mObj->mId = id; }
		void SoftObject::setLifespan(double time) { mObj->mLifespan = time; }

		// Runs update logic
		void SoftObject::update(double step)
		{

			// Add time step to lifetime
			mObj->mLifetime += step;
		
			// Update position and rotation
			mObj->mPosition = bullet::fromBulletVector3(mObj->mBody->m_bounds[0].lerp(mObj->mBody->m_bounds[1], 0.5f));
			mObj->mRotation = Quatf(bullet::getWorldTransform(mObj->mBody));

			// TO DO update VBO mesh

		}

		/****** CONSTRUCTORS ******/

		// Soft box
		SoftBox::SoftBox(DynamicsWorldRef world, const Vec3f & dimensions, const Vec3f & position, const Quatf & rotation) 
			: SoftObject(world, position, rotation)
		{

			// Set object dimensions
			//mObj->mBody = bullet::create(world->getWorld(), world->getInfo(), dimensions, position, rotation);

			// TO DO create VBO mesh

		}

		// Soft sphere
		SoftSphere::SoftSphere(DynamicsWorldRef world, float radius, int32_t segments, const Vec3f & position, const Quatf & rotation) 
			: SoftObject(world, position, rotation)
		{

			// Set object dimensions
			//mObj->mBody = bullet::create(world->getWorld(), world->getInfo(), radius, segments, position, rotation);

			// TO DO create VBO mesh

		}

		// Soft tetra cube
		SoftTetraBox::SoftTetraBox(DynamicsWorldRef world, const Vec3f & dimensions, const Vec3f & position, const Quatf & rotation) 
			: SoftObject(world, position, rotation)
		{

			// Set object dimensions
			//mObj->mBody = bullet::createTetraBox(world->getWorld(), world->getInfo(), dimensions, position, rotation);

			// TO DO create VBO mesh

		}

	}

}
