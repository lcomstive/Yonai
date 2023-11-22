using System;

namespace Yonai.Graphics.Backends.Vulkan
{
	[Flags]
	public enum VkQueueFlags : int
	{
		Graphics = 0x00000001,
		Compute = 0x00000002,
		Transfer = 0x00000004,
		SparseBinding = 0x00000008,
		Protected = 0x00000010,
		VideoDecodeKHR = 0x00000020,
		OpticalFlowNV = 0x00000100
	}

	public enum VkPhysicalDeviceType : int
	{
		Other = 0,
		IntegratedGPU = 1,
		DiscreteGPU = 2,
		VirtualGPU = 3,
		CPU = 4
	}

	public enum VkFormat : int
	{
		Undefined = 0,
		R4G4_UNORM_PACK8 = 1,
		R4G4B4A4_UNORM_PACK16 = 2,
		B4G4R4A4_UNORM_PACK16 = 3,
		R5G6B5_UNORM_PACK16 = 4,
		B5G6R5_UNORM_PACK16 = 5,
		R5G5B5A1_UNORM_PACK16 = 6,
		B5G5R5A1_UNORM_PACK16 = 7,
		A1R5G5B5_UNORM_PACK16 = 8,
		R8_UNORM = 9,
		R8_SNORM = 10,
		R8_USCALED = 11,
		R8_SSCALED = 12,
		R8_UINT = 13,
		R8_SINT = 14,
		R8_SRGB = 15,
		R8G8_UNORM = 16,
		R8G8_SNORM = 17,
		R8G8_USCALED = 18,
		R8G8_SSCALED = 19,
		R8G8_UINT = 20,
		R8G8_SINT = 21,
		R8G8_SRGB = 22,
		R8G8B8_UNORM = 23,
		R8G8B8_SNORM = 24,
		R8G8B8_USCALED = 25,
		R8G8B8_SSCALED = 26,
		R8G8B8_UINT = 27,
		R8G8B8_SINT = 28,
		R8G8B8_SRGB = 29,
		B8G8R8_UNORM = 30,
		B8G8R8_SNORM = 31,
		B8G8R8_USCALED = 32,
		B8G8R8_SSCALED = 33,
		B8G8R8_UINT = 34,
		B8G8R8_SINT = 35,
		B8G8R8_SRGB = 36,
		R8G8B8A8_UNORM = 37,
		R8G8B8A8_SNORM = 38,
		R8G8B8A8_USCALED = 39,
		R8G8B8A8_SSCALED = 40,
		R8G8B8A8_UINT = 41,
		R8G8B8A8_SINT = 42,
		R8G8B8A8_SRGB = 43,
		B8G8R8A8_UNORM = 44,
		B8G8R8A8_SNORM = 45,
		B8G8R8A8_USCALED = 46,
		B8G8R8A8_SSCALED = 47,
		B8G8R8A8_UINT = 48,
		B8G8R8A8_SINT = 49,
		B8G8R8A8_SRGB = 50,
		A8B8G8R8_UNORM_PACK32 = 51,
		A8B8G8R8_SNORM_PACK32 = 52,
		A8B8G8R8_USCALED_PACK32 = 53,
		A8B8G8R8_SSCALED_PACK32 = 54,
		A8B8G8R8_UINT_PACK32 = 55,
		A8B8G8R8_SINT_PACK32 = 56,
		A8B8G8R8_SRGB_PACK32 = 57,
		A2R10G10B10_UNORM_PACK32 = 58,
		A2R10G10B10_SNORM_PACK32 = 59,
		A2R10G10B10_USCALED_PACK32 = 60,
		A2R10G10B10_SSCALED_PACK32 = 61,
		A2R10G10B10_UINT_PACK32 = 62,
		A2R10G10B10_SINT_PACK32 = 63,
		A2B10G10R10_UNORM_PACK32 = 64,
		A2B10G10R10_SNORM_PACK32 = 65,
		A2B10G10R10_USCALED_PACK32 = 66,
		A2B10G10R10_SSCALED_PACK32 = 67,
		A2B10G10R10_UINT_PACK32 = 68,
		A2B10G10R10_SINT_PACK32 = 69,
		R16_UNORM = 70,
		R16_SNORM = 71,
		R16_USCALED = 72,
		R16_SSCALED = 73,
		R16_UINT = 74,
		R16_SINT = 75,
		R16_SFLOAT = 76,
		R16G16_UNORM = 77,
		R16G16_SNORM = 78,
		R16G16_USCALED = 79,
		R16G16_SSCALED = 80,
		R16G16_UINT = 81,
		R16G16_SINT = 82,
		R16G16_SFLOAT = 83,
		R16G16B16_UNORM = 84,
		R16G16B16_SNORM = 85,
		R16G16B16_USCALED = 86,
		R16G16B16_SSCALED = 87,
		R16G16B16_UINT = 88,
		R16G16B16_SINT = 89,
		R16G16B16_SFLOAT = 90,
		R16G16B16A16_UNORM = 91,
		R16G16B16A16_SNORM = 92,
		R16G16B16A16_USCALED = 93,
		R16G16B16A16_SSCALED = 94,
		R16G16B16A16_UINT = 95,
		R16G16B16A16_SINT = 96,
		R16G16B16A16_SFLOAT = 97,
		R32_UINT = 98,
		R32_SINT = 99,
		R32_SFLOAT = 100,
		R32G32_UINT = 101,
		R32G32_SINT = 102,
		R32G32_SFLOAT = 103,
		R32G32B32_UINT = 104,
		R32G32B32_SINT = 105,
		R32G32B32_SFLOAT = 106,
		R32G32B32A32_UINT = 107,
		R32G32B32A32_SINT = 108,
		R32G32B32A32_SFLOAT = 109,
		R64_UINT = 110,
		R64_SINT = 111,
		R64_SFLOAT = 112,
		R64G64_UINT = 113,
		R64G64_SINT = 114,
		R64G64_SFLOAT = 115,
		R64G64B64_UINT = 116,
		R64G64B64_SINT = 117,
		R64G64B64_SFLOAT = 118,
		R64G64B64A64_UINT = 119,
		R64G64B64A64_SINT = 120,
		R64G64B64A64_SFLOAT = 121,
		B10G11R11_UFLOAT_PACK32 = 122,
		E5B9G9R9_UFLOAT_PACK32 = 123,
		D16_UNORM = 124,
		X8_D24_UNORM_PACK32 = 125,
		D32_SFLOAT = 126,
		S8_UINT = 127,
		D16_UNORM_S8_UINT = 128,
		D24_UNORM_S8_UINT = 129,
		D32_SFLOAT_S8_UINT = 130,
		BC1_RGB_UNORM_BLOCK = 131,
		BC1_RGB_SRGB_BLOCK = 132,
		BC1_RGBA_UNORM_BLOCK = 133,
		BC1_RGBA_SRGB_BLOCK = 134,
		BC2_UNORM_BLOCK = 135,
		BC2_SRGB_BLOCK = 136,
		BC3_UNORM_BLOCK = 137,
		BC3_SRGB_BLOCK = 138,
		BC4_UNORM_BLOCK = 139,
		BC4_SNORM_BLOCK = 140,
		BC5_UNORM_BLOCK = 141,
		BC5_SNORM_BLOCK = 142,
		BC6H_UFLOAT_BLOCK = 143,
		BC6H_SFLOAT_BLOCK = 144,
		BC7_UNORM_BLOCK = 145,
		BC7_SRGB_BLOCK = 146,
		ETC2_R8G8B8_UNORM_BLOCK = 147,
		ETC2_R8G8B8_SRGB_BLOCK = 148,
		ETC2_R8G8B8A1_UNORM_BLOCK = 149,
		ETC2_R8G8B8A1_SRGB_BLOCK = 150,
		ETC2_R8G8B8A8_UNORM_BLOCK = 151,
		ETC2_R8G8B8A8_SRGB_BLOCK = 152,
		EAC_R11_UNORM_BLOCK = 153,
		EAC_R11_SNORM_BLOCK = 154,
		EAC_R11G11_UNORM_BLOCK = 155,
		EAC_R11G11_SNORM_BLOCK = 156,
		ASTC_4x4_UNORM_BLOCK = 157,
		ASTC_4x4_SRGB_BLOCK = 158,
		ASTC_5x4_UNORM_BLOCK = 159,
		ASTC_5x4_SRGB_BLOCK = 160,
		ASTC_5x5_UNORM_BLOCK = 161,
		ASTC_5x5_SRGB_BLOCK = 162,
		ASTC_6x5_UNORM_BLOCK = 163,
		ASTC_6x5_SRGB_BLOCK = 164,
		ASTC_6x6_UNORM_BLOCK = 165,
		ASTC_6x6_SRGB_BLOCK = 166,
		ASTC_8x5_UNORM_BLOCK = 167,
		ASTC_8x5_SRGB_BLOCK = 168,
		ASTC_8x6_UNORM_BLOCK = 169,
		ASTC_8x6_SRGB_BLOCK = 170,
		ASTC_8x8_UNORM_BLOCK = 171,
		ASTC_8x8_SRGB_BLOCK = 172,
		ASTC_10x5_UNORM_BLOCK = 173,
		ASTC_10x5_SRGB_BLOCK = 174,
		ASTC_10x6_UNORM_BLOCK = 175,
		ASTC_10x6_SRGB_BLOCK = 176,
		ASTC_10x8_UNORM_BLOCK = 177,
		ASTC_10x8_SRGB_BLOCK = 178,
		ASTC_10x10_UNORM_BLOCK = 179,
		ASTC_10x10_SRGB_BLOCK = 180,
		ASTC_12x10_UNORM_BLOCK = 181,
		ASTC_12x10_SRGB_BLOCK = 182,
		ASTC_12x12_UNORM_BLOCK = 183,
		ASTC_12x12_SRGB_BLOCK = 184,
		G8B8G8R8_422_UNORM = 1000156000,
		B8G8R8G8_422_UNORM = 1000156001,
		G8_B8_R8_3PLANE_420_UNORM = 1000156002,
		G8_B8R8_2PLANE_420_UNORM = 1000156003,
		G8_B8_R8_3PLANE_422_UNORM = 1000156004,
		G8_B8R8_2PLANE_422_UNORM = 1000156005,
		G8_B8_R8_3PLANE_444_UNORM = 1000156006,
		R10X6_UNORM_PACK16 = 1000156007,
		R10X6G10X6_UNORM_2PACK16 = 1000156008,
		R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
		G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
		B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
		G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
		G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
		G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
		G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
		G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
		R12X4_UNORM_PACK16 = 1000156017,
		R12X4G12X4_UNORM_2PACK16 = 1000156018,
		R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
		G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
		B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
		G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
		G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
		G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
		G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
		G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
		G16B16G16R16_422_UNORM = 1000156027,
		B16G16R16G16_422_UNORM = 1000156028,
		G16_B16_R16_3PLANE_420_UNORM = 1000156029,
		G16_B16R16_2PLANE_420_UNORM = 1000156030,
		G16_B16_R16_3PLANE_422_UNORM = 1000156031,
		G16_B16R16_2PLANE_422_UNORM = 1000156032,
		G16_B16_R16_3PLANE_444_UNORM = 1000156033,
		G8_B8R8_2PLANE_444_UNORM = 1000330000,
		G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
		G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
		G16_B16R16_2PLANE_444_UNORM = 1000330003,
		A4R4G4B4_UNORM_PACK16 = 1000340000,
		A4B4G4R4_UNORM_PACK16 = 1000340001,
		ASTC_4x4_SFLOAT_BLOCK = 1000066000,
		ASTC_5x4_SFLOAT_BLOCK = 1000066001,
		ASTC_5x5_SFLOAT_BLOCK = 1000066002,
		ASTC_6x5_SFLOAT_BLOCK = 1000066003,
		ASTC_6x6_SFLOAT_BLOCK = 1000066004,
		ASTC_8x5_SFLOAT_BLOCK = 1000066005,
		ASTC_8x6_SFLOAT_BLOCK = 1000066006,
		ASTC_8x8_SFLOAT_BLOCK = 1000066007,
		ASTC_10x5_SFLOAT_BLOCK = 1000066008,
		ASTC_10x6_SFLOAT_BLOCK = 1000066009,
		ASTC_10x8_SFLOAT_BLOCK = 1000066010,
		ASTC_10x10_SFLOAT_BLOCK = 1000066011,
		ASTC_12x10_SFLOAT_BLOCK = 1000066012,
		ASTC_12x12_SFLOAT_BLOCK = 1000066013,
		PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
		PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
		PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
		PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
		PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
		PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
		PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
		PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
		R16G16_S10_5_NV = 1000464000,
		A1B5G5R5_UNORM_PACK16_KHR = 1000470000,
		A8_UNORM_KHR = 1000470001,
		ASTC_4x4_SFLOAT_BLOCK_EXT = ASTC_4x4_SFLOAT_BLOCK,
		ASTC_5x4_SFLOAT_BLOCK_EXT = ASTC_5x4_SFLOAT_BLOCK,
		ASTC_5x5_SFLOAT_BLOCK_EXT = ASTC_5x5_SFLOAT_BLOCK,
		ASTC_6x5_SFLOAT_BLOCK_EXT = ASTC_6x5_SFLOAT_BLOCK,
		ASTC_6x6_SFLOAT_BLOCK_EXT = ASTC_6x6_SFLOAT_BLOCK,
		ASTC_8x5_SFLOAT_BLOCK_EXT = ASTC_8x5_SFLOAT_BLOCK,
		ASTC_8x6_SFLOAT_BLOCK_EXT = ASTC_8x6_SFLOAT_BLOCK,
		ASTC_8x8_SFLOAT_BLOCK_EXT = ASTC_8x8_SFLOAT_BLOCK,
		ASTC_10x5_SFLOAT_BLOCK_EXT = ASTC_10x5_SFLOAT_BLOCK,
		ASTC_10x6_SFLOAT_BLOCK_EXT = ASTC_10x6_SFLOAT_BLOCK,
		ASTC_10x8_SFLOAT_BLOCK_EXT = ASTC_10x8_SFLOAT_BLOCK,
		ASTC_10x10_SFLOAT_BLOCK_EXT = ASTC_10x10_SFLOAT_BLOCK,
		ASTC_12x10_SFLOAT_BLOCK_EXT = ASTC_12x10_SFLOAT_BLOCK,
		ASTC_12x12_SFLOAT_BLOCK_EXT = ASTC_12x12_SFLOAT_BLOCK,
		G8B8G8R8_422_UNORM_KHR = G8B8G8R8_422_UNORM,
		B8G8R8G8_422_UNORM_KHR = B8G8R8G8_422_UNORM,
		G8_B8_R8_3PLANE_420_UNORM_KHR = G8_B8_R8_3PLANE_420_UNORM,
		G8_B8R8_2PLANE_420_UNORM_KHR = G8_B8R8_2PLANE_420_UNORM,
		G8_B8_R8_3PLANE_422_UNORM_KHR = G8_B8_R8_3PLANE_422_UNORM,
		G8_B8R8_2PLANE_422_UNORM_KHR = G8_B8R8_2PLANE_422_UNORM,
		G8_B8_R8_3PLANE_444_UNORM_KHR = G8_B8_R8_3PLANE_444_UNORM,
		R10X6_UNORM_PACK16_KHR = R10X6_UNORM_PACK16,
		R10X6G10X6_UNORM_2PACK16_KHR = R10X6G10X6_UNORM_2PACK16,
		R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = R10X6G10X6B10X6A10X6_UNORM_4PACK16,
		G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
		B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
		G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
		G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
		G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
		G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
		G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
		R12X4_UNORM_PACK16_KHR = R12X4_UNORM_PACK16,
		R12X4G12X4_UNORM_2PACK16_KHR = R12X4G12X4_UNORM_2PACK16,
		R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = R12X4G12X4B12X4A12X4_UNORM_4PACK16,
		G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
		B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
		G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
		G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
		G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
		G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
		G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
		G16B16G16R16_422_UNORM_KHR = G16B16G16R16_422_UNORM,
		B16G16R16G16_422_UNORM_KHR = B16G16R16G16_422_UNORM,
		G16_B16_R16_3PLANE_420_UNORM_KHR = G16_B16_R16_3PLANE_420_UNORM,
		G16_B16R16_2PLANE_420_UNORM_KHR = G16_B16R16_2PLANE_420_UNORM,
		G16_B16_R16_3PLANE_422_UNORM_KHR = G16_B16_R16_3PLANE_422_UNORM,
		G16_B16R16_2PLANE_422_UNORM_KHR = G16_B16R16_2PLANE_422_UNORM,
		G16_B16_R16_3PLANE_444_UNORM_KHR = G16_B16_R16_3PLANE_444_UNORM,
		G8_B8R8_2PLANE_444_UNORM_EXT = G8_B8R8_2PLANE_444_UNORM,
		G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT = G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
		G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT = G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
		G16_B16R16_2PLANE_444_UNORM_EXT = G16_B16R16_2PLANE_444_UNORM,
		A4R4G4B4_UNORM_PACK16_EXT = A4R4G4B4_UNORM_PACK16,
		A4B4G4R4_UNORM_PACK16_EXT = A4B4G4R4_UNORM_PACK16
	}

