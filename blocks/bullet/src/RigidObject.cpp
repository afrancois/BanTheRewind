// Include header
#include "RigidObject.h"
#include "boost/lexical_cast.hpp"

namespace cinder
{

	namespace bullet
	{

		// Constructor
		RigidObject::RigidObject(DynamicsWorldRef world, const Vec3f & position, const Quatf & rotation) 
		{

			// Define internal object
			mObj = shared_ptr<Obj>(new Obj());
			mObj->mWorld = world->getWorld();
			mObj->mPosition = position;
			mObj->mRotation = rotation;
			mObj->mLifespan = 0.0;
			mObj->mLifetime = 0.0;

		}

		// Destructor
		RigidObject::~RigidObject() 
		{
			mObj.reset();
		}

		// Internal object constructor
		RigidObject::Obj::Obj()
		{

			// Initialize VBO layout
			mVboLayout.setStaticIndices();
			mVboLayout.setStaticNormals();
			mVboLayout.setStaticPositions();

		}

		// Internal object destructor
		RigidObject::Obj::~Obj()
		{

			// Clean up
			if (mBody->getMotionState())
				delete mBody->getMotionState();
			mWorld->removeRigidBody(mBody);
			delete mBody;

		}

		// Clear VBO data
		void RigidObject::clearVboData()
		{

			// Clear vectors since we don't need them anymore
			mObj->mVboIndices.clear();
			mObj->mVboNormals.clear();
			mObj->mVboPositions.clear();

		}

		// Render object
		void RigidObject::draw(bool wireframe) const
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
		btRigidBody * RigidObject::getBody() { return mObj->mBody; }
		btRigidBody * RigidObject::getBody() const { return mObj->mBody; }
		int32_t RigidObject::getId() { return mObj->mId; }
		int32_t RigidObject::getId() const { return mObj->mId; }
		double RigidObject::getLifespan() { return mObj->mLifespan; }
		double RigidObject::getLifespan() const { return mObj->mLifespan; }
		double RigidObject::getLifetime() { return mObj->mLifetime; }
		double RigidObject::getLifetime() const { return mObj->mLifetime; }
		Vec3f RigidObject::getPosition() { return mObj->mPosition; }
		Vec3f RigidObject::getPosition() const { return mObj->mPosition; }
		Quatf RigidObject::getRotation() { return mObj->mRotation; }
		Quatf RigidObject::getRotation() const { return mObj->mRotation; }
		gl::VboMesh RigidObject::getVboMesh() { return mObj->mVboMesh; }
		gl::VboMesh RigidObject::getVboMesh() const { return mObj->mVboMesh; }
		btDynamicsWorld * RigidObject::getWorld() { return mObj->mWorld; }
		btDynamicsWorld * RigidObject::getWorld() const { return mObj->mWorld; }

		// Setters
		void RigidObject::setId(int32_t id) { mObj->mId = id; }
		void RigidObject::setLifespan(double time) { mObj->mLifespan = time; }

		// Runs update logic
		void RigidObject::update(double step)
		{

			// Add time step to lifetime
			mObj->mLifetime += step;

			// Update position and rotation
			mObj->mPosition = bullet::fromBulletVector3(mObj->mBody->getCenterOfMassPosition());
			mObj->mRotation = Quatf(bullet::getWorldTransform(mObj->mBody));

		}

		// Sets VBO data
		void RigidObject::setVboData(GLenum primitiveType)
		{

			// Set VBO data
			mObj->mVboMesh = gl::VboMesh(mObj->mVboPositions.size(), mObj->mVboIndices.size(), mObj->mVboLayout, primitiveType);
			mObj->mVboMesh.bufferIndices(mObj->mVboIndices);
			mObj->mVboMesh.bufferNormals(mObj->mVboNormals);
			mObj->mVboMesh.bufferPositions(mObj->mVboPositions);

		}

		/****** CONSTRUCTORS ******/

