#include "Otter/Utilities/ShaderUtilities.hpp"
#include <fstream>
#include <loguru.hpp>
#include <iostream>
#include "glslang/Public/ShaderLang.h"
#include "Otter/Utilities/MD5.hpp"

namespace Otter
{
	const TBuiltInResource DefaultTBuiltInResource = {
		/* .MaxLights = */ 32,
		/* .MaxClipPlanes = */ 6,
		/* .MaxTextureUnits = */ 32,
		/* .MaxTextureCoords = */ 32,
		/* .MaxVertexAttribs = */ 64,
		/* .MaxVertexUniformComponents = */ 4096,
		/* .MaxVaryingFloats = */ 64,
		/* .MaxVertexTextureImageUnits = */ 32,
		/* .MaxCombinedTextureImageUnits = */ 80,
		/* .MaxTextureImageUnits = */ 32,
		/* .MaxFragmentUniformComponents = */ 4096,
		/* .MaxDrawBuffers = */ 32,
		/* .MaxVertexUniformVectors = */ 128,
		/* .MaxVaryingVectors = */ 8,
		/* .MaxFragmentUniformVectors = */ 16,
		/* .MaxVertexOutputVectors = */ 16,
		/* .MaxFragmentInputVectors = */ 15,
		/* .MinProgramTexelOffset = */ -8,
		/* .MaxProgramTexelOffset = */ 7,
		/* .MaxClipDistances = */ 8,
		/* .MaxComputeWorkGroupCountX = */ 65535,
		/* .MaxComputeWorkGroupCountY = */ 65535,
		/* .MaxComputeWorkGroupCountZ = */ 65535,
		/* .MaxComputeWorkGroupSizeX = */ 1024,
		/* .MaxComputeWorkGroupSizeY = */ 1024,
		/* .MaxComputeWorkGroupSizeZ = */ 64,
		/* .MaxComputeUniformComponents = */ 1024,
		/* .MaxComputeTextureImageUnits = */ 16,
		/* .MaxComputeImageUniforms = */ 8,
		/* .MaxComputeAtomicCounters = */ 8,
		/* .MaxComputeAtomicCounterBuffers = */ 1,
		/* .MaxVaryingComponents = */ 60,
		/* .MaxVertexOutputComponents = */ 64,
		/* .MaxGeometryInputComponents = */ 64,
		/* .MaxGeometryOutputComponents = */ 128,
		/* .MaxFragmentInputComponents = */ 128,
		/* .MaxImageUnits = */ 8,
		/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
		/* .MaxCombinedShaderOutputResources = */ 8,
		/* .MaxImageSamples = */ 0,
		/* .MaxVertexImageUniforms = */ 0,
		/* .MaxTessControlImageUniforms = */ 0,
		/* .MaxTessEvaluationImageUniforms = */ 0,
		/* .MaxGeometryImageUniforms = */ 0,
		/* .MaxFragmentImageUniforms = */ 8,
		/* .MaxCombinedImageUniforms = */ 8,
		/* .MaxGeometryTextureImageUnits = */ 16,
		/* .MaxGeometryOutputVertices = */ 256,
		/* .MaxGeometryTotalOutputComponents = */ 1024,
		/* .MaxGeometryUniformComponents = */ 1024,
		/* .MaxGeometryVaryingComponents = */ 64,
		/* .MaxTessControlInputComponents = */ 128,
		/* .MaxTessControlOutputComponents = */ 128,
		/* .MaxTessControlTextureImageUnits = */ 16,
		/* .MaxTessControlUniformComponents = */ 1024,
		/* .MaxTessControlTotalOutputComponents = */ 4096,
		/* .MaxTessEvaluationInputComponents = */ 128,
		/* .MaxTessEvaluationOutputComponents = */ 128,
		/* .MaxTessEvaluationTextureImageUnits = */ 16,
		/* .MaxTessEvaluationUniformComponents = */ 1024,
		/* .MaxTessPatchComponents = */ 120,
		/* .MaxPatchVertices = */ 32,
		/* .MaxTessGenLevel = */ 64,
		/* .MaxViewports = */ 16,
		/* .MaxVertexAtomicCounters = */ 0,
		/* .MaxTessControlAtomicCounters = */ 0,
		/* .MaxTessEvaluationAtomicCounters = */ 0,
		/* .MaxGeometryAtomicCounters = */ 0,
		/* .MaxFragmentAtomicCounters = */ 8,
		/* .MaxCombinedAtomicCounters = */ 8,
		/* .MaxAtomicCounterBindings = */ 1,
		/* .MaxVertexAtomicCounterBuffers = */ 0,
		/* .MaxTessControlAtomicCounterBuffers = */ 0,
		/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
		/* .MaxGeometryAtomicCounterBuffers = */ 0,
		/* .MaxFragmentAtomicCounterBuffers = */ 1,
		/* .MaxCombinedAtomicCounterBuffers = */ 1,
		/* .MaxAtomicCounterBufferSize = */ 16384,
		/* .MaxTransformFeedbackBuffers = */ 4,
		/* .MaxTransformFeedbackInterleavedComponents = */ 64,
		/* .MaxCullDistances = */ 8,
		/* .MaxCombinedClipAndCullDistances = */ 8,
		/* .MaxSamples = */ 4,
		/* .maxMeshOutputVerticesNV = */ 256,
		/* .maxMeshOutputPrimitivesNV = */ 512,
		/* .maxMeshWorkGroupSizeX_NV = */ 32,
		/* .maxMeshWorkGroupSizeY_NV = */ 1,
		/* .maxMeshWorkGroupSizeZ_NV = */ 1,
		/* .maxTaskWorkGroupSizeX_NV = */ 32,
		/* .maxTaskWorkGroupSizeY_NV = */ 1,
		/* .maxTaskWorkGroupSizeZ_NV = */ 1,
		/* .maxMeshViewCountNV = */ 4,
		/* .maxDualSourceDrawBuffersEXT = */ 1,

		/* .limits = */ {
			/* .nonInductiveForLoops = */ 1,
			/* .whileLoops = */ 1,
			/* .doWhileLoops = */ 1,
			/* .generalUniformIndexing = */ 1,
			/* .generalAttributeMatrixVectorIndexing = */ 1,
			/* .generalVaryingIndexing = */ 1,
			/* .generalSamplerIndexing = */ 1,
			/* .generalVariableIndexing = */ 1,
			/* .generalConstantMatrixVectorIndexing = */ 1,
    }};