	public enum VkSampleCount : int
	{
		_1 = 0x00000001,
		_2 = 0x00000002,
		_4 = 0x00000004,
		_8 = 0x00000008,
		_16 = 0x00000010,
		_32 = 0x00000020,
		_64 = 0x00000040
	}

	public enum VkAttachmentLoadOp
	{
		Load = 0,
		Clear = 1,
		DontCare = 2,
		None_EXT = 1000400000
	}

	public enum VkAttachmentStoreOp : int
	{
		Store = 0,
		DontCare = 1,
		None = 1000301000,
		None_KHR = None,
		None_QCOM = None,
		None_EXT = None
	}

	public enum VkImageLayout : int
	{
		Undefined = 0,
		GENERAL = 1,
		COLOR_ATTACHMENT_OPTIMAL = 2,
		DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
		DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
		SHADER_READ_ONLY_OPTIMAL = 5,
		TRANSFER_SRC_OPTIMAL = 6,
		TRANSFER_DST_OPTIMAL = 7,
		PREINITIALIZED = 8,
		DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 1000117000,
		DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 1000117001,
		DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
		DEPTH_READ_ONLY_OPTIMAL = 1000241001,
		STENCIL_ATTACHMENT_OPTIMAL = 1000241002,
		STENCIL_READ_ONLY_OPTIMAL = 1000241003,
		READ_ONLY_OPTIMAL = 1000314000,
		ATTACHMENT_OPTIMAL = 1000314001,
		PRESENT_SRC_KHR = 1000001002,
		VIDEO_DECODE_DST_KHR = 1000024000,
		VIDEO_DECODE_SRC_KHR = 1000024001,
		VIDEO_DECODE_DPB_KHR = 1000024002,
		SHARED_PRESENT_KHR = 1000111000,
		FRAGMENT_DENSITY_MAP_OPTIMAL_EXT = 1000218000,
		FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR = 1000164003,
		ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT = 1000339000,
		DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR = DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
		DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR = DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
		SHADING_RATE_OPTIMAL_NV = FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
		DEPTH_ATTACHMENT_OPTIMAL_KHR = DEPTH_ATTACHMENT_OPTIMAL,
		DEPTH_READ_ONLY_OPTIMAL_KHR = DEPTH_READ_ONLY_OPTIMAL,
		STENCIL_ATTACHMENT_OPTIMAL_KHR = STENCIL_ATTACHMENT_OPTIMAL,
		STENCIL_READ_ONLY_OPTIMAL_KHR = STENCIL_READ_ONLY_OPTIMAL,
		READ_ONLY_OPTIMAL_KHR = READ_ONLY_OPTIMAL,
		ATTACHMENT_OPTIMAL_KHR = ATTACHMENT_OPTIMAL
	}