		// Box
		RigidBox::RigidBox(DynamicsWorldRef world, const Vec3f & dimensions, const Vec3f & position, const Quatf & rotation) 
			: RigidObject(world, position, rotation)
		{

			// Create body
			mObj->mBody = create(world->getWorld(), dimensions, position, rotation);

			// Define vertices
			Vec3f size = dimensions * 0.5f;
			mObj->mVboPositions.push_back(size * Vec3f(-1.0f, -1.0f, -1.0f)); // 0 ---
			mObj->mVboPositions.push_back(size * Vec3f( 1.0f, -1.0f, -1.0f)); // 1 +--
			mObj->mVboPositions.push_back(size * Vec3f(-1.0f,  1.0f, -1.0f)); // 2 -+-
			mObj->mVboPositions.push_back(size * Vec3f( 1.0f,  1.0f, -1.0f)); // 3 ++-
			mObj->mVboPositions.push_back(size * Vec3f(-1.0f, -1.0f,  1.0f)); // 4 --+
			mObj->mVboPositions.push_back(size * Vec3f( 1.0f, -1.0f,  1.0f)); // 5 +-+
			mObj->mVboPositions.push_back(size * Vec3f(-1.0f,  1.0f,  1.0f)); // 6 -++
			mObj->mVboPositions.push_back(size * Vec3f( 1.0f,  1.0f,  1.0f)); // 7 +++

			// Right
			mObj->mVboIndices.push_back(5);
			mObj->mVboIndices.push_back(7);
			mObj->mVboIndices.push_back(3);
			mObj->mVboIndices.push_back(1);

			// Top
			mObj->mVboIndices.push_back(6);
			mObj->mVboIndices.push_back(2);
			mObj->mVboIndices.push_back(3);
			mObj->mVboIndices.push_back(7);

			// Front
			mObj->mVboIndices.push_back(4);
			mObj->mVboIndices.push_back(6);
			mObj->mVboIndices.push_back(7);
			mObj->mVboIndices.push_back(5);

			// Left
			mObj->mVboIndices.push_back(0);
			mObj->mVboIndices.push_back(2);
			mObj->mVboIndices.push_back(6);
			mObj->mVboIndices.push_back(4);

			// Bottom
			mObj->mVboIndices.push_back(0);
			mObj->mVboIndices.push_back(4);
			mObj->mVboIndices.push_back(5);
			mObj->mVboIndices.push_back(1);

			// Back
			mObj->mVboIndices.push_back(1);
			mObj->mVboIndices.push_back(3);
			mObj->mVboIndices.push_back(2);
			mObj->mVboIndices.push_back(0);
			
			// Define normals
			Vec3f norm0(1.0f, 0.0f, 0.0f); // Right
			Vec3f norm1(0.0f, 1.0f, 0.0f); // Top
			Vec3f norm2(0.0f, 0.0f, 1.0f); // Front
			Vec3f norm3(-1.0f, 0.0f, 0.0f); // Left
			Vec3f norm4(0.0f, -1.0f, 0.0f); // Bottom
			Vec3f norm5(0.0f, 0.0f, -1.0f); // Back

			// Set normals
			for (int32_t i = 0; i < 4; i++)
				mObj->mVboNormals.push_back(norm0);
			for (int32_t i = 0; i < 4; i++)
				mObj->mVboNormals.push_back(norm1);
			for (int32_t i = 0; i < 4; i++)
				mObj->mVboNormals.push_back(norm2);
			for (int32_t i = 0; i < 4; i++)
				mObj->mVboNormals.push_back(norm3);
			for (int32_t i = 0; i < 4; i++)
				mObj->mVboNormals.push_back(norm4);
			for (int32_t i = 0; i < 4; i++)
				mObj->mVboNormals.push_back(norm5);

			// Set VBO data
			setVboData(GL_QUADS);
			clearVboData();

		}

