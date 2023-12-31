#include "Capsule.h"
#include "Cylinder.h"
#include "Sphere.h"
#include <MMath.h>
using namespace MATH;
	
GEOMETRY::Capsule::Capsule()
{
	set(1, MATH::Vec3(0.0f, 1.0f, 0.0f), MATH::Vec3(0.0f, 0.0f, 0.0f));
}

GEOMETRY::Capsule::Capsule(float r_, MATH::Vec3 sphereCentrePosA_, MATH::Vec3 sphereCentrePosB_)
{
	set(r_, sphereCentrePosA_, sphereCentrePosB_);
}

void GEOMETRY::Capsule::set(float r_, MATH::Vec3 sphereCentrePosA_, MATH::Vec3 sphereCentrePosB_)
{
	r = r_;
	topPoint = sphereCentrePosA_;
	bottomPoint = sphereCentrePosB_;
	generateVerticesAndNormals();
}

void GEOMETRY::Capsule::generateVerticesAndNormals()
{
	// We need to fill the vertices and normals arrays with the correct data for a sphere
	// deltaTheta governs how close each ring will be around our sphere. Try messing with it
	const float deltaTheta = 10.0f;
	// deltaPhi governs how close each point will be per ring. Try messing with it
	const float deltaPhi = 14.0f;

	const float deltaRing = 0.18f;
	Vec3 circle;
	for (float thetaDeg = 0.0f; thetaDeg <= 180.0f; thetaDeg += deltaTheta)
	{
		// Build a ring
		circle = Vec3(r * cos(thetaDeg * DEGREES_TO_RADIANS), r * sin(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
		circle += topPoint;
		for (float phiDeg = 0.0f; phiDeg <= 180.0f; phiDeg += deltaPhi) {
			// Rotate a point in the ring around the y-axis to build a sphere!
			Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
			circle = rotationMatrix * circle;
			// Push the circle point to our vertices array
			vertices.push_back(circle);
			// The normal of a sphere points outwards from the center position Vec3(x, y, z)
			normals.push_back(circle);
		}
		
	}
	for (float thetaDeg = 0.0f; thetaDeg <= 360.0f; thetaDeg += deltaTheta) {
		circle = Vec3(r * -sin(thetaDeg * DEGREES_TO_RADIANS), 0.0f , r * cos(thetaDeg * DEGREES_TO_RADIANS));
		circle += bottomPoint;
		vertices.push_back(circle);
		//	// The normal of a sphere points outwards from the center position Vec3(x, y, z)
		normals.push_back(circle);
		for (float i = bottomPoint.y; i <= topPoint.y; i += deltaRing) {
			circle.y += deltaRing;
			vertices.push_back(circle);
			// The normal of a sphere points outwards from the center position Vec3(x, y, z)
			normals.push_back(circle);
		}
	}

	for (float thetaDeg = 0.0f; thetaDeg <= 180.0f; thetaDeg += deltaTheta)
	{
		// Build a ring
		circle = Vec3(r * cos(thetaDeg * DEGREES_TO_RADIANS), r * -sin(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
		circle += bottomPoint;
		for (float phiDeg = 0.0f; phiDeg <= 180.0f; phiDeg += deltaPhi) {
			// Rotate a point in the ring around the y-axis to build a sphere!
			Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
			circle = rotationMatrix * circle;
			// Push the circle point to our vertices array
			vertices.push_back(circle);
			// The normal of a sphere points outwards from the center position Vec3(x, y, z)
			normals.push_back(circle);
		}


	}
	// Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
	StoreMeshData(GL_POINTS);
}

GEOMETRY::RayIntersectionInfo GEOMETRY::Capsule::rayIntersectionInfo(const Ray& ray) const
{

	RayIntersectionInfo rayInfo;
	/// Capsule is two spheres connected by a cylinder
	Cylinder cylinderInCapsule(r, topPoint, bottomPoint);
	rayInfo = cylinderInCapsule.checkInfiniteCylinder(ray);

	if (rayInfo.isIntersected == false) {
		return rayInfo;
	}
	Vec3 P = ray.currentPosition(rayInfo.t);
	Vec3 AP = P - topPoint;
	Vec3 AB = bottomPoint - topPoint;
	Vec3 BP = P - bottomPoint;

	//Step 1 check if we are outside endCapA
	if (VMath::dot(AB, AP) < 0) {
			Sphere sphereA(topPoint, r);
			RayIntersectionInfo raySphereInfo = sphereA.rayIntersectionInfo(ray);
			return raySphereInfo; 
	}
	
	else if (VMath::dot(VMath::normalize(AB), AP) > VMath::mag(AB)) {
		Sphere sphereA(bottomPoint, r);
		RayIntersectionInfo raySphereInfo = sphereA.rayIntersectionInfo(ray);
		return raySphereInfo;
	}
	else {
		return rayInfo;
	}
	return rayInfo;
}