	public enum VkPipelineBindPoint
	{
		Graphics = 0,
		Compute = 1,
		RayTracingKHR = 1000165000,
		SubpassShading_HUAWEI = 1000369003,
		RayTracing_NV = RayTracingKHR
	}

	public enum VkSubpassDescriptionFlags : int
	{
		None = 0,
		PER_VIEW_ATTRIBUTES_BIT_NVX = 0x00000001,
		PER_VIEW_POSITION_X_ONLY_BIT_NVX = 0x00000002,
		FRAGMENT_REGION_BIT_QCOM = 0x00000004,
		SHADER_RESOLVE_BIT_QCOM = 0x00000008,
		RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_EXT = 0x00000010,
		RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_EXT = 0x00000020,
		RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_EXT = 0x00000040,
		ENABLE_LEGACY_DITHERING_BIT_EXT = 0x00000080,
		RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_ARM = RASTERIZATION_ORDER_ATTACHMENT_COLOR_ACCESS_BIT_EXT,
		RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_ARM = RASTERIZATION_ORDER_ATTACHMENT_DEPTH_ACCESS_BIT_EXT,
		RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_ARM = RASTERIZATION_ORDER_ATTACHMENT_STENCIL_ACCESS_BIT_EXT
	}

	[Flags]
	public enum VkPipelineStageFlags : int
	{
		TopOfPipe = 0x00000001,
		DrawIndirect = 0x00000002,
		VertexInput = 0x00000004,
		VertexShader = 0x00000008,
		TessellationControlShader= 0x00000010,
		TessellationEvaluationShader = 0x00000020,
		GeometryShader = 0x00000040,
		FragmentShader = 0x00000080,
		EarlyFragmentTests = 0x00000100,
		LateFragmentTests = 0x00000200,
		ColorAttachmentOutput = 0x00000400,
		ComputeShader = 0x00000800,
		Transfer = 0x00001000,
		BottomOfPipe = 0x00002000,
		Host = 0x00004000,
		AllGraphics = 0x00008000,
		AllCommands = 0x00010000,
		None = 0,
		TRANSFORM_FEEDBACK_BIT_EXT = 0x01000000,
		CONDITIONAL_RENDERING_BIT_EXT = 0x00040000,
		ACCELERATION_STRUCTURE_BUILD_BIT_KHR = 0x02000000,
		RAY_TRACING_SHADER_BIT_KHR = 0x00200000,
		FRAGMENT_DENSITY_PROCESS_BIT_EXT = 0x00800000,
		FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = 0x00400000,
		COMMAND_PREPROCESS_BIT_NV = 0x00020000,
		TASK_SHADER_BIT_EXT = 0x00080000,
		MESH_SHADER_BIT_EXT = 0x00100000,
		SHADING_RATE_IMAGE_BIT_NV = FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
		RAY_TRACING_SHADER_BIT_NV = RAY_TRACING_SHADER_BIT_KHR,
		ACCELERATION_STRUCTURE_BUILD_BIT_NV = ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
		TASK_SHADER_BIT_NV = TASK_SHADER_BIT_EXT,
		MESH_SHADER_BIT_NV = MESH_SHADER_BIT_EXT,
		NONE_KHR = None
	}