		// Cylinder
		RigidCylinder::RigidCylinder(DynamicsWorldRef world, float topRadius, float bottomRadius, float height, int32_t segments, const Vec3f & position, const Quatf & rotation)
			: RigidObject(world, position, rotation)
		{

			// Create body
			mObj->mBody = create(world->getWorld(), topRadius, bottomRadius, height, segments, position, rotation);

			// Top center
			mObj->mVboPositions.push_back(Vec3f::zero());

			// Iterate phi, theta
			for (int32_t p = 0; p < 2; p++) 
				for (int32_t t = 1; t <= segments; t++) 
				{

					// Get coordinates
					float theta = (float)t / (float)segments;
					float cosTheta = cos(2.0f * (float)M_PI * theta);
					float sinTheta = sin(2.0f * (float)M_PI * theta);

					// Choose radius
					float radius = p == 0 ? bottomRadius : topRadius;

					// Set vertex and normal
					mObj->mVboPositions.push_back(Vec3f(cosTheta * radius, (float)p * height, sinTheta * radius));
					mObj->mVboNormals.push_back(Vec3f(cosTheta, 0.0f, sinTheta));

				}

			// Bottom center
			mObj->mVboPositions.push_back(Vec3f(0.0f, height, 0.0f));

			// Build top face
			for (int32_t t = 1; t <= segments; t++) 
			{
				mObj->mVboIndices.push_back(t);
				mObj->mVboIndices.push_back(0);
				mObj->mVboIndices.push_back(t + 1);
			}
			mObj->mVboIndices.push_back(segments);
			mObj->mVboIndices.push_back(0);
			mObj->mVboIndices.push_back(1);

			// Build body
			for (int32_t t = 1; t < segments; t++) 
			{
				mObj->mVboIndices.push_back(t);
				mObj->mVboIndices.push_back(segments + t);
				mObj->mVboIndices.push_back(t + 1);
				mObj->mVboIndices.push_back(t + 1);
				mObj->mVboIndices.push_back(segments + t);
				mObj->mVboIndices.push_back(segments + (t + 1));
			}
			
			// Build bottom face
			int32_t lastIndex = (int32_t)mObj->mVboPositions.size() - 1;
			for (int32_t t = segments + 1; t < segments * 2; t++) 
			{
				mObj->mVboIndices.push_back(t);
				mObj->mVboIndices.push_back(lastIndex);
				mObj->mVboIndices.push_back(t + 1);
			}
			mObj->mVboIndices.push_back(segments * 2);
			mObj->mVboIndices.push_back(lastIndex);
			mObj->mVboIndices.push_back(segments + 1);

			// Set VBO data
			setVboData();
			clearVboData();

		}

		// Convex hull mesh
		RigidHull::RigidHull(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale, const Vec3f & position, const Quatf & rotation)
			: RigidObject(world, position, rotation)
		{

			// Change positions to dynamic
			mObj->mVboLayout.setDynamicPositions();

			// Define body and VBO
			mObj->mBody = create(world->getWorld(), createConvexHullShape(mesh, scale), position, rotation);
			mObj->mVboMesh = gl::VboMesh(mesh, mObj->mVboLayout);

			// Scale VBO
			gl::VboMesh::VertexIter vertexIt = mObj->mVboMesh.mapVertexBuffer();
			for (uint32_t i = 0; i < mObj->mVboMesh.getNumVertices(); i++, ++vertexIt)
				vertexIt.setPosition(vertexIt.getPositionPointer()->xyz() * scale);

		}

