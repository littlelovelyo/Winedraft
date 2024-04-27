#pragma once
#include <concepts>
#include <glex.h>

/**
 * For transparent blocks:
 * +---------+
 * | 16 bits |
 * +---------+
 */
class Metadata
{
protected:
	uint16_t m_value;
public:
	constexpr Metadata(uint16_t value) : m_value(value) {}
	uint16_t Value() const {
		return m_value;
	}
	template <typename T> requires std::derived_from<T, Metadata>
	T& As() {
		return *reinterpret_cast<T*>(this);
	}
	template <typename T> requires std::derived_from<T, Metadata>
	T const& As() const {
		return *reinterpret_cast<T const*>(this);
	}
};

/**
 * For normal blocks:
 * +----------------------+---------+
 * | POWER_STATE (2 bits) | 14 bits |
 * +----------------------+---------+
 */
class PowerableMetadata : public Metadata
{
protected:
	constexpr static uint16_t PowerMask = 0xc000;
	constexpr static uint16_t UnpowerMask = 0x3fff;
	constexpr static uint16_t WeakPowerMask = 0x4000;
	constexpr static uint16_t StrongPowerMask = 0x8000;
public:
	bool IsPowered() const {
		return m_value & PowerMask;
	}
	bool IsStronglyPowered() const {
		return m_value & StrongPowerMask;
	}
	bool IsWeaklyPowered() const {
		return m_value & WeakPowerMask;
	}
	void PowerStrongly() {
		m_value = m_value & UnpowerMask | StrongPowerMask;
	}
	void PowerWeakly() {
		m_value = m_value & UnpowerMask | WeakPowerMask;
	}
	void Unpower() {
		m_value = m_value & UnpowerMask;
	}
};

/**
 * For multi-textured blocks:
 * +----------------------+-------------------------+
 * | POWER_STATE (2 bits) | TEXTURE_INDEX (14 bits) |
 * +----------------------+-------------------------+
 */
class MultitexturedMetadata : public PowerableMetadata
{
public:
	constexpr static uint16_t Unlit = 0;
	constexpr static uint16_t Lit = 1;
public:
	uint16_t TextureIndex() const {
		return m_value & UnpowerMask;
	}
	void TextureIndex(uint16_t index) {
		m_value = m_value & PowerMask | index;
	}
};

/**
 * For directional blocks:
 * +----------------------+---------------------+
 * | POWER_STATE (2 bits) | DIRECTION (14 bits) |
 * +----------------------+---------------------+
 */
class DirectionalMetadata : public PowerableMetadata
{
public:
	constexpr static uint16_t XAxis = 0;
	constexpr static uint16_t YAxis = 1;
	constexpr static uint16_t ZAxis = 2;
public:
	uint16_t Direction() const {
		return m_value & UnpowerMask;
	}
	void Direction(uint16_t direction) {
		m_value = m_value & PowerMask | direction;
	}
};

/**
 * For slabs:
 * +----------------------+---------+--------------------+
 * | POWER_STATE (2 bits) | 12 bits | SLAB_TYPE (2 bits) |
 * +----------------------+---------+--------------------+
 */
class SlabMetadata : public PowerableMetadata
{
public:
	constexpr static uint16_t Double = 0;
	constexpr static uint16_t Lower = 1;
	constexpr static uint16_t Upper = 2;
public:
	uint16_t SlabType() const {
		return m_value & UnpowerMask;
	}
	void SlabType(uint16_t type) {
		m_value = m_value & PowerMask | type;
	}
};

/**
 * For connectable:
 * +--------+--------------------------------+
 * | 8 bits | CONNECTION_STATE (4 of 2 bits) |
 * +--------+--------------------------------+
 */