	[Flags]
	public enum VkAccessFlags : int
	{
		IndirectCommandRead = 0x00000001,
		IndexRead = 0x00000002,
		VertexAttributeRead = 0x00000004,
		UniformRead = 0x00000008,
		InputAttachmentRead = 0x00000010,
		ShaderRead = 0x00000020,
		ShaderWrite = 0x00000040,
		ColorAttachmentRead = 0x00000080,
		ColorAttachmentWrite = 0x00000100,
		DepthStencilAttachmentRead = 0x00000200,
		DepthStencilAttachmentWrite = 0x00000400,
		TransferRead = 0x00000800,
		TransferWrite = 0x00001000,
		HostRead = 0x00002000,
		HostWrite = 0x00004000,
		MemoryRead = 0x00008000,
		MemoryWrite = 0x00010000,
		None = 0,
		TRANSFORM_FEEDBACK_WRITE_BIT_EXT = 0x02000000,
		TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT = 0x04000000,
		TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT = 0x08000000,
		CONDITIONAL_RENDERING_READ_BIT_EXT = 0x00100000,
		COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT = 0x00080000,
		ACCELERATION_STRUCTURE_READ_BIT_KHR = 0x00200000,
		ACCELERATION_STRUCTURE_WRITE_BIT_KHR = 0x00400000,
		FRAGMENT_DENSITY_MAP_READ_BIT_EXT = 0x01000000,
		FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR = 0x00800000,
		COMMAND_PREPROCESS_READ_BIT_NV = 0x00020000,
		COMMAND_PREPROCESS_WRITE_BIT_NV = 0x00040000,
		SHADING_RATE_IMAGE_READ_BIT_NV = FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR,
		ACCELERATION_STRUCTURE_READ_BIT_NV = ACCELERATION_STRUCTURE_READ_BIT_KHR,
		ACCELERATION_STRUCTURE_WRITE_BIT_NV = ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
		NONE_KHR = None
	}
	public enum VkDependencyFlags : int
	{
		BY_REGION_BIT = 0x00000001,
		DEVICE_GROUP_BIT = 0x00000004,
		VIEW_LOCAL_BIT = 0x00000002,
		FEEDBACK_LOOP_BIT_EXT = 0x00000008,
		VIEW_LOCAL_BIT_KHR = VIEW_LOCAL_BIT,
		DEVICE_GROUP_BIT_KHR = DEVICE_GROUP_BIT
	}

