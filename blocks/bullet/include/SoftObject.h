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

		/*! Soft object base class */
		class SoftObject : public CollisionObject
		{

		public:

			// Draw
			void draw(bool wireframe) const;

			// Getters
			btSoftBody * getBody();
			btSoftBody * getBody() const;
			int32_t getId();
			int32_t getId() const;
			btSoftBodyWorldInfo * getInfo();
			btSoftBodyWorldInfo * getInfo() const;
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
				btSoftBody * mBody;
				int32_t mId;
				btSoftBodyWorldInfo * mInfo;
				double mLifespan;
				double mLifetime;
				Vec3f mPosition;
				Quatf mRotation;
				gl::VboMesh::Layout mVboLayout;
				gl::VboMesh mVboMesh;
				btDynamicsWorld * mWorld;

			};

			// Pointer to object
			shared_ptr<Obj> mObj;

			// Con/de-structor
			SoftObject(DynamicsWorldRef world, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
			~SoftObject();

		};

		/*! Soft box */
		class SoftBox : public SoftObject 
		{
		public:
			SoftBox(DynamicsWorldRef world, const Vec3f & dimensions = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Soft sphere */
		class SoftSphere : public SoftObject 
		{
		public:
			SoftSphere(DynamicsWorldRef world, float radius = 10.0f, int32_t segments = 16, const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		/*! Soft tetra cube */
		class SoftTetraBox : public SoftObject 
		{
		public:
			SoftTetraBox(DynamicsWorldRef world, const Vec3f & dimensions = Vec3f(10.0f, 10.0f, 10.0f), const Vec3f & position = Vec3f::zero(), const Quatf & rotation = Quatf());
		};

		// Aliases
		typedef std::vector<SoftObject> SoftObjectList;
		typedef shared_ptr<SoftObject> SoftObjectRef;
		typedef std::vector<SoftObjectRef> SoftObjectRefList;

	}

}
