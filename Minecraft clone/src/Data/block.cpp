#include "block.h"
#include "Game/config.h"
#include <glm/gtx/euler_angles.hpp>

/**
 * NORMAL BLOCKS
 */
static float upQuadVerts[] = {
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
};

static float bottomQuadVerts[] = {
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f
};

static float frontQuadVerts[] = {
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

static float backQuadVerts[] = {
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f
};

static float leftQuadVerts[] = {
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f
};

static float rightQuadVerts[] = {
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
};

static uint8_t quadIndices[] = {
	0, 1, 2, 0, 2, 3
};

/**
 * SLABS
 */
static float frontUpperSlabVerts[] = {
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.0f, 0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

static float frontLowerSlabVerts[] = {
	-0.5f, 0.0f, 0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, 0.0f, 1.0f
};

static float backUpperSlabVerts[] = {
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.0f, -0.5f, 1.0f, 0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f
};

static float backLowerSlabVerts[] = {
	0.5f, 0.0f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.0f, -0.5f, 1.0f, 0.5f, 0.0f, 0.0f, -1.0f
};

static float leftUpperSlabVerts[] = {
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.0f, 0.5f, 1.0f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f
};

static float leftLowerSlabVerts[] = {
	-0.5f, 0.0f, -0.5f, 0.0f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.0f, 0.5f, 1.0f, 0.5f, -1.0f, 0.0f, 0.0f
};

static float rightUpperSlabVerts[] = {
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.0f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
};

static float rightLowerSlabVerts[] = {
	0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
};

static float bottomUpperSlabVerts[] = {
	0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	-0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
	-0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
	0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f
};

static float upLowerSlabVerts[] = {
	-0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
};

static Face upperSlabFrontFace = {
	.vertices = frontUpperSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::UpperSlabOrUpsideDownStair
};

static Face upperSlabBackFace = {
	.vertices = backUpperSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::UpperSlabOrUpsideDownStair
};

static Face upperSlabLeftFace = {
	.vertices = leftUpperSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::UpperSlabOrUpsideDownStair
};

static Face upperSlabRightFace = {
	.vertices = rightUpperSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::UpperSlabOrUpsideDownStair
};

static Face upperSlabBottomFace = {
	.vertices = bottomUpperSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::Null
};

static Face lowerSlabFrontFace = {
	.vertices = frontLowerSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::LowerSlabOrRightSideUpStair
};

static Face lowerSlabBackFace = {
	.vertices = backLowerSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::LowerSlabOrRightSideUpStair
};

static Face lowerSlabLeftFace = {
	.vertices = leftLowerSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::LowerSlabOrRightSideUpStair
};

static Face lowerSlabRightFace = {
	.vertices = rightLowerSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::LowerSlabOrRightSideUpStair
};

static Face lowerSlabUpFace = {
	.vertices = upLowerSlabVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::Null
};

static Face quadUpFace = {
	.vertices = upQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = true,
	.cullMode = CullMode::Null
};

static Face quadBottomFace = {
	.vertices = bottomQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = true,
	.cullMode = CullMode::Null
};

static Face quadFrontFace = {
	.vertices = frontQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = true,
	.cullMode = CullMode::Null
};

static Face quadBackFace = {
	.vertices = backQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = true,
	.cullMode = CullMode::Null
};

static Face quadLeftFace = {
	.vertices = leftQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = true,
	.cullMode = CullMode::Null
};

static Face quadRightFace = {
	.vertices = rightQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = true,
	.cullMode = CullMode::Null
};

static Model cubeModel = {
	&quadUpFace, &quadBottomFace, &quadFrontFace, &quadBackFace, &quadLeftFace, &quadRightFace, nullptr
};

static Model lowerSlabModel = {
	.faces = { nullptr, &quadBottomFace, &lowerSlabFrontFace, &lowerSlabBackFace, &lowerSlabLeftFace, &lowerSlabRightFace, &lowerSlabUpFace }
};

static Model upperSlabModel = {
	.faces = { &quadUpFace, nullptr, &upperSlabFrontFace, &upperSlabBackFace, &upperSlabLeftFace, &upperSlabRightFace, &upperSlabBottomFace }
};

static TexturedModel doubleStoneSlabModel[3] = { {
		.model = &cubeModel,
		.textureId = { TextureId::StoneSlabTop, TextureId::StoneSlabTop, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::Missing }
	}, {
		.model = &lowerSlabModel,
		.textureId = { TextureId::Missing, TextureId::StoneSlabTop, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabTop }
	}, {
		.model = &upperSlabModel,
		.textureId = { TextureId::StoneSlabTop, TextureId::Missing, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabSide, TextureId::StoneSlabTop }
	}
};

static TexturedModel stoneModel = {
	.model = &cubeModel,
	.textureId = { TextureId::Stone, TextureId::Stone, TextureId::Stone, TextureId::Stone, TextureId::Stone, TextureId::Stone }
};

static TexturedModel dirtModel = {
	.model = &cubeModel,
	.textureId = { TextureId::Dirt, TextureId::Dirt, TextureId::Dirt, TextureId::Dirt, TextureId::Dirt, TextureId::Dirt }
};

static TexturedModel grassBlockModel = {
	.model = &cubeModel,
	.textureId = { TextureId::GrassTop, TextureId::Dirt, TextureId::GrassSide, TextureId::GrassSide, TextureId::GrassSide, TextureId::GrassSide }
};

static TexturedModel acaciaPlanksModel[3] = { {
		.model = &cubeModel,
		.textureId = { TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks }
	}, {
		.model = &lowerSlabModel,
		.textureId = { TextureId::Missing, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks }
	}, {
		.model = &upperSlabModel,
		.textureId = { TextureId::AcaciaPlanks, TextureId::Missing, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks, TextureId::AcaciaPlanks }
	}
};

static TexturedModel birchPlanksModel[3] = { {
		.model = &cubeModel,
		.textureId = { TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks }
	}, {
		.model = &lowerSlabModel,
		.textureId = { TextureId::Missing, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks }
	}, {
		.model = &upperSlabModel,
		.textureId = { TextureId::BirchPlanks, TextureId::Missing, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks, TextureId::BirchPlanks }
	}
};

static TexturedModel oakPlanksModel[3] = { {
		.model = &cubeModel,
		.textureId = { TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks }
	}, {
		.model = &lowerSlabModel,
		.textureId = { TextureId::Missing, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks }
	}, {
		.model = &upperSlabModel,
		.textureId = { TextureId::OakPlanks, TextureId::Missing, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks, TextureId::OakPlanks }
	}
};

static TexturedModel sprucePlanksModel[3] = { {
		.model = &cubeModel,
		.textureId = { TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks }
	}, {
		.model = &upperSlabModel,
		.textureId = { TextureId::SprucePlanks, TextureId::Missing, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks }
	}, {
		.model = &lowerSlabModel,
		.textureId = { TextureId::Missing, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks, TextureId::SprucePlanks }
	}
};

static TexturedModel ironBlockModel = {
	.model = &cubeModel,
	.textureId = { TextureId::IronBlock, TextureId::IronBlock, TextureId::IronBlock, TextureId::IronBlock, TextureId::IronBlock, TextureId::IronBlock, TextureId::Missing }
};

static TexturedModel goldBlockModel = {
	.model = &cubeModel,
	.textureId = { TextureId::GoldBlock, TextureId::GoldBlock, TextureId::GoldBlock, TextureId::GoldBlock, TextureId::GoldBlock, TextureId::GoldBlock, TextureId::Missing }
};

static TexturedModel diamondBlockModel = {
	.model = &cubeModel,
	.textureId = { TextureId::DiamondBlock, TextureId::DiamondBlock, TextureId::DiamondBlock, TextureId::DiamondBlock, TextureId::DiamondBlock, TextureId::DiamondBlock, TextureId::Missing }
};

static TexturedModel cobblestoneModel[3] = { {
		.model = &cubeModel,
		.textureId = { TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Missing }
	}, {
		.model = &lowerSlabModel,
		.textureId = { TextureId::Missing, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone }
	}, {
		.model = &upperSlabModel,
		.textureId = { TextureId::Cobblestone, TextureId::Missing, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone, TextureId::Cobblestone }
	}
};

static TexturedModel redstoneBlockModel = {
	.model = &cubeModel,
	.textureId = { TextureId::RedstoneBlock, TextureId::RedstoneBlock, TextureId::RedstoneBlock, TextureId::RedstoneBlock, TextureId::RedstoneBlock, TextureId::RedstoneBlock, TextureId::Missing }
};

static TexturedModel acaciaLogModel = {
	.model = &cubeModel,
	.textureId = { TextureId::AcaciaLogTop, TextureId::AcaciaLogTop, TextureId::AcaciaLog, TextureId::AcaciaLog, TextureId::AcaciaLog, TextureId::AcaciaLog, TextureId::Missing }
};

static TexturedModel birchLogModel = {
	.model = &cubeModel,
	.textureId = { TextureId::BirchLogTop, TextureId::BirchLogTop, TextureId::BirchLog, TextureId::BirchLog, TextureId::BirchLog, TextureId::BirchLog, TextureId::Missing }
};

static TexturedModel oakLogModel = {
	.model = &cubeModel,
	.textureId = { TextureId::OakLogTop, TextureId::OakLogTop, TextureId::OakLog, TextureId::OakLog, TextureId::OakLog, TextureId::OakLog, TextureId::Missing }
};

static TexturedModel spruceLogModel = {
	.model = &cubeModel,
	.textureId = { TextureId::SpruceLogTop, TextureId::SpruceLogTop, TextureId::SpruceLog, TextureId::SpruceLog, TextureId::SpruceLog, TextureId::SpruceLog, TextureId::Missing }
};

static TexturedModel glowstoneModel = {
	.model = &cubeModel,
	.textureId = { TextureId::Glowstone, TextureId::Glowstone, TextureId::Glowstone, TextureId::Glowstone, TextureId::Glowstone, TextureId::Glowstone, TextureId::Missing }
};

/**
 * GLASSES
 */
	static Face glassUpFace = {
		.vertices = upQuadVerts,
		.indices = quadIndices,
		.numVertices = 4,
		.numIndices = 6,
		.opaque = false,
		.cullMode = CullMode::SameBlock
};

static Face glassBottomFace = {
	.vertices = bottomQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::SameBlock
};

static Face glassFrontFace = {
	.vertices = frontQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::SameBlock
};

static Face glassBackFace = {
	.vertices = backQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::SameBlock
};

static Face glassLeftFace = {
	.vertices = leftQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::SameBlock
};

static Face glassRightFace = {
	.vertices = rightQuadVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::SameBlock
};

static Model transparentModel = {
	&glassUpFace, &glassBottomFace, &glassFrontFace, &glassBackFace, &glassLeftFace, &glassRightFace, nullptr
};

static TexturedModel glassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::Glass, TextureId::Glass, TextureId::Glass, TextureId::Glass, TextureId::Glass, TextureId::Glass, TextureId::Missing }
};

