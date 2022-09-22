#include <gtest/gtest.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Components/DebugName.hpp>
#include <AquaEngine/Components/Transform.hpp>

TEST(ECS, EntityCreation)
{
	AquaEngine::World world;
	AquaEngine::Entity entity = world.CreateEntity();
	EXPECT_NE(entity.ID(), AquaEngine::InvalidEntityID);
	EXPECT_TRUE(entity.IsValid());
	EXPECT_FALSE(entity.HasComponents());
	EXPECT_EQ(entity.GetWorld(), &world);
}

TEST(ECS, EntityDestruction)
{
	AquaEngine::World world;
	AquaEngine::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());
	entity.Destroy();
	EXPECT_FALSE(entity.IsValid());
}

TEST(ECS, EntityAddComponent)
{
	AquaEngine::World world;
	AquaEngine::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	AquaEngine::Components::DebugName* debugNameComponent = entity.AddComponent<AquaEngine::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_EQ(debugNameComponent->Entity.ID(), entity.ID());
	EXPECT_TRUE(entity.HasComponent<AquaEngine::Components::DebugName>());
}

TEST(ECS, EntityModifyNameComponent)
{
	AquaEngine::World world;
	AquaEngine::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	AquaEngine::Components::DebugName* debugNameComponent = entity.AddComponent<AquaEngine::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_TRUE(entity.HasComponent<AquaEngine::Components::DebugName>());

	const std::string TestDebugName = "Test Debug";
	debugNameComponent->Name = TestDebugName;

	debugNameComponent = nullptr;
	debugNameComponent = entity.GetComponent<AquaEngine::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, TestDebugName);
}

TEST(ECS, EntityRemoveComponent)
{
	// Create entity
	AquaEngine::World world;
	AquaEngine::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	// Add debug name component
	AquaEngine::Components::DebugName* debugNameComponent = entity.AddComponent<AquaEngine::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_TRUE(entity.HasComponent<AquaEngine::Components::DebugName>());

	// Remove debug name component
	entity.RemoveComponent<AquaEngine::Components::DebugName>();

	EXPECT_FALSE(entity.HasComponent<AquaEngine::Components::DebugName>());
}

TEST(ECS, EntityAddComponentTwice)
{
	// Create entity
	AquaEngine::World world;
	AquaEngine::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	// Add debug name component
	AquaEngine::Components::DebugName* debugNameComponent = entity.AddComponent<AquaEngine::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_TRUE(entity.HasComponent<AquaEngine::Components::DebugName>());

	AquaEngine::Components::DebugName* debugNameComponent2 = entity.AddComponent<AquaEngine::Components::DebugName>();
	EXPECT_EQ(debugNameComponent, debugNameComponent2);

	debugNameComponent2 = entity.GetComponent<AquaEngine::Components::DebugName>();
	EXPECT_EQ(debugNameComponent, debugNameComponent2);
}

TEST(ECS, TransformComponentChild)
{
	AquaEngine::World world;

	// Create entity1
	auto entity1 = world.CreateEntity();
	EXPECT_TRUE(entity1.IsValid());

	// Create entity2
	auto entity2 = world.CreateEntity();
	EXPECT_TRUE(entity2.IsValid());

	EXPECT_NE(entity1.ID(), entity2.ID());

	// Create transforms
	auto transform1 = entity1.AddComponent<AquaEngine::Components::Transform>();
	auto transform2 = entity2.AddComponent<AquaEngine::Components::Transform>();

	EXPECT_NE(transform1, nullptr);
	EXPECT_NE(transform2, nullptr);
	EXPECT_NE(transform1, transform2);
	EXPECT_EQ(transform1->Parent, nullptr);
	EXPECT_EQ(transform2->Parent, nullptr);

	// Set transform2 as child of transform1
	// TODO: Hierarchy functions
}