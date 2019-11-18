#include "RenderPass.h"
#include "../Orbital.Video/RenderPassDesc.h"
#include "Texture.h"

ORBITAL_EXPORT RenderPass* Orbital_Video_Vulkan_RenderPass_Create(Device* device)
{
	RenderPass* handle = (RenderPass*)calloc(1, sizeof(RenderPass));
	handle->device = device;
	return handle;
}

ORBITAL_EXPORT int Orbital_Video_Vulkan_RenderPass_Init(RenderPass* handle, RenderPassDesc* desc, VkImageView* imageView, uint32_t imageViewCount, uint32_t width, uint32_t height, uint32_t depth, VkFormat format)
{
	handle->width = width;
	handle->height = height;

	// init native desc
	uint32_t attachmentCount = 1;
	VkAttachmentDescription attachments[1] = {0};
	attachments[0].format = format;
	attachments[0].flags = 0;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	/*attachments[1].format = format;
	attachments[1].flags = 0;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

    VkAttachmentReference colorReference = {0};
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    /*VkAttachmentReference depthReference = {0};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

	uint32_t subpassDependencyCount = 0;
    VkSubpassDependency subpassDependencies[1] = {0};
	/*subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;// Depth buffer is shared between swapchain images
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	subpassDependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	subpassDependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	subpassDependencies[0].dependencyFlags = 0;*/

	/*subpassDependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;// Image Layout Transition
	subpassDependencies[1].dstSubpass = 0;
	subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[1].srcAccessMask = 0;
	subpassDependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	subpassDependencies[1].dependencyFlags = 0;*/

	VkSubpassDescription subpass = {0};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;
	subpass.preserveAttachmentCount = 0;
	//subpass.pDepthStencilAttachment = &depthReference;
	subpass.preserveAttachmentCount = 0;
	subpass.pResolveAttachments = NULL;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;

	// create render pass
	VkRenderPassCreateInfo renderPassInfo = {0};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachmentCount;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.dependencyCount = subpassDependencyCount;
	renderPassInfo.pDependencies = subpassDependencies;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	if (vkCreateRenderPass(handle->device->device, &renderPassInfo, NULL, &handle->renderPass) != VK_SUCCESS) return 0;

	// create frame buffers
	for (uint32_t i = 0; i != imageViewCount; ++i)
	{
		VkFramebufferCreateInfo frameBufferInfo = {0};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = handle->renderPass;
		frameBufferInfo.attachmentCount = 1;
		frameBufferInfo.pAttachments = &imageView;
		frameBufferInfo.width = width;
		frameBufferInfo.height = height;
		frameBufferInfo.layers = depth;
		if (vkCreateFramebuffer(handle->device->device, &frameBufferInfo, NULL, &handle->frameBuffer) != VK_SUCCESS) return 0;
	}
}

ORBITAL_EXPORT int Orbital_Video_Vulkan_RenderPass_Init_SwapChain(RenderPass* handle, RenderPassDesc* desc, SwapChain* swapChain)
{
	handle->swapChain = swapChain;
	return Orbital_Video_Vulkan_RenderPass_Init(handle, desc, swapChain->imageViews, swapChain->bufferCount, swapChain->width, swapChain->height, 1, swapChain->format);
}

ORBITAL_EXPORT int Orbital_Video_Vulkan_RenderPass_Init_Texture(RenderPass* handle, RenderPassDesc* desc, Texture* texture)
{
	return Orbital_Video_Vulkan_RenderPass_Init(handle, desc, texture->imageView, 1, texture->width, texture->height, texture->depth, texture->format);
}