static TexturedModel blackGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::BlackGlass, TextureId::BlackGlass, TextureId::BlackGlass, TextureId::BlackGlass, TextureId::BlackGlass, TextureId::BlackGlass, TextureId::Missing }
};

static TexturedModel blueGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::BlueGlass, TextureId::BlueGlass, TextureId::BlueGlass, TextureId::BlueGlass, TextureId::BlueGlass, TextureId::BlueGlass, TextureId::Missing }
};

static TexturedModel brownGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::BrownGlass, TextureId::BrownGlass, TextureId::BrownGlass, TextureId::BrownGlass, TextureId::BrownGlass, TextureId::BrownGlass, TextureId::Missing }
};

static TexturedModel cyanGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::CyanGlass, TextureId::CyanGlass, TextureId::CyanGlass, TextureId::CyanGlass, TextureId::CyanGlass, TextureId::CyanGlass, TextureId::Missing }
};

static TexturedModel grayGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::GrayGlass, TextureId::GrayGlass, TextureId::GrayGlass, TextureId::GrayGlass, TextureId::GrayGlass, TextureId::GrayGlass, TextureId::Missing }
};

static TexturedModel greenGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::GreenGlass, TextureId::GreenGlass, TextureId::GreenGlass, TextureId::GreenGlass, TextureId::GreenGlass, TextureId::GreenGlass, TextureId::Missing }
};