class ConnectableMetadata : public Metadata
{
public:
	constexpr static uint16_t Unconnected = 0;
	constexpr static uint16_t Connected = 1;
	constexpr static uint16_t ConnectedUpwards = 2;
	constexpr static uint16_t ConnectedDownwards = 3;
public:
	uint16_t FrontConnection() const {
		return m_value >> 6 & 0x3;
	}
	void FrontConnection(uint16_t connection) {
		m_value = m_value & 0xff3f | connection << 6;
	}
	uint16_t BackConnection() const {
		return m_value >> 4 & 0x3;
	}
	void BackConnection(uint16_t connection) {
		m_value = m_value & 0xffcf | connection << 4;
	}
	uint16_t LeftConnection() const {
		return m_value >> 2 & 0x3;
	}
	void LeftConnection(uint16_t connection) {
		m_value = m_value & 0xfff3 | connection << 2;
	}
	uint16_t RightConnection() const {
		return m_value & 0x3;
	}
	void RightConnection(uint16_t connection) {
		m_value = m_value & 0xfffc | connection;
	}
	void Connection(uint16_t connection) {
		m_value = connection;
	}
};

/**
 * For redstone:
 * +----------------+--------+--------------------------------+
 * | POWER (4 bits) | 4 bits | CONNECTION_STATE (4 of 2 bits) |
 * +----------------+--------+--------------------------------+
 */
class RedstoneMetadata : public ConnectableMetadata
{
public:
	uint16_t Power() const {
		return m_value >> 12;
	}
	void Power(uint16_t power) {
		m_value = m_value & 0x00ff | power << 12;
	}
	bool IsPoweringSide(uint8_t facing) const {
		// Having power && (dot shape || pointing towards)
		return m_value & 0xf000 && ((m_value & 0x00ff) == 0 || (m_value >> 2 * (5 - facing) & 0x0003) == Connected);
	}
};

struct Facing
{
	enum : uint8_t {
		Up,
		Down,
		Front,
		Back,
		Left,
		Right,
		Nowhere
	};
	GLEX_STATIC_CLASS(Facing)
};

enum class CullMode : uint8_t
{
	Null,
	UpperSlabOrUpsideDownStair,
	LowerSlabOrRightSideUpStair,
	SameBlock
};

struct Face
{
	float const* vertices;
	uint8_t const* indices;
	uint8_t numVertices, numIndices;
	bool opaque;
	CullMode cullMode;
};

struct Model
{
	Face const* faces[7];
};

struct TexturedModel
{
	Model const* model;
	uint16_t textureId[7];
};

struct BoundingBox
{
	glm::vec3 lowerLeftBack, upperRightFront;
};

constexpr glm::ivec3 blockNormals[] =
{
	glm::ivec3(0, 1, 0),
	glm::ivec3(0, -1, 0),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, 0, -1),
	glm::ivec3(-1, 0, 0),
	glm::ivec3(1, 0, 0)
};

constexpr glm::i16vec3 sblockNormals[] =
{
	glm::i16vec3(0, 1, 0),
	glm::i16vec3(0, -1, 0),
	glm::i16vec3(0, 0, 1),
	glm::i16vec3(0, 0, -1),
	glm::i16vec3(-1, 0, 0),
	glm::i16vec3(1, 0, 0)
};

constexpr glm::ivec3 blockEdges[] =
{
	glm::ivec3(-1, -1, 0),
	glm::ivec3(-1, 1, 0),
	glm::ivec3(1, -1, 0),
	glm::ivec3(1, 1, 0),
	glm::ivec3(-1, 0, -1),
	glm::ivec3(-1, 0, 1),
	glm::ivec3(1, 0, -1),
	glm::ivec3(1, 0, 1),
	glm::ivec3(0, -1, -1),
	glm::ivec3(0, -1, 1),
	glm::ivec3(0, 1, -1),
	glm::ivec3(0, 1, 1)
};

enum class BlockType : uint8_t
{
	Normal,
	Transparent,
	ThreeDirectional,
	MultiTextured,
	Slab,
	Connectable,
	Null
};

struct BlockDesctiptor
{
	BlockType type;
	uint8_t lightLevel;
	uint8_t lightOpacity;
	uint16_t inventoryMetadata;
	TexturedModel const* model;
};

