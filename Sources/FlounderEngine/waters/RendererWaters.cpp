#include "RendererWaters.hpp"

#include "UbosWaters.hpp"
#include "../textures/Texture.hpp"

namespace Flounder
{
	const DescriptorType RendererWaters::typeUboScene = UniformBuffer::CreateDescriptor(0, VK_SHADER_STAGE_VERTEX_BIT);
	const DescriptorType RendererWaters::typeUboObject = UniformBuffer::CreateDescriptor(1, VK_SHADER_STAGE_ALL);
	const DescriptorType RendererWaters::typeSamplerReflection = Texture::CreateDescriptor(3, VK_SHADER_STAGE_FRAGMENT_BIT);
	const PipelineCreateInfo RendererWaters::pipelineCreateInfo =
	{
		PIPELINE_MRT, // pipelineModeFlags
		VK_POLYGON_MODE_FILL, // polygonMode
		VK_CULL_MODE_NONE, // cullModeFlags

		Vertex::GetBindingDescriptions(), // vertexBindingDescriptions
		Vertex::GetAttributeDescriptions(), // vertexAttributeDescriptions

		{ typeUboScene, typeUboObject, typeSamplerReflection }, // descriptors

		{ "res/shaders/waters/water.vert.spv", "res/shaders/waters/water.frag.spv" } // shaderStages
	};

	RendererWaters::RendererWaters(const int &subpass) :
		IRenderer(),
		m_uniformScene(new UniformBuffer(sizeof(UbosWaters::UboScene))),
		m_pipeline(new Pipeline("waters", pipelineCreateInfo, subpass))
	{
	}

	RendererWaters::~RendererWaters()
	{
		delete m_uniformScene;
		delete m_pipeline;
	}

	void RendererWaters::Render(const VkCommandBuffer *commandBuffer, const Vector4 &clipPlane, const ICamera &camera)
	{
		UbosWaters::UboScene uboScene = {};
		uboScene.projection = *camera.GetProjectionMatrix();
		uboScene.view = *camera.GetViewMatrix();
		m_uniformScene->Update(&uboScene);

		vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());

		if (Waters::Get()->GetWater() != nullptr)
		{
			Waters::Get()->GetWater()->CmdRender(*commandBuffer, *m_pipeline, *m_uniformScene);
		}
	}
}