static TexturedModel lightBlueGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::LightBlueGlass, TextureId::LightBlueGlass, TextureId::LightBlueGlass, TextureId::LightBlueGlass, TextureId::LightBlueGlass, TextureId::LightBlueGlass, TextureId::Missing }
};

static TexturedModel lightGrayGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::LightGrayGlass, TextureId::LightGrayGlass, TextureId::LightGrayGlass, TextureId::LightGrayGlass, TextureId::LightGrayGlass, TextureId::LightGrayGlass, TextureId::Missing }
};

static TexturedModel limeGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::LimeGlass, TextureId::LimeGlass, TextureId::LimeGlass, TextureId::LimeGlass, TextureId::LimeGlass, TextureId::LimeGlass, TextureId::Missing }
};

static TexturedModel magentaGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::MagentaGlass, TextureId::MagentaGlass, TextureId::MagentaGlass, TextureId::MagentaGlass, TextureId::MagentaGlass, TextureId::MagentaGlass, TextureId::Missing }
};

static TexturedModel orangeGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::OrangeGlass, TextureId::OrangeGlass, TextureId::OrangeGlass, TextureId::OrangeGlass, TextureId::OrangeGlass, TextureId::OrangeGlass, TextureId::Missing }
};

static TexturedModel pinkGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::PinkGlass, TextureId::PinkGlass, TextureId::PinkGlass, TextureId::PinkGlass, TextureId::PinkGlass, TextureId::PinkGlass, TextureId::Missing }
};