struct TextureId {
	enum : uint16_t {
		Missing,
		GrassTop,
		GrassSide,
		Dirt,
		Stone,
		AcaciaPlanks,
		BirchPlanks,
		OakPlanks,
		SprucePlanks,
		IronBlock,
		GoldBlock,
		DiamondBlock,
		Cobblestone,
		StoneSlabTop,
		StoneSlabSide,
		Glass,
		AcaciaLog,
		AcaciaLogTop,
		BirchLog,
		BirchLogTop,
		OakLog,
		OakLogTop,
		SpruceLog,
		SpruceLogTop,
		BlackGlass,
		BlueGlass,
		BrownGlass,
		CyanGlass,
		GrayGlass,
		GreenGlass,
		LightBlueGlass,
		LightGrayGlass,
		LimeGlass,
		MagentaGlass,
		OrangeGlass,
		PinkGlass,
		PurpleGlass,
		RedGlass,
		WhiteGlass,
		YellowGlass,
		RedstoneBlock,
		RedstoneDot,
		RedstoneLine,
		RedstoneLamp,
		LitRedstoneLamp,
		Redstone,
		Glowstone
	};
	GLEX_STATIC_CLASS(TextureId)
};

extern glm::mat4 leftRotation, rightRotation, backRotation;
extern BlockDesctiptor blockDescriptors[];

struct Block
{
	uint16_t id;
	uint16_t metadata;
	enum Id : uint16_t {
		Air,
		Stone,
		Dirt,
		GrassBlock,
		AcaciaPlanks,
		BirchPlanks,
		OakPlanks,
		SprucePlanks,
		IronBlock,
		GoldBlock,
		DiamondBlock,
		Cobblestone,
		RedstoneBlock,
		Glass,
		BlackGlass,
		BlueGlass,
		BrownGlass,
		CyanGlass,
		GrayGlass,
		GreenGlass,
		LightBlueGlass,
		LightGrayGlass,
		LimeGlass,
		MagentaGlass,
		OrangeGlass,
		PinkGlass,
		PurpleGlass,
		RedGlass,
		WhiteGlass,
		YellowGlass,
		AcaciaLog,
		BirchLog,
		OakLog,
		SpruceLog,
		RedstoneLamp,
		CobblestoneSlab,
		SmoothstoneSlab,
		AcaciaSlab,
		BirchSlab,
		OakSlab,
		SpruceSlab,
		Redstone,
		Glowstone,
		Unknown
	};
	Metadata& Metadata() {
		return *reinterpret_cast<::Metadata*>(&metadata);
	}
	::Metadata const& Metadata() const {
		return metadata;
	}
	BlockDesctiptor const& GetDescriptor() const {
		return blockDescriptors[id];
	}
	uint8_t DefaultBlockLight() const {
		uint8_t light = GetDescriptor().lightLevel;
		if (light == 87)
			light = Metadata().As<MultitexturedMetadata>().TextureIndex() == MultitexturedMetadata::Lit ? 15 : 0;
		return light;
	}
	bool IsFull() const {
		BlockType type = GetDescriptor().type;
		return type < BlockType::Slab || type == BlockType::Slab && Metadata().As<SlabMetadata>().SlabType() == SlabMetadata::Double;
	}
	bool IsSolid() const {
		BlockType type = GetDescriptor().type;
		return type == BlockType::Normal || type == BlockType::ThreeDirectional || type == BlockType::MultiTextured ||
			type == BlockType::Slab && Metadata().As<SlabMetadata>().SlabType() == SlabMetadata::Double;
	}
	bool IsPowered() const {
		return IsSolid() && Metadata().As<PowerableMetadata>().IsPowered();
	}
	bool IsStronglyPowered() const {
		return IsSolid() && Metadata().As<PowerableMetadata>().IsStronglyPowered();
	}
	BoundingBox const& GetBoundingBox() const;
	int8_t GetFaceState(uint8_t facing) const;
};