	bool ShaderUtilities::LoadShaderCode(std::string path, std::vector<uint32_t>& result)
	{
		std::filesystem::path fullPath(path);
		fullPath = std::filesystem::absolute(fullPath);
		std::ifstream glslFileStream(fullPath, std::ios::ate | std::ios::binary);
		if (!glslFileStream.is_open())
		{
			LOG_F(ERROR, "Failed to open file with path %s", fullPath.c_str());
			return false;
		}

		size_t fileSize = (size_t) glslFileStream.tellg();
		std::vector<char> buffer(fileSize);
		glslFileStream.seekg(0);
		glslFileStream.read(buffer.data(), fileSize);
		glslFileStream.close();

		// Find hash of shader
		std::string hash = md5(std::string(buffer.begin(), buffer.end())); //.spv
		std::filesystem::path shaderPath("CompiledShaders/");
		if(!std::filesystem::exists(shaderPath))
			std::filesystem::create_directory(shaderPath);
		shaderPath /= hash += ".spv";

		// Compile the shader if it does not yet exist, otherwise, read an existing shader.
		if(!std::filesystem::exists(shaderPath))
		{
			glslang_stage_t shaderLanguage = FindShaderLanguage(fullPath);
			if(shaderLanguage == glslang_stage_t::GLSLANG_STAGE_COUNT)
				return false;

			result.clear();
			result = CompileShaderToSPIRV_Vulkan(shaderLanguage, buffer.data(), fullPath.filename().c_str());

			if(result.size() <= 0)
				return false;

			LOG_F(INFO, "Compiled shader %s", fullPath.filename().c_str());
			
			//Export spir-v
			std::ofstream out(shaderPath, std::ofstream::binary);
			for (size_t i = 0; i < result.size(); ++i)
			{
				uint32_t a = i*2;
				out.write(reinterpret_cast<const char *>(&result[i]), sizeof(result[i]));
			}
			out.close();

			return true;
		}
		else
		{
			std::ifstream spirvFileStream(shaderPath, std::ios::ate | std::ios::binary);
			if (!spirvFileStream.is_open())
			{
				LOG_F(ERROR, "Failed to open file with path %s", fullPath.c_str());
				return false;
			}
			
			fileSize = (size_t) glslFileStream.tellg();
			spirvFileStream.seekg(0);

			while(!spirvFileStream.eof())
			{
				uint32_t val;
    			spirvFileStream.read(reinterpret_cast<char*>(&val), sizeof val);
				result.push_back(val);
			}
			result.erase(result.end()-1);	// 🤔
			spirvFileStream.close();

			return true;
		}
	}