static TexturedModel purpleGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::PurpleGlass, TextureId::PurpleGlass, TextureId::PurpleGlass, TextureId::PurpleGlass, TextureId::PurpleGlass, TextureId::PurpleGlass, TextureId::Missing }
};

static TexturedModel redGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::RedGlass, TextureId::RedGlass, TextureId::RedGlass, TextureId::RedGlass, TextureId::RedGlass, TextureId::RedGlass, TextureId::Missing }
};

static TexturedModel whiteGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::WhiteGlass, TextureId::WhiteGlass, TextureId::WhiteGlass, TextureId::WhiteGlass, TextureId::WhiteGlass, TextureId::WhiteGlass, TextureId::Missing }
};

static TexturedModel yellowGlassModel = {
	.model = &transparentModel,
	.textureId = { TextureId::YellowGlass, TextureId::YellowGlass, TextureId::YellowGlass, TextureId::YellowGlass, TextureId::YellowGlass, TextureId::YellowGlass, TextureId::Missing }
};

/**
 * MULTI-TEXTURE BLOCKS
 */
static TexturedModel redstoneLampModel[2] = { {
		.model = &cubeModel,
		.textureId = { TextureId::RedstoneLamp, TextureId::RedstoneLamp, TextureId::RedstoneLamp, TextureId::RedstoneLamp, TextureId::RedstoneLamp, TextureId::RedstoneLamp, TextureId::Missing }
	}, {
		.model = &cubeModel,
		.textureId = { TextureId::LitRedstoneLamp, TextureId::LitRedstoneLamp, TextureId::LitRedstoneLamp, TextureId::LitRedstoneLamp, TextureId::LitRedstoneLamp, TextureId::LitRedstoneLamp, TextureId::Missing }
	}
};

/**
 * MISC
 */
static float redstoneVerts[] = {
	-0.25f, -0.49f, -0.25f, 0.25f, 0.75f, 0.0f, 1.0f, 0.0f,
	-0.25f, -0.49f, 0.25f, 0.25f, 0.25f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.49f, 0.25f, 0.75f, 0.25f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.49f, -0.25f, 0.75f, 0.75f, 0.0f, 1.0f, 0.0f
};

static float redstoneFrontVerts[] = {
	-0.25f, -0.49f, 0.0f, 0.25f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.25f, -0.49f, 0.5f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.49f, 0.5f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.49f, 0.0f, 0.75f, 0.5f, 0.0f, 1.0f, 0.0f
};

static float redstoneFrontUpVerts[] = {
	-0.25f, -0.49f, 0.0f, 0.25f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.25f, -0.49f, 0.5f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.49f, 0.5f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.49f, 0.0f, 0.75f, 0.5f, 0.0f, 1.0f, 0.0f,
	0.25f, 0.5f, 0.49f, 0.25f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.25f, -0.5f, 0.49f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.25f, -0.5f, 0.49f, 0.75f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.25f, 0.5f, 0.49f, 0.75f, 1.0f, 0.0f, 1.0f, 0.0f
};