	[Flags]
	public enum VkShaderStage : int
	{
		Vertex = 0x00000001,
		TessellationControl = 0x00000002,
		TessellationEvaluation = 0x00000004,
		Geometry = 0x00000008,
		Fragment = 0x00000010,
		Compute = 0x00000020,
		AllGraphics = 0x0000001F,
		All = 0x7FFFFFFF,
		RAYGEN_BIT_KHR = 0x00000100,
		ANY_HIT_BIT_KHR = 0x00000200,
		CLOSEST_HIT_BIT_KHR = 0x00000400,
		MISS_BIT_KHR = 0x00000800,
		INTERSECTION_BIT_KHR = 0x00001000,
		CALLABLE_BIT_KHR = 0x00002000,
		TASK_BIT_EXT = 0x00000040,
		MESH_BIT_EXT = 0x00000080,
		SUBPASS_SHADING_BIT_HUAWEI = 0x00004000,
		CLUSTER_CULLING_BIT_HUAWEI = 0x00080000,
		RAYGEN_BIT_NV = RAYGEN_BIT_KHR,
		ANY_HIT_BIT_NV = ANY_HIT_BIT_KHR,
		CLOSEST_HIT_BIT_NV = CLOSEST_HIT_BIT_KHR,
		MISS_BIT_NV = MISS_BIT_KHR,
		INTERSECTION_BIT_NV = INTERSECTION_BIT_KHR,
		CALLABLE_BIT_NV = CALLABLE_BIT_KHR,
		TASK_BIT_NV = TASK_BIT_EXT,
		MESH_BIT_NV = MESH_BIT_EXT
	}

	public enum VkVertexInputRate : int
	{
		Vertex = 0,
		Instance = 1
	}

	public enum VkPrimitiveTopology
	{
		POINT_LIST = 0,
		LINE_LIST = 1,
		LINE_STRIP = 2,
		TRIANGLE_LIST = 3,
		TRIANGLE_STRIP = 4,
		TRIANGLE_FAN = 5,
		LINE_LIST_WITH_ADJACENCY = 6,
		LINE_STRIP_WITH_ADJACENCY = 7,
		TRIANGLE_LIST_WITH_ADJACENCY = 8,
		TRIANGLE_STRIP_WITH_ADJACENCY = 9,
		PATCH_LIST = 10
	}