	VkShaderModule ShaderUtilities::LoadShaderModule(std::string path, VkDevice logicalDevice)
	{
		std::vector<uint32_t> shaderCode;
		bool result = ShaderUtilities::LoadShaderCode(path, shaderCode);
		if(!result)
			return VK_NULL_HANDLE;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size() * sizeof(uint32_t);
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			LOG_F(ERROR, "Failed to create shader module for %s", path.c_str());
			return VK_NULL_HANDLE;
		}
		return shaderModule;
	}

	std::vector<uint32_t> ShaderUtilities::CompileShaderToSPIRV_Vulkan(const glslang_stage_t stage, const char* shaderSource, const char* fileName)
	{
		const glslang_input_t input = {
			.language = GLSLANG_SOURCE_GLSL,
			.stage = stage,
			.client = GLSLANG_CLIENT_VULKAN,
			.client_version = GLSLANG_TARGET_VULKAN_1_3,
			.target_language = GLSLANG_TARGET_SPV,
			.target_language_version = GLSLANG_TARGET_SPV_1_3,
			.code = shaderSource,
			.default_version = 450,
			.default_profile = GLSLANG_NO_PROFILE,
			.force_default_version_and_profile = false,
			.forward_compatible = false,
			.messages = GLSLANG_MSG_DEFAULT_BIT,
	        .resource = reinterpret_cast<const glslang_resource_t*>(&DefaultTBuiltInResource),
		};

		glslang_shader_t* shader = glslang_shader_create(&input);

		if (!glslang_shader_preprocess(shader, &input))	{
			LOG_F(ERROR, "GLSL preprocessing failed %s", fileName);
			LOG_F(ERROR, "%s", glslang_shader_get_info_log(shader));
			LOG_F(ERROR, "%s", glslang_shader_get_info_debug_log(shader));
			glslang_shader_delete(shader);
			return std::vector<uint32_t>();
		}

		if (!glslang_shader_parse(shader, &input)) {
			LOG_F(ERROR, "GLSL parsing failed %s", fileName);
			LOG_F(ERROR, "%s", glslang_shader_get_info_log(shader));
			LOG_F(ERROR, "%s", glslang_shader_get_info_debug_log(shader));
			glslang_shader_delete(shader);
			return std::vector<uint32_t>();
		}

		glslang_program_t* program = glslang_program_create();
		glslang_program_add_shader(program, shader);

		if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
			LOG_F(ERROR, "GLSL linking failed %s", fileName);
			LOG_F(ERROR, "%s", glslang_program_get_info_log(program));
			LOG_F(ERROR, "%s", glslang_program_get_info_debug_log(program));
			glslang_program_delete(program);
			glslang_shader_delete(shader);
			return std::vector<uint32_t>();
		}

		glslang_program_SPIRV_generate(program, stage);

		std::vector<uint32_t> outShaderModule(glslang_program_SPIRV_get_size(program));
		glslang_program_SPIRV_get(program, outShaderModule.data());

		const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
		if (spirv_messages)
			LOG_F(INFO, "(%s) %s\b", fileName, spirv_messages);

		glslang_program_delete(program);
		glslang_shader_delete(shader);

		return outShaderModule;
	}
	
	glslang_stage_t ShaderUtilities::FindShaderLanguage(std::filesystem::path &filePath)
	{
		glslang_stage_t result;

		if(!filePath.has_extension())
		{
			LOG_F(ERROR, "Could not deduct shader language for %s, as it doesn't have an extension. Path: %s", filePath.filename().c_str(), filePath.c_str());
			return result;
		}

		std::string extension(filePath.extension().c_str());
		extension.erase(extension.begin());

		/*	vert = vertex
		*	tesc = tessellation control
		*	tese = tessellation evaluation
		*	geom = geometry
		*	frag = fragment
		*	comp = compute
		*	rgen = ray generation
		*	rint = ray intersection
		*	rahit = ray any hit
		*	rchit = ray closest hit
		*	rmiss = ray miss
		*	rcall = ray callable
		*	mesh  = mesh
		*	task  = task	*/
		if (extension == "vert") 
				return glslang_stage_t::GLSLANG_STAGE_VERTEX;
		if (extension == "geom") 
				return glslang_stage_t::GLSLANG_STAGE_GEOMETRY;
		if (extension == "frag") 
				return glslang_stage_t::GLSLANG_STAGE_FRAGMENT;
		
		LOG_F(ERROR, "Unsupported shader extension found: %s. It may need to be added to FindShaderLanguage.", extension.c_str());
		return glslang_stage_t::GLSLANG_STAGE_COUNT;
	}
}