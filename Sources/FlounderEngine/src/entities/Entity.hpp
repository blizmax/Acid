﻿#pragma once

#include "../maths/Transform.hpp"
#include "../renderer/pipelines/Pipeline.hpp"
#include "../renderer/buffers/UniformBuffer.hpp"
#include "../space/ISpatialStructure.hpp"
#include "IComponent.hpp"

namespace Flounder
{
	class Entity :
		public ISpatialObject
	{
	private:
		UniformBuffer *m_uniformObject;

		ISpatialStructure<Entity*> *m_structure;
		std::vector<IComponent*> *m_components;

		Transform *m_transform;
		bool m_removed;
	public:
		Entity(ISpatialStructure<Entity*> *structure, const Transform &transform);

		~Entity();

		void Update();

		void CmdRender(const VkCommandBuffer &commandBuffer, const Pipeline &pipeline, const UniformBuffer &uniformScene);

		ISpatialStructure<Entity*> *GetStructure() const { return m_structure; }

		void MoveStructure(ISpatialStructure<Entity*> *structure);

		std::vector<IComponent*> *GetComponents() const { return m_components; }

		void AddComponent(IComponent *component);

		void RemoveComponent(IComponent *component);

		template<typename T>
		T GetComponent()
		{
			for (auto c : *m_components)
			{
				T casted = dynamic_cast<T>(c);

				if (casted != nullptr)
				{
					return casted;
				}
			}

			return nullptr;
		}

		Transform *GetTransform() const { return m_transform; }

		void SetTransform(const Transform &transform) const { m_transform->Set(transform); }

		bool GetRemoved() const { return m_removed; }

		void Remove();

		ICollider *GetCollider() override;
	};
}