	public enum VkDynamicState : int
	{
		VIEWPORT = 0,
		SCISSOR = 1,
		LINE_WIDTH = 2,
		DEPTH_BIAS = 3,
		BLEND_CONSTANTS = 4,
		DEPTH_BOUNDS = 5,
		STENCIL_COMPARE_MASK = 6,
		STENCIL_WRITE_MASK = 7,
		STENCIL_REFERENCE = 8,
		CULL_MODE = 1000267000,
		FRONT_FACE = 1000267001,
		PRIMITIVE_TOPOLOGY = 1000267002,
		VIEWPORT_WITH_COUNT = 1000267003,
		SCISSOR_WITH_COUNT = 1000267004,
		VERTEX_INPUT_BINDING_STRIDE = 1000267005,
		DEPTH_TEST_ENABLE = 1000267006,
		DEPTH_WRITE_ENABLE = 1000267007,
		DEPTH_COMPARE_OP = 1000267008,
		DEPTH_BOUNDS_TEST_ENABLE = 1000267009,
		STENCIL_TEST_ENABLE = 1000267010,
		STENCIL_OP = 1000267011,
		RASTERIZER_DISCARD_ENABLE = 1000377001,
		DEPTH_BIAS_ENABLE = 1000377002,
		PRIMITIVE_RESTART_ENABLE = 1000377004,
		VIEWPORT_W_SCALING_NV = 1000087000,
		DISCARD_RECTANGLE_EXT = 1000099000,
		DISCARD_RECTANGLE_ENABLE_EXT = 1000099001,
		DISCARD_RECTANGLE_MODE_EXT = 1000099002,
		SAMPLE_LOCATIONS_EXT = 1000143000,
		RAY_TRACING_PIPELINE_STACK_SIZE_KHR = 1000347000,
		VIEWPORT_SHADING_RATE_PALETTE_NV = 1000164004,
		VIEWPORT_COARSE_SAMPLE_ORDER_NV = 1000164006,
		EXCLUSIVE_SCISSOR_ENABLE_NV = 1000205000,
		EXCLUSIVE_SCISSOR_NV = 1000205001,
		FRAGMENT_SHADING_RATE_KHR = 1000226000,
		LINE_STIPPLE_EXT = 1000259000,
		VERTEX_INPUT_EXT = 1000352000,
		PATCH_CONTROL_POINTS_EXT = 1000377000,
		LOGIC_OP_EXT = 1000377003,
		COLOR_WRITE_ENABLE_EXT = 1000381000,
		TESSELLATION_DOMAIN_ORIGIN_EXT = 1000455002,
		DEPTH_CLAMP_ENABLE_EXT = 1000455003,
		POLYGON_MODE_EXT = 1000455004,
		RASTERIZATION_SAMPLES_EXT = 1000455005,
		SAMPLE_MASK_EXT = 1000455006,
		ALPHA_TO_COVERAGE_ENABLE_EXT = 1000455007,
		ALPHA_TO_ONE_ENABLE_EXT = 1000455008,
		LOGIC_OP_ENABLE_EXT = 1000455009,
		COLOR_BLEND_ENABLE_EXT = 1000455010,
		COLOR_BLEND_EQUATION_EXT = 1000455011,
		COLOR_WRITE_MASK_EXT = 1000455012,
		RASTERIZATION_STREAM_EXT = 1000455013,
		CONSERVATIVE_RASTERIZATION_MODE_EXT = 1000455014,
		EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT = 1000455015,
		DEPTH_CLIP_ENABLE_EXT = 1000455016,
		SAMPLE_LOCATIONS_ENABLE_EXT = 1000455017,
		COLOR_BLEND_ADVANCED_EXT = 1000455018,
		PROVOKING_VERTEX_MODE_EXT = 1000455019,
		LINE_RASTERIZATION_MODE_EXT = 1000455020,
		LINE_STIPPLE_ENABLE_EXT = 1000455021,
		DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT = 1000455022,
		VIEWPORT_W_SCALING_ENABLE_NV = 1000455023,
		VIEWPORT_SWIZZLE_NV = 1000455024,
		COVERAGE_TO_COLOR_ENABLE_NV = 1000455025,
		COVERAGE_TO_COLOR_LOCATION_NV = 1000455026,
		COVERAGE_MODULATION_MODE_NV = 1000455027,
		COVERAGE_MODULATION_TABLE_ENABLE_NV = 1000455028,
		COVERAGE_MODULATION_TABLE_NV = 1000455029,
		SHADING_RATE_IMAGE_ENABLE_NV = 1000455030,
		REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV = 1000455031,
		COVERAGE_REDUCTION_MODE_NV = 1000455032,
		ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT = 1000524000,
		CULL_MODE_EXT = CULL_MODE,
		FRONT_FACE_EXT = FRONT_FACE,
		PRIMITIVE_TOPOLOGY_EXT = PRIMITIVE_TOPOLOGY,
		VIEWPORT_WITH_COUNT_EXT = VIEWPORT_WITH_COUNT,
		SCISSOR_WITH_COUNT_EXT = SCISSOR_WITH_COUNT,
		VERTEX_INPUT_BINDING_STRIDE_EXT = VERTEX_INPUT_BINDING_STRIDE,
		DEPTH_TEST_ENABLE_EXT = DEPTH_TEST_ENABLE,
		DEPTH_WRITE_ENABLE_EXT = DEPTH_WRITE_ENABLE,
		DEPTH_COMPARE_OP_EXT = DEPTH_COMPARE_OP,
		DEPTH_BOUNDS_TEST_ENABLE_EXT = DEPTH_BOUNDS_TEST_ENABLE,
		STENCIL_TEST_ENABLE_EXT = STENCIL_TEST_ENABLE,
		STENCIL_OP_EXT = STENCIL_OP,
		RASTERIZER_DISCARD_ENABLE_EXT = RASTERIZER_DISCARD_ENABLE,
		DEPTH_BIAS_ENABLE_EXT = DEPTH_BIAS_ENABLE,
		PRIMITIVE_RESTART_ENABLE_EXT = PRIMITIVE_RESTART_ENABLE
	}

	public enum VkPolygonMode : int
	{
		Fill = 0,
		Line = 1,
		Point = 2,
		FillRectangleNV = 1000153000
	}

	public enum VkCullMode : int
	{
		NONE = 0,
		FRONT_BIT = 0x00000001,
		BACK_BIT = 0x00000002,
		FRONT_AND_BACK = 0x00000003,
	}

	public enum VkFrontFace : int
	{
		CounterClockwise = 0,
		Clockwise = 1
	}

	public enum VkBlendFactor : int
	{
		ZERO = 0,
		ONE = 1,
		SRC_COLOR = 2,
		ONE_MINUS_SRC_COLOR = 3,
		DST_COLOR = 4,
		ONE_MINUS_DST_COLOR = 5,
		SRC_ALPHA = 6,
		ONE_MINUS_SRC_ALPHA = 7,
		DST_ALPHA = 8,
		ONE_MINUS_DST_ALPHA = 9,
		CONSTANT_COLOR = 10,
		ONE_MINUS_CONSTANT_COLOR = 11,
		CONSTANT_ALPHA = 12,
		ONE_MINUS_CONSTANT_ALPHA = 13,
		SRC_ALPHA_SATURATE = 14,
		SRC1_COLOR = 15,
		ONE_MINUS_SRC1_COLOR = 16,
		SRC1_ALPHA = 17,
		ONE_MINUS_SRC1_ALPHA = 18
	}

	public enum VkBlendOp : int
	{
		ADD = 0,
		SUBTRACT = 1,
		REVERSE_SUBTRACT = 2,
		MIN = 3,
		MAX = 4,
		ZERO_EXT = 1000148000,
		SRC_EXT = 1000148001,
		DST_EXT = 1000148002,
		SRC_OVER_EXT = 1000148003,
		DST_OVER_EXT = 1000148004,
		SRC_IN_EXT = 1000148005,
		DST_IN_EXT = 1000148006,
		SRC_OUT_EXT = 1000148007,
		DST_OUT_EXT = 1000148008,
		SRC_ATOP_EXT = 1000148009,
		DST_ATOP_EXT = 1000148010,
		XOR_EXT = 1000148011,
		MULTIPLY_EXT = 1000148012,
		SCREEN_EXT = 1000148013,
		OVERLAY_EXT = 1000148014,
		DARKEN_EXT = 1000148015,
		LIGHTEN_EXT = 1000148016,
		COLORDODGE_EXT = 1000148017,
		COLORBURN_EXT = 1000148018,
		HARDLIGHT_EXT = 1000148019,
		SOFTLIGHT_EXT = 1000148020,
		DIFFERENCE_EXT = 1000148021,
		EXCLUSION_EXT = 1000148022,
		INVERT_EXT = 1000148023,
		INVERT_RGB_EXT = 1000148024,
		LINEARDODGE_EXT = 1000148025,
		LINEARBURN_EXT = 1000148026,
		VIVIDLIGHT_EXT = 1000148027,
		LINEARLIGHT_EXT = 1000148028,
		PINLIGHT_EXT = 1000148029,
		HARDMIX_EXT = 1000148030,
		HSL_HUE_EXT = 1000148031,
		HSL_SATURATION_EXT = 1000148032,
		HSL_COLOR_EXT = 1000148033,
		HSL_LUMINOSITY_EXT = 1000148034,
		PLUS_EXT = 1000148035,
		PLUS_CLAMPED_EXT = 1000148036,
		PLUS_CLAMPED_ALPHA_EXT = 1000148037,
		PLUS_DARKER_EXT = 1000148038,
		MINUS_EXT = 1000148039,
		MINUS_CLAMPED_EXT = 1000148040,
		CONTRAST_EXT = 1000148041,
		INVERT_OVG_EXT = 1000148042,
		RED_EXT = 1000148043,
		GREEN_EXT = 1000148044,
		BLUE_EXT = 1000148045
	}

