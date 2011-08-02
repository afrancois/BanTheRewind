// Include header
#include "DynamicsWorld.h"
#include "RigidObject.h"

namespace cinder
{

	namespace bullet
	{

		// Convenience method for creating world pointer
		DynamicsWorldRef createWorld() 
		{ 
			return DynamicsWorldRef(new DynamicsWorld()); 
		}

		// Constructor
		DynamicsWorld::DynamicsWorld()
		{

			// Setup physics environment
			mCollisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
			mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
			mBroadphase = new btDbvtBroadphase();
			mSolver = new btSequentialImpulseConstraintSolver();
	
			// Soft body dynamics
			mSoftBodyWorldInfo.air_density = (btScalar)1.2f;
			mSoftBodyWorldInfo.m_broadphase = mBroadphase;
			mSoftBodyWorldInfo.m_dispatcher = mDispatcher;
			mSoftBodyWorldInfo.m_gravity.setValue(0, -10, 0);
			mSoftBodyWorldInfo.water_density = 0;
			mSoftBodyWorldInfo.water_offset = 0;
			mSoftBodyWorldInfo.water_normal = btVector3(0, 0, 0);
			mSoftBodyWorldInfo.m_sparsesdf.Initialize();

			// Build world
			mDynamicsWorld = new btSoftRigidDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
			mDynamicsWorld->setGravity(btVector3(0, -10, 0));
			mDynamicsWorld->getDispatchInfo().m_enableSPU = true;

			// Set timeline
			mElapsedSeconds = 0.0;

		}

		// Destructor
		DynamicsWorld::~DynamicsWorld()
		{

			// Delete Bullet objects
			delete mDynamicsWorld;
			delete mSolver;
			delete mBroadphase;
			delete mDispatcher;
			delete mCollisionConfiguration;

		}

		// Draws everything
		void DynamicsWorld::draw(bool wireframe)
		{

			// Draw all items
			int32_t i = 0;
			for (CollisionObjectRefList::const_iterator objectIt = mObjects.cbegin(); objectIt != mObjects.cend(); ++objectIt, i++)
				if (!wireframe || (wireframe && i > 0))
					(* objectIt)->draw(wireframe);

		}

		// Erase object
		void DynamicsWorld::erase(const CollisionObjectRefList::iterator & object)
		{

			// DO IT!
			mObjects.erase(object);

		}

		// Erase an item using its index
		void DynamicsWorld::erase(uint32_t index)
		{
			if (index > 0 && index < mObjects.size())
				mObjects.erase(mObjects.begin() + index);
		}

		// Erase an item using its ID
		bool DynamicsWorld::eraseById(int32_t id)
		{

			// Find and erase item
			int32_t i = 0;
			for (CollisionObjectRefList::const_iterator objectIt = mObjects.cbegin(); objectIt != mObjects.cend(); ++objectIt, i++)
				if ((* objectIt)->getId() == id)
				{
					erase(i);
					return true;
				}
			return false;

		}

		// Getters
		btDynamicsWorld * DynamicsWorld::getWorld() { return mDynamicsWorld; }
		btBroadphaseInterface * DynamicsWorld::getBroadphase() { return mBroadphase; }
		btCollisionDispatcher * DynamicsWorld::getDispatcher() { return mDispatcher; }
		btSoftBodyWorldInfo DynamicsWorld::getInfo() { return mSoftBodyWorldInfo; }
		CollisionObjectRefList DynamicsWorld::getObjects() { return mObjects; }
		btConstraintSolver * DynamicsWorld::getSolver() { return mSolver; }
		btSoftBodyRigidBodyCollisionConfiguration * DynamicsWorld::getCollisionConfiguration() { return mCollisionConfiguration; }

		// Get object from index
		CollisionObjectRef DynamicsWorld::getObject(uint32_t index) 
		{ 
			if (index > 0 && index < mObjects.size())
				return mObjects[index];
			else
				return 0;
		}

		// Retrieve object using its ID
		CollisionObjectRef DynamicsWorld::getObjectById(int32_t id)
		{
			for (CollisionObjectRefList::const_iterator objectIt = mObjects.cbegin(); objectIt != mObjects.cend(); ++objectIt)
				if ((* objectIt)->getId() == id)
					return (* objectIt);
			return 0;
		}

		// Add an object
		CollisionObjectRef DynamicsWorld::insert(const CollisionObjectRef & object)
		{

			// Assign ID
			int32_t id = -1;
			for (CollisionObjectRefList::const_iterator objectIt = mObjects.cbegin(); objectIt != mObjects.cend(); ++objectIt)
				if ((* objectIt)->getId() > id)
					id = (* objectIt)->getId();
			object->setId(id + 1);

			// Add and return object
			mObjects.push_back(object);
			return object;

		}

		// Handles mouse down
		void DynamicsWorld::mouseDown(const Vec2i & position)
		{

			// TO DO

		}

		// Handles mouse drag
		void DynamicsWorld::mouseDrag(const Vec2i & position)
		{

			// TO DO

		}

		// Converts screen coordinates to Bullet world
		btVector3 DynamicsWorld::screenToWorld(const Vec2i & position)
		{

			// TO DO math
			return btVector3(0, 0, 0);

		}

		// Set soft body info
		void DynamicsWorld::setInfo(const btSoftBodyWorldInfo & info)
		{

			// DO IT!
			mSoftBodyWorldInfo = info;

		}

		// Runs update logic
		void DynamicsWorld::update()
		{

			// Get time since last frame, update
			double elapsedSeconds = getElapsedSeconds();
			double step = elapsedSeconds - mElapsedSeconds;
			mElapsedSeconds = elapsedSeconds;

			// Update Bullet world
			mDynamicsWorld->stepSimulation(1.0f, 10, 1.0f / math<float>::max(1.0f, getFrameRate()));

			// Get object counts
			uint32_t numObjects = mObjects.size();

			// Update bodies
			for (uint32_t i = 0; i < mObjects.size(); i++)
			{
				mObjects[i]->update(step);
				if (mObjects[i]->getLifespan() > 0.0 && mObjects[i]->getLifetime() > mObjects[i]->getLifespan())
					mObjects.erase(mObjects.begin() + i);
			}

			// Object count has changed
			if (numObjects != mObjects.size())
			{
	
				// Iterate through objects
				numObjects = mDynamicsWorld->getNumCollisionObjects();
				for (int32_t i = (int32_t)numObjects - 1; i >= 0 ; i--)
				{

					// Check range
					if (i >= 0)
					{

						// Activate object if it is a rigid or soft body
						btCollisionObject * collisionObject = mDynamicsWorld->getCollisionObjectArray()[i];
						btRigidBody * rigidBody = btRigidBody::upcast(collisionObject);
						if (rigidBody)
						{
							rigidBody->activate(true);
						}
						else
						{
							btSoftBody * softBody = btSoftBody::upcast(collisionObject);
							if (softBody)
								softBody->activate(true);
						}

					}

				}

			}

		}

	}

}