		// Concave mesh
		RigidMesh::RigidMesh(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale, float margin, const Vec3f & position, const Quatf & rotation)
			: RigidObject(world, position, rotation)
		{

			// Change positions to dynamic so we can scale the VBO
			mObj->mVboLayout.setDynamicPositions();

			// Define body and VBO
			Vec3f halfScale = scale * 0.5f;
			mObj->mBody = create(world->getWorld(), createConcaveMeshShape(mesh, scale, margin), position, rotation);
			mObj->mVboMesh = gl::VboMesh(mesh, mObj->mVboLayout);

			// Scale VBO
			gl::VboMesh::VertexIter vertexIt = mObj->mVboMesh.mapVertexBuffer();
			for (uint32_t i = 0; i < mObj->mVboMesh.getNumVertices(); i++, ++vertexIt)
				vertexIt.setPosition(vertexIt.getPositionPointer()->xyz() * scale);

		}

		// Sphere
		RigidSphere::RigidSphere(DynamicsWorldRef world, float radius, int32_t segments, const Vec3f & position, const Quatf & rotation) 
			: RigidObject(world, position, rotation)
		{

			// Create body
			mObj->mBody = create(world->getWorld(), radius, position, rotation);

			// Define steps
			uint32_t phiSteps = segments / 2;
			uint32_t thetaSteps = segments;
			float phiStep = (float)M_PI / (float)phiSteps;
			float thetaStep = ((float)M_PI * 2.0f) / (float)thetaSteps;

			// Phi
			uint32_t p = 0;
			for (float phi = 0.0f; p <= phiSteps; p++, phi += phiStep)
			{

				// Theta
				uint32_t t = 0;
				for (float theta = thetaStep; t <= thetaSteps; t++, theta += thetaStep)
				{

					// Keep indices in range
					if (p * thetaSteps + t < phiSteps * (thetaSteps + 1))
					{

						// Add indices
						mObj->mVboIndices.push_back(p * thetaSteps + t);
						mObj->mVboIndices.push_back((p + 1) * thetaSteps + t);
						mObj->mVboIndices.push_back((p + 1) * thetaSteps + (t + 1));
						mObj->mVboIndices.push_back(p * thetaSteps + (t + 1));
						mObj->mVboIndices.push_back((p + 1) * thetaSteps + (t + 1));
						mObj->mVboIndices.push_back(p * thetaSteps + t);

					}

					// Set vertex position
					Vec3f vertex(
						radius * math<float>::sin(phi) * math<float>::cos(theta),
						radius * math<float>::sin(phi) * math<float>::sin(theta),
						-radius * math<float>::cos(phi));
					mObj->mVboPositions.push_back(vertex);

					// Set normal
					vertex.normalize();
					mObj->mVboNormals.push_back(vertex);

				}

			}

			// Set VBO data
			setVboData();
			clearVboData();

		}

