#pragma once

#include "Maths/Transform.hpp"
#include "Objects/IComponent.hpp"
#include "Objects/GameObject.hpp"
#include "Frustum.hpp"
#include "Intersect.hpp"
#include "Ray.hpp"

namespace fl
{
	/// <summary>
	/// A simple class that represents a physical shape.
	/// </summary>
	class FL_EXPORT ICollider :
		public IComponent
	{
	public:
		/// <summary>
		/// Creates a new collider.
		/// </summary>
		ICollider()
		{
		}

		/// <summary>
		/// Deconstructor for the collider.
		/// </summary>
		virtual ~ICollider()
		{
		}

		void Update() override
		{
		}

		virtual void Load(LoadedValue *value) override = 0;

		virtual void Write(LoadedValue *destination) override = 0;

		virtual std::string GetName() const override = 0;

		/// <summary>
		/// Clones this collder into the destination and updates it.
		/// </summary>
		/// <param name="transform"> The amount to transform the object. </param>
		/// <param name="destination"> The collider to store the new data in. </param>
		/// <returns> The destination. </returns>
		virtual ICollider *UpdateCollider(const Transform &transform, ICollider *destination) = 0;

		/// <summary>
		/// Tests whether a shape is intersecting this shape.
		/// </summary>
		/// <param name="other"> The other shape being tested for intersection. </param>
		/// <returns> Data about the calculated shape intersection. </returns>
		virtual Intersect Intersects(const ICollider &other) = 0;

		/// <summary>
		/// Tests whether a ray is intersecting this shape.
		/// </summary>
		/// <param name="ray"> The ray being tested for intersection. </param>
		/// <returns> Data about the calculated ray intersection. </returns>
		virtual Intersect Intersects(const Ray &ray) = 0;

		/// <summary>
		/// Gets if the shape is partially in the view frustum.
		/// </summary>
		/// <param name="frustum"> The view frustum. </param>
		/// <returns> If the shape is partially in the view frustum. </returns>
		virtual bool InFrustum(const Frustum &frustum) = 0;

		/// <summary>
		/// Tests whether another this shape completely contains the other.
		/// </summary>
		/// <param name="other"> The shape being tested for containment. </param>
		/// <returns> True if {@code other} is contained by this shape, false otherwise. </returns>
		virtual bool Contains(const ICollider &other) = 0;

		/// <summary>
		/// Gets if a point is contained in this shape.
		/// </summary>
		/// <param name="point"> The point to check if it is contained. </param>
		/// <returns> If the point is contained in this shape. </returns>
		virtual bool Contains(const Vector3 &point) = 0;
	};
}
