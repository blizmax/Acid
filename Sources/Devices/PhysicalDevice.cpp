#include "PhysicalDevice.hpp"

#include "Graphics/Graphics.hpp"
#include "Instance.hpp"

namespace acid
{
static const std::vector<VkSampleCountFlagBits> STAGE_FLAG_BITS{ VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_8_BIT,
	VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_2_BIT };

PhysicalDevice::PhysicalDevice(const Instance *instance) :
	m_instance{instance}
{
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(*m_instance, &physicalDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(*m_instance, &physicalDeviceCount, physicalDevices.data());

	m_physicalDevice = ChoosePhysicalDevice(physicalDevices);

	if (!m_physicalDevice)
	{
		throw std::runtime_error{"Vulkan runtime error, failed to find a suitable GPU"};
	}

	vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);
	vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_features);
	vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);
	m_msaaSamples = GetMaxUsableSampleCount();

#if defined(ACID_VERBOSE)
	std::cout << "Selected Physical Device: " << m_properties.deviceID << " " << std::quoted(m_properties.deviceName) << '\n';
#endif
}

VkPhysicalDevice PhysicalDevice::ChoosePhysicalDevice(const std::vector<VkPhysicalDevice> &devices)
{
	// Maps to hold devices and sort by rank.
	std::multimap<int32_t, VkPhysicalDevice> rankedDevices;

	// Iterates through all devices and rate their suitability.
	for (const auto &device : devices)
	{
		rankedDevices.emplace(ScorePhysicalDevice(device), device);
	}

	// Checks to make sure the best candidate scored higher than 0  rbegin points to last element of ranked devices(highest rated), first is its rating.
	if (rankedDevices.rbegin()->first > 0)
	{
		return rankedDevices.rbegin()->second;
	}

	return nullptr;
}

int32_t PhysicalDevice::ScorePhysicalDevice(const VkPhysicalDevice &device)
{
	int32_t score{};

	// Checks if the requested extensions are supported.
	uint32_t extensionPropertyCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, nullptr);
	std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, extensionProperties.data());

	// Iterates through all extensions requested.
	for (const char *currentExtension : Instance::DeviceExtensions)
	{
		bool extensionFound{};

		// Checks if the extension is in the available extensions.
		for (const auto &extension : extensionProperties)
		{
			if (strcmp(currentExtension, extension.extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		// Returns a score of 0 if this device is missing a required extension.
		if (!extensionFound)
		{
			return 0;
		}
	}

	// Obtain the device features and properties of the current device being rateds.
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(device, &physicalDeviceFeatures);

#if defined(ACID_VERBOSE)
	LogVulkanDevice(physicalDeviceProperties, extensionProperties);
#endif

	// Adds a large score boost for discrete GPUs (dedicated graphics cards).
	if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	// Gives a higher score to devices with a higher maximum texture size.
	score += physicalDeviceProperties.limits.maxImageDimension2D;

	return score;
}

VkSampleCountFlagBits PhysicalDevice::GetMaxUsableSampleCount() const
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

	auto counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);

	for (const auto &sampleFlag : STAGE_FLAG_BITS)
	{
		if (counts & sampleFlag)
		{
			return sampleFlag;
		}
	}

	return VK_SAMPLE_COUNT_1_BIT;
}

void PhysicalDevice::LogVulkanDevice(const VkPhysicalDeviceProperties &physicalDeviceProperties, const std::vector<VkExtensionProperties> &extensionProperties)
{
	switch (static_cast<int32_t>(physicalDeviceProperties.deviceType))
	{
	case 1:
		std::cout << "Integrated";
		break;
	case 2:
		std::cout << "Discrete";
		break;
	case 3:
		std::cout << "Virtual";
		break;
	case 4:
		std::cout << "CPU";
		break;
	default:
		std::cout << "Other " << physicalDeviceProperties.deviceType;
	}

	std::cout << " Physical Device: " << physicalDeviceProperties.deviceID;

	switch (physicalDeviceProperties.vendorID)
	{
	case 0x8086:
		std::cout << " \"Intel\"";
		break;
	case 0x10DE:
		std::cout << " \"Nvidia\"";
		break;
	case 0x1002:
		std::cout << " \"AMD\"";
		break;
	default:
		std::cout << " \"" << physicalDeviceProperties.vendorID << '\"';
	}

	std::cout << " " << std::quoted(physicalDeviceProperties.deviceName) << '\n';

	uint32_t supportedVersion[3] = {VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion), VK_VERSION_MINOR(physicalDeviceProperties.apiVersion),
		VK_VERSION_PATCH(physicalDeviceProperties.apiVersion)};
	std::cout << "API Version: " << supportedVersion[0] << "." << supportedVersion[1] << "." << supportedVersion[2] << '\n';
	std::cout << "Extensions: ";

	for (const auto &extension : extensionProperties)
	{
		std::cout << extension.extensionName << ", ";
	}

	std::cout << "\n\n";
}
}