		// Terrain from Surface data
		RigidTerrain::RigidTerrain(DynamicsWorldRef world, const Surface32f & heightField, int32_t stickWidth, int32_t stickLength, float minHeight, float maxHeight, int32_t upAxis, const Vec3f & scale, const Vec3f & position, const Quatf & rotation)
			: RigidObject(world, position, rotation)
		{

			// Create body
			mObj->mBody = create(world->getWorld(), createHeightfieldTerrainShape(heightField, stickWidth, stickLength, 1.0f, minHeight, maxHeight, 1, scale), position, rotation);

			// Set VBO positions from height map
			Surface32f::ConstIter pixelIt = heightField.getIter();
			while (pixelIt.line()) 
				while (pixelIt.pixel())
					mObj->mVboPositions.push_back(Vec3f((pixelIt.x() - heightField.getWidth() / 2.0f) * scale.x, (Color(pixelIt.r(), pixelIt.g(), pixelIt.b())).dot(Color(0.3333f, 0.3333f, 0.3333f)) * scale.y, (pixelIt.y() - heightField.getHeight() / 2.0f) * scale.x));

			// Set triangle indices
			for (int32_t x = 0; x < stickWidth; x++) 
				for (int32_t y = 0; y < stickLength; y++) 
					if (x + 1 < stickWidth && y + 1 < stickLength) 
					{
						mObj->mVboIndices.push_back((x + 0) * stickLength + (y + 0));
						mObj->mVboIndices.push_back((x + 1) * stickLength + (y + 0));
						mObj->mVboIndices.push_back((x + 1) * stickLength + (y + 1));
						mObj->mVboIndices.push_back((x + 0) * stickLength + (y + 1));
						mObj->mVboIndices.push_back((x + 1) * stickLength + (y + 1));
						mObj->mVboIndices.push_back((x + 0) * stickLength + (y + 0));
					}

			// Populate normals with 0's
			for (uint32_t i = 0; i < mObj->mVboPositions.size(); i++)
				mObj->mVboNormals.push_back(Vec3f::zero());

			// Iterate through buffer to calculate normals
			for (int32_t i = 0 ; i < (stickWidth - 1) * (stickLength - 1) * 2; i++) 
			{

				// Calculate normal
				Vec3f vert0 = mObj->mVboPositions[mObj->mVboIndices[i * 3]];
				Vec3f vert1 = mObj->mVboPositions[mObj->mVboIndices[i * 3 + 1]];
				Vec3f vert2 = mObj->mVboPositions[mObj->mVboIndices[i * 3 + 2]];
				Vec3f cross0 = vert2 - vert0;
				Vec3f cross1 = vert2 - vert1;
				Vec3f normal = cross0.cross(cross1).normalized();
				mObj->mVboNormals[mObj->mVboIndices[i * 3]] += normal;
				mObj->mVboNormals[mObj->mVboIndices[i * 3 + 1]] += normal;
				mObj->mVboNormals[mObj->mVboIndices[i * 3 + 2]] += normal;

			}

			// Normalize normals
			for (uint32_t i = 0; i < mObj->mVboNormals.size(); i++) 
				mObj->mVboNormals[i].normalize();

			// Set VBO data
			setVboData();
			clearVboData();

		}

		// Torus
		RigidTorus::RigidTorus(DynamicsWorldRef world, float innerRadius, float outerRadius, int32_t segments, const Vec3f & position, const Quatf & rotation) 
			: RigidObject(world, position, rotation)
		{

			// Create body
			mObj->mBody = create(world->getWorld(), innerRadius, outerRadius, segments, position, rotation);

			// Set VBO data
			//setVboData();
			//clearVboData();

		}

		/****** Convenience methods for creating object pointers ******/

		// Box from Vec3f
		RigidObjectRef createRigidBox(DynamicsWorldRef world, const Vec3f & dimensions, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidBox(world, dimensions, position, rotation));
		}

		// Cylinder
		RigidObjectRef createRigidCylinder(DynamicsWorldRef world, float topRadius, float bottomRadius, float height, int32_t segments, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidCylinder(world, topRadius, bottomRadius, height, segments, position, rotation));
		}

		// Hull
		RigidObjectRef createRigidHull(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidHull(world, mesh, scale, position, rotation));
		}

		// Mesh
		RigidObjectRef createRigidMesh(DynamicsWorldRef world, const TriMesh & mesh, const Vec3f & scale, float margin, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidMesh(world, mesh, scale, margin, position, rotation));
		}

		// Sphere
		RigidObjectRef createRigidSphere(DynamicsWorldRef world, float radius, int32_t segments, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidSphere(world, radius, segments, position, rotation));
		}

		// Terrain
		RigidObjectRef createRigidTerrain(DynamicsWorldRef world, const Surface32f & heightField, int32_t stickWidth, int32_t stickLength, float minHeight, float maxHeight, int32_t upAxis, const Vec3f & scale, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidTerrain(world, heightField, stickWidth, stickLength, minHeight, maxHeight, upAxis, scale, position, rotation));
		}

		// Torus
		RigidObjectRef createRigidTorus(DynamicsWorldRef world, float innerRadius, float outerRadius, int32_t segments, const Vec3f & position, const Quatf & rotation)
		{
			return RigidObjectRef(new RigidTorus(world, innerRadius, outerRadius, segments, position, rotation));
		}

	}

}