static uint8_t redstoneFrontUpIndices[] = {
	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7
};

static Face redstoneFace = {
	.vertices = redstoneVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::Null
};

static Face redstoneFrontFace = {
	.vertices = redstoneFrontVerts,
	.indices = quadIndices,
	.numVertices = 4,
	.numIndices = 6,
	.opaque = false,
	.cullMode = CullMode::Null
};

static Face redstoneFrontUpFace = {
	.vertices = redstoneFrontUpVerts,
	.indices = redstoneFrontUpIndices,
	.numVertices = 8,
	.numIndices = 12,
	.opaque = false,
	.cullMode = CullMode::Null
};

static Model redstoneDotModel = {
	.faces = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &redstoneFace }
};

static Model redstoneFrontModel = {
	.faces = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &redstoneFrontFace }
};

static Model redstoneFrontUpModel = {
	.faces = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &redstoneFrontUpFace }
};

static TexturedModel redstoneModel[4] = { {
		.model = &redstoneDotModel,
		.textureId = { TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::RedstoneDot }
	}, {
		.model = &redstoneFrontModel,
		.textureId = { TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::RedstoneLine }
	}, {
		.model = &redstoneFrontUpModel,
		.textureId = { TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::RedstoneLine }
	}, {
		.model = &redstoneFrontModel,
		.textureId = { TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::Missing, TextureId::RedstoneLine }
	}
};

glm::mat4 leftRotation = glm::eulerAngleY(glm::radians(-90.0f));
glm::mat4 rightRotation = glm::eulerAngleY(glm::radians(90.0f));
glm::mat4 backRotation = glm::eulerAngleY(glm::radians(180.0f));

BlockDesctiptor blockDescriptors[Block::Unknown]
{
	{ BlockType::Null, 0, 0, 0, nullptr }, // Air
	{ BlockType::Normal, 0, 15, 0, &stoneModel }, // Stone
	{ BlockType::Normal, 0, 15, 0, &dirtModel }, // Dirt
	{ BlockType::Normal, 0, 15, 0, &grassBlockModel }, // Grass Block
	{ BlockType::Normal, 0, 15, 0, acaciaPlanksModel }, // Acacia Planks
	{ BlockType::Normal, 0, 15, 0, birchPlanksModel }, // Birch Planks
	{ BlockType::Normal, 0, 15, 0, oakPlanksModel }, // Oak Planks
	{ BlockType::Normal, 0, 15, 0, sprucePlanksModel }, // Spruce Planks
	{ BlockType::Normal, 0, 15, 0, &ironBlockModel }, // Iron Block
	{ BlockType::Normal, 0, 15, 0, &goldBlockModel }, // Gold Block
	{ BlockType::Normal, 0, 15, 0, &diamondBlockModel }, // Diamond Block
	{ BlockType::Normal, 0, 15, 0, cobblestoneModel }, // Cobblestone Block
	{ BlockType::Normal, 0, 15, 0, &redstoneBlockModel }, // Redstone Block
	{ BlockType::Transparent, 0, 0, 0, &glassModel }, // Glass
	{ BlockType::Transparent, 0, 1, 0, &blackGlassModel }, // Black Glass
	{ BlockType::Transparent, 0, 1, 0, &blueGlassModel }, // Blue Glass
	{ BlockType::Transparent, 0, 1, 0, &brownGlassModel }, // Brown Glass
	{ BlockType::Transparent, 0, 1, 0, &cyanGlassModel }, // Cyan Glass
	{ BlockType::Transparent, 0, 1, 0, &grayGlassModel }, // Gray Glass
	{ BlockType::Transparent, 0, 1, 0, &greenGlassModel }, // Green Glass
	{ BlockType::Transparent, 0, 1, 0, &lightBlueGlassModel }, // Light Blue Glass
	{ BlockType::Transparent, 0, 1, 0, &lightGrayGlassModel }, // Light Gray Glass
	{ BlockType::Transparent, 0, 1, 0, &limeGlassModel }, // Lime Glass
	{ BlockType::Transparent, 0, 1, 0, &magentaGlassModel }, // Magenta Glass
	{ BlockType::Transparent, 0, 1, 0, &orangeGlassModel }, // Orange Glass
	{ BlockType::Transparent, 0, 1, 0, &pinkGlassModel }, // Pink Glass
	{ BlockType::Transparent, 0, 1, 0, &purpleGlassModel }, // Purple Glass
	{ BlockType::Transparent, 0, 1, 0, &redGlassModel }, // Red Glass
	{ BlockType::Transparent, 0, 1, 0, &whiteGlassModel }, // White Glass
	{ BlockType::Transparent, 0, 1, 0, &yellowGlassModel }, // Yellow Glass
	{ BlockType::ThreeDirectional, 0, 15, DirectionalMetadata::YAxis, &acaciaLogModel }, // Acacia Log
	{ BlockType::ThreeDirectional, 0, 15, DirectionalMetadata::YAxis, &birchLogModel }, // Birch Log
	{ BlockType::ThreeDirectional, 0, 15, DirectionalMetadata::YAxis, &oakLogModel }, // Oak Log
	{ BlockType::ThreeDirectional, 0, 15, DirectionalMetadata::YAxis, &spruceLogModel }, // Spruce Log
	{ BlockType::MultiTextured, 87, 15, 0, redstoneLampModel }, // Redstone Lamp
	{ BlockType::Slab, 0, 7, SlabMetadata::Lower, cobblestoneModel }, // Cobblestone Slab
	{ BlockType::Slab, 0, 7, SlabMetadata::Lower, doubleStoneSlabModel }, // Stone Slab
	{ BlockType::Slab, 0, 7, SlabMetadata::Lower, acaciaPlanksModel }, // Acacia Slab
	{ BlockType::Slab, 0, 7, SlabMetadata::Lower, birchPlanksModel }, // Birch Slab
	{ BlockType::Slab, 0, 7, SlabMetadata::Lower, oakPlanksModel }, // Oak Slab
	{ BlockType::Slab, 0, 7, SlabMetadata::Lower, sprucePlanksModel }, // Spruce Slab
	{ BlockType::Connectable, 0, 0, 0, redstoneModel }, // Redstone
	{ BlockType::Normal, 15, 15, 0, &glowstoneModel } // Glowstone
};

