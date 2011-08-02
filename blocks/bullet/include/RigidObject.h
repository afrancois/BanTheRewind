/*
* 
* Copyright (c) 2011, Ban the Rewind
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or 
* without modification, are permitted provided that the following 
* conditions are met:
* 
* Redistributions of source code must retain the above copyright 
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright 
* notice, this list of conditions and the following disclaimer in 
* the documentation and/or other materials provided with the 
* distribution.
* 
* Neither the name of the Ban the Rewind nor the names of its 
* contributors may be used to endorse or promote products 
* derived from this software without specific prior written 
* permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
*/

#pragma once

// Includes
#include "CollisionObject.h"
#include "DynamicsWorld.h"
#include "BulletUtils.h"

// Imports
using namespace ci;
using namespace ci::app;
using namespace std;

namespace cinder
{

	namespace bullet
	{

		/*! Rigid object base class */
		class RigidObject : public CollisionObject
		{

		public:

			// Draw
			void draw(bool wireframe) const;

			// Getters
			btRigidBody * getBody();
			btRigidBody * getBody() const;
			int32_t getId();
			int32_t getId() const;
			double getLifespan();
			double getLifespan() const;
			double getLifetime();
			double getLifetime() const;
			Vec3f getPosition();
			Vec3f getPosition() const;
			Quatf getRotation();
			Quatf getRotation() const;
			gl::VboMesh getVboMesh();
			gl::VboMesh getVboMesh() const;
			btDynamicsWorld * getWorld();
			btDynamicsWorld * getWorld() const;

			// Setters
			void setId(int32_t id);
			void setLifespan(double time);

			// Runs update logic
			void update(double step);

		protected:

			// Internal object
			class Obj
			{

			public:

				// Con/de-structor
				Obj();
				~Obj();

				// Properties
				btRigidBody * mBody;
				int32_t mId;
				double mLifespan;
				double mLifetime;
				Vec3f mPosition;
				Quatf mRotation;
				vector<uint32_t> mVboIndices;
				gl::VboMesh::Layout mVboLayout;
				gl::VboMesh mVboMesh;
				vector<Vec3f> mVboNormals;
				vector<Vec3f> mVboPositions;
				btDynamicsWorld * mWorld;

			};

			// Pointer to object
			shared_ptr<Obj> mObj;

			// Con/de-structor
			RigidObject(DynamicsWorldRef world, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
			~RigidObject();

			// Set, clear VBO data
			void setVboData(GLenum primitiveType = GL_TRIANGLES);
			void clearVboData();

		};

		/*! Rigid box */
		class RigidBox : public RigidObject 
		{
		public:
			RigidBox(DynamicsWorldRef world, const Vec3f & dimensions = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Rigid cylinder */
		class RigidCylinder : public RigidObject 
		{
		public:
			RigidCylinder(DynamicsWorldRef world, float topRadius = 10.0f, float bottomRadius = 10.0f, float height = 20.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Rigid hull */
		class RigidHull : public RigidObject 
		{
		public:
			RigidHull(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		// Rigid mesh */
		class RigidMesh : public RigidObject 
		{
		public:
			RigidMesh(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f), float margin = 0.05f, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Rigid sphere */
		class RigidSphere : public RigidObject 
		{
		public:
			RigidSphere(DynamicsWorldRef world, float radius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Rigid terrain */
		class RigidTerrain : public RigidObject 
		{
		public:
			RigidTerrain(DynamicsWorldRef world, const Surface32f & heightField, int32_t stickWidth, int32_t stickLength, float minHeight = -200.0f, float maxHeight = 200.0f, int32_t upAxis = 1, const Vec3f & scale = Vec3f(1.0f, 100.0f, 1.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Rigid torus */
		class RigidTorus : public RigidObject 
		{
		public:
			RigidTorus(DynamicsWorldRef world, float innerRadius = 5.0f, float outerRadius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		// Aliases
		typedef std::vector<RigidObject> RigidObjectList;
		typedef shared_ptr<RigidObject> RigidObjectRef;
		typedef std::vector<RigidObjectRef> RigidObjectRefList;

		// Convenience methods for creating object pointers
		RigidObjectRef createRigidBox(DynamicsWorldRef world, const Vec3f & dimensions = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		RigidObjectRef createRigidCylinder(DynamicsWorldRef world, float topRadius = 10.0f, float bottomRadius = 10.0f, float height = 20.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		RigidObjectRef createRigidHull(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		RigidObjectRef createRigidMesh(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale = Vec3f(1.0f, 1.0f, 1.0f), float margin = 0.05f, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		RigidObjectRef createRigidSphere(DynamicsWorldRef world, float radius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		RigidObjectRef createRigidTerrain(DynamicsWorldRef world, const Surface32f & heightField, int32_t stickWidth, int32_t stickLength, float minHeight = -200.0f, float maxHeight = 200.0f, int32_t upAxis = 1, const Vec3f & scale = Vec3f(1.0f, 100.0f, 1.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		RigidObjectRef createRigidTorus(DynamicsWorldRef world, float innerRadius = 5.0f, float outerRadius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());

	}

}