	[Flags]
	public enum VkColorComponentFlagBits : int
	{
		R = 0x00000001,
		G = 0x00000002,
		B = 0x00000004,
		A = 0x00000008
	}

	public enum VkLogicOp : int
	{
		CLEAR = 0,
		AND = 1,
		AND_REVERSE = 2,
		COPY = 3,
		AND_INVERTED = 4,
		NO_OP = 5,
		XOR = 6,
		OR = 7,
		NOR = 8,
		EQUIVALENT = 9,
		INVERT = 10,
		OR_REVERSE = 11,
		COPY_INVERTED = 12,
		OR_INVERTED = 13,
		NAND = 14,
		SET = 15
	}

	public enum VkCompareOp : int
	{
		NEVER = 0,
		LESS = 1,
		EQUAL = 2,
		LESS_OR_EQUAL = 3,
		GREATER = 4,
		NOT_EQUAL = 5,
		GREATER_OR_EQUAL = 6,
		ALWAYS = 7
	}

	public enum VkStencilOp : int
	{
		VK_STENCIL_OP_KEEP = 0,
		VK_STENCIL_OP_ZERO = 1,
		VK_STENCIL_OP_REPLACE = 2,
		VK_STENCIL_OP_INCREMENT_AND_CLAMP = 3,
		VK_STENCIL_OP_DECREMENT_AND_CLAMP = 4,
		VK_STENCIL_OP_INVERT = 5,
		VK_STENCIL_OP_INCREMENT_AND_WRAP = 6,
		VK_STENCIL_OP_DECREMENT_AND_WRAP = 7,
		VK_STENCIL_OP_MAX_ENUM = 0x7FFFFFFF
	}

	public enum VkCommandPoolCreateFlag : int
	{
		Transient = 0x00000001,
		ResetCommandBuffer = 0x00000002,
		Protected = 0x00000004
	}

	public enum VkCommandBufferLevel : int
	{
		Primary = 0,
		Secondary = 1
	}

	public enum VkFenceCreateFlags : int
	{
		None = 0,
		Signaled = 0x00000001
	}

	public enum VkResult : int
	{
		Success = 0,
		NotReady = 1,
		Timeout = 2,
		EventSet = 3,
		EventReset = 4,
		Incomplete = 5,
		ErrorOutOfHostMemory = -1,
		ErrorOutOfDeviceMemory = -2,
		InitializationFailed = -3,
		DeviceLost = -4,
		MemoryMapFailed = -5,
		LayerNotPresent = -6,
		ExtensionNotPresent = -7,
		FeatureNotPresent = -8,
		IncompatibleDriver = -9,
		TooManyObjects = -10,
		FormatNotSupported = -11,
		SurfaceLost = -1000000000,
		Suboptimal = 1000001003,
		ErrorOutOfDate = -1000001004,
		ErrorIncompatibleDisplay = -1000003001,
		ErrorNativeWindowInUse = -1000000001,
		ErrorValidationFailed = -1000011001
	}

	static class VkResultExtension
	{
		public static void CheckForSuccess(this VkResult result, string context = "")
		{
			if (result != VkResult.Success)
				Log.Error($"Vulkan error '{Enum.GetName(typeof(VkResult), result)}" +
					(string.IsNullOrEmpty(context) ? string.Empty : $" - {context}"));
		}
	}

	public enum VkCommandBufferResetFlag : int
	{
		None = 0,
		ReleaseResources = 0x00000001
	}

	public enum VkBufferUsage : int
	{
		TransferSource = 0x00000001,
		TransferDestination = 0x00000002,
		UniformTexel = 0x00000004,
		StorageTexel = 0x00000008,
		Uniform = 0x00000010,
		Storage = 0x00000020,
		Index = 0x00000040,
		Vertex = 0x00000080,
		Indirect = 0x00000100,
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT = 0x00020000,
		VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR = 0x00002000,
		VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR = 0x00004000,
		VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT = 0x00000800,
		VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT = 0x00001000,
		VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT = 0x00000200,
		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR = 0x00080000,
		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR = 0x00100000,
		VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR = 0x00000400,
		VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT = 0x00200000,
		VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT = 0x00400000,
		VK_BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT = 0x04000000,
		VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT = 0x00800000,
		VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT = 0x01000000,
		VK_BUFFER_USAGE_RAY_TRACING_BIT_NV = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
	}

	public enum VkSharingMode : int
	{
		Exclusive = 0,
		Concurrent = 1
	}

	[Flags]
	public enum VkMemoryProperty : int
	{
		DeviceLocal = 0x00000001,
		HostVisible = 0x00000002,
		HostCoherent = 0x00000004,
		HostCached = 0x00000008,
		LazilyAllocated = 0x00000010,
		Protected = 0x00000020,
		DeviceCoherent_AMD = 0x00000040,
		DeviceUncached_AMD = 0x00000080,
		RDMACapable_NV = 0x00000100
	}

	public enum VkCommandBufferUsage : int
	{
		OneTimeSubmit = 0x00000001,
		RenderPassContinue = 0x00000002,
		SimultaneousUse = 0x00000004
	}

	public enum VkIndexType : int
	{
		UINT16 = 0,
		UINT32 = 1,
		NONE_KHR = 1000165000,
		UINT8_EXT = 1000265000,
		NONE_NV = NONE_KHR,
		MAX_ENUM = 0x7FFFFFFF
	}

	public enum VkDescriptorType : int
	{
		SAMPLER = 0,
		COMBINED_IMAGE_SAMPLER = 1,
		SAMPLED_IMAGE = 2,
		STORAGE_IMAGE = 3,
		UNIFORM_TEXEL_BUFFER = 4,
		STORAGE_TEXEL_BUFFER = 5,
		UNIFORM_BUFFER = 6,
		STORAGE_BUFFER = 7,
		UNIFORM_BUFFER_DYNAMIC = 8,
		STORAGE_BUFFER_DYNAMIC = 9,
		INPUT_ATTACHMENT = 10,
		INLINE_UNIFORM_BLOCK = 1000138000,
		MAX_ENUM = 0x7FFFFFFF
	}

	public enum VkImageType : int
	{
		Type1D = 0,
		Type2D = 1,
		Type3D = 2
	}

	public enum VkImageTiling : int
	{
		Optimal = 0,
		Linear = 1,
		VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT = 1000158000
	}

	[Flags]
	public enum VkImageUsage : int
	{
		TransferSrc = 0x00000001,
		TransferDst = 0x00000002,
		Sampled = 0x00000004,
		Storage = 0x00000008,
		ColorAttachment = 0x00000010,
		DepthStencilAttachment = 0x00000020,
		TransientAttachment = 0x00000040,
		InputAttachment = 0x00000080,
		VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR = 0x00000400,
		VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR = 0x00000800,
		VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR = 0x00001000,
		VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT = 0x00000200,
		VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR = 0x00000100,
		VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT = 0x00400000,
		VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT = 0x00080000,
		VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI = 0x00040000,
		VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM = 0x00100000,
		VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM = 0x00200000,
		VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV = VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
		VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
	}

	[Flags]
	public enum VkImageAspectFlags : int
	{
		Color = 0x00000001,
		Depth = 0x00000002,
		Stencil = 0x00000004,
		Metadata = 0x00000008,
		Plane0 = 0x00000010,
		Plane1 = 0x00000020,
		Plane2 = 0x00000040,
		None = 0,
		MEMORY_PLANE_0_BIT_EXT = 0x00000080,
		MEMORY_PLANE_1_BIT_EXT = 0x00000100,
		MEMORY_PLANE_2_BIT_EXT = 0x00000200,
		MEMORY_PLANE_3_BIT_EXT = 0x00000400,
		PLANE_0_BIT_KHR = Plane0,
		PLANE_1_BIT_KHR = Plane1,
		PLANE_2_BIT_KHR = Plane2,
		NONE_KHR = None
	}

	public enum VkFilter : int
	{
		Nearest = 0,
		Linear = 1,
		Cubic_EXT = 1000015000,
		Cubic_IMG = Cubic_EXT,
	}

	public enum VkBorderColor : int
	{
		FloatTransparentBlack = 0,
		IntTransparentBlack = 1,
		FloatOpaqueBlack = 2,
		IntOpaqueBlack = 3,
		FloatOpaqueWhite = 4,
		IntOpaqueWhite = 5,
		FloatCustom_EXT = 1000287003,
		IntCustom_EXT = 1000287004
	}

	public enum VkSamplerMipmapMode : int
	{
		Nearest = 0,
		Linear = 1
	}

	public enum VkSamplerAddressMode : int
	{
		Repeat = 0,
		MirroredRepeat = 1,
		ClampToEdge = 2,
		ClampToBorder = 3,
		MirrorClampToEdge = 4,
		MirrorClampToEdge_KHR = MirrorClampToEdge
	}

	public enum VkImageCreateFlag : int
	{
		SPARSE_BINDING_BIT = 0x00000001,
		SPARSE_RESIDENCY_BIT = 0x00000002,
		SPARSE_ALIASED_BIT = 0x00000004,
		MUTABLE_FORMAT_BIT = 0x00000008,
		CUBE_COMPATIBLE_BIT = 0x00000010,
		ALIAS_BIT = 0x00000400,
		SPLIT_INSTANCE_BIND_REGIONS_BIT = 0x00000040,
		_2D_ARRAY_COMPATIBLE_BIT = 0x00000020,
		BLOCK_TEXEL_VIEW_COMPATIBLE_BIT = 0x00000080,
		EXTENDED_USAGE_BIT = 0x00000100,
		PROTECTED_BIT = 0x00000800,
		DISJOINT_BIT = 0x00000200,
		CORNER_SAMPLED_BIT_NV = 0x00002000,
		SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT = 0x00001000,
		SUBSAMPLED_BIT_EXT = 0x00004000,
		DESCRIPTOR_BUFFER_CAPTURE_REPLAY_BIT_EXT = 0x00010000,
		MULTISAMPLED_RENDER_TO_SINGLE_SAMPLED_BIT_EXT = 0x00040000,
		_2D_VIEW_COMPATIBLE_BIT_EXT = 0x00020000,
		FRAGMENT_DENSITY_MAP_OFFSET_BIT_QCOM = 0x00008000,
		SPLIT_INSTANCE_BIND_REGIONS_BIT_KHR = SPLIT_INSTANCE_BIND_REGIONS_BIT,
		_2D_ARRAY_COMPATIBLE_BIT_KHR = _2D_ARRAY_COMPATIBLE_BIT,
		BLOCK_TEXEL_VIEW_COMPATIBLE_BIT_KHR = BLOCK_TEXEL_VIEW_COMPATIBLE_BIT,
		EXTENDED_USAGE_BIT_KHR = EXTENDED_USAGE_BIT,
		DISJOINT_BIT_KHR = DISJOINT_BIT,
		ALIAS_BIT_KHR = ALIAS_BIT
	}

	public enum VkImageViewType : int
	{
		_1D = 0,
		_2D = 1,
		_3D = 2,
		Cube = 3,
		_1D_ARRAY = 4,
		_2D_ARRAY = 5,
		CubeArray = 6
	}

	public enum VkComponentSwizzle : int
	{
		Identity = 0,
		Zero = 1,
		One = 2,
		R = 3,
		G = 4,
		B = 5,
		A = 6
	}
}