BoundingBox const& Block::GetBoundingBox() const
{
	static BoundingBox nullBoundingBox = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };
	static BoundingBox fullBlockBoundingBox = { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f) };
	static BoundingBox upperSlabBoundingBox = { glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f) };
	static BoundingBox lowerSlabBoundingBox = { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f) };
	static BoundingBox redstoneBoundingBox = { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.4f, 0.5f) };
	switch (GetDescriptor().type)
	{
		case BlockType::Normal:
		case BlockType::Transparent:
		case BlockType::ThreeDirectional:
		case BlockType::MultiTextured:
			return fullBlockBoundingBox;
		case BlockType::Slab:
		{
			switch (Metadata().As<SlabMetadata>().SlabType())
			{
				case SlabMetadata::Lower: return lowerSlabBoundingBox;
				case SlabMetadata::Upper: return upperSlabBoundingBox;
				default: return fullBlockBoundingBox;
			}
		}
		default:
		{
			if (id == Redstone)
				return redstoneBoundingBox;
		}
	}
	return nullBoundingBox;
}

/**
 * 1 - no face at all
 * 0 - opaque face
 * -1 - non-opaque
 */
int8_t Block::GetFaceState(uint8_t facing) const
{
	if (id == Air)
		return 1;
	switch (GetDescriptor().type)
	{
		case BlockType::Normal:
		case BlockType::ThreeDirectional:
		case BlockType::MultiTextured:
			return 0;
		case BlockType::Transparent:
			return -1;
		case BlockType::Slab:
		{
			switch (Metadata().As<SlabMetadata>().SlabType())
			{
				case SlabMetadata::Lower:
				{
					switch (facing)
					{
						case Facing::Down: return 0;
						case Facing::Up: return 1;
						default: return -1;
					}
				}
				case SlabMetadata::Upper:
				{
					switch (facing)
					{
						case Facing::Up: return 0;
						case Facing::Down: return 1;
						default: return -1;
					}
				}
				default: return 0;
			}
		}
	}
	return 1;
}