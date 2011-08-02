#ifndef CINDER_BULLET
#define CINDER_BULLET

/*
*  CinderBullet originally created by Peter Holzkorn on 2/16/10
*
*/

// Cinder includes
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Vbo.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/CinderMath.h"
#include "cinder/Sphere.h"
#include "cinder/TriMesh.h"

// Bullet includes
#include "TetraCube.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBody.h"

namespace cinder 
{ 

	namespace bullet 
	{

		/****** TRANSFORM ******/

		/*! Convert Bullet matrix to Cinder matrix for rigid bodies */
		Matrix44f getWorldTransform(const btRigidBody * body);

		/*! Convert Bullet matrix to Cinder matrix for soft bodies */
		Matrix44f getWorldTransform(const btSoftBody * body);

		/*! Convert Cinder vector to Bullet vector */
		btVector3 toBulletVector3(const Vec3f & v);

		/*! Convert Bullet vector to Cinder vector */
		Vec3f fromBulletVector3(const btVector3 & v);

		/*! Convert Cinder quaternion to Bullet quaternion */
		btQuaternion toBulletQuaternion(const Quatf & q);

		/*! Convert Bullet quaternion to Cinder quaternion */
		Quatf fromBulletQuaternion(const btQuaternion & q);

		/****** MESH ******/

		/*! Create terrain from color channel */
		btHeightfieldTerrainShape * createHeightfieldTerrainShape(const Surface32f & heightField, int32_t stickWidth, int32_t stickLength, float heightScale, float minHeight, float maxHeight, int32_t upAxis, const Vec3f & scale);

		/*! Creates a concave Bullet mesh from a TriMesh */
		btBvhTriangleMeshShape * createConcaveMeshShape(const TriMesh & mesh, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f), float margin = 0.0f);

		/*! Creates a concave Bullet mesh from a list of vertices and indices */
		btBvhTriangleMeshShape * createConcaveMeshShape(const std::vector<Vec3f> & vertices, const std::vector<uint32_t> & indices, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f), float margin = 0.0f);

		/*! Creates a convex Bullet hull from a TriMesh */
		btConvexHullShape * createConvexHullShape(const TriMesh & mesh, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f));

		/*! Creates a convex Bullet hull from a list of vertices */
		btConvexHullShape * createConvexHullShape(const std::vector<Vec3f> & vertices, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f));

		/****** RIGID BODIES ******/

		/*! Creates rigid body from collision shape */
		btRigidBody * create(btDynamicsWorld * world, btCollisionShape * shape, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Create rigid body from triangle mesh */
		btRigidBody * create(btDynamicsWorld * world, btBvhTriangleMeshShape * shape, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates rigid body from convex hull shape */
		btRigidBody * create(btDynamicsWorld * world, btConvexHullShape * shape, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a rigid box */
		btRigidBody * create(btDynamicsWorld * world, const Vec3f & size = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a rigid cylinder */
		btRigidBody * create(btDynamicsWorld * world, float topRadius = 10.0f, float bottomRadius = 10.0f, float height = 20.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a rigid sphere */
		btRigidBody * create(btDynamicsWorld * world, float radius = 10.0f, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a rigid torus */
		btRigidBody * create(btDynamicsWorld * world, float innerRadius = 5.0f, float outerRadius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/****** SOFT BODIES ******/

		/*! Create soft body from arbitrary collision shape */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, btCollisionShape * shape, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates soft body from triangle mesh */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, btBvhTriangleMeshShape * shape, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates soft concave mesh */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, btConvexHullShape * shape, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Create a soft box from AxisAlignedBox3f */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, const AxisAlignedBox3f & box, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a soft box from two Rectfs */
		//btRigidBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, const Rectf & top, const Rectf & bottom, float height = 10.0f, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates soft box */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, const Vec3f & size = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a soft cylinder */
		//btRigidBody * create(btDynamicsWorld * world, float topRadius = 10.0f, float bottomRadius = 10.0f, float height = 20.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a soft sphere from Sphere */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, const Sphere & sphere, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a soft sphere */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, float radius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates soft tetra box */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, const TetraCube & tetraCube, const Vec3f & size = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

		/*! Creates a soft torus */
		//btSoftBody * create(btDynamicsWorld * world, btSoftBodyWorldInfo & info, float innerRadius = 5.0f, float outerRadius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

	} 

}

#endif
