#include <gtest/gtest.h>
#include <Yonai/World.hpp>
#include <Yonai/Components/DebugName.hpp>
#include <Yonai/Components/Transform.hpp>

TEST(ECS, EntityCreation)
{
	Yonai::World world;
	Yonai::Entity entity = world.CreateEntity();
	EXPECT_NE(entity.ID(), Yonai::InvalidEntityID);
	EXPECT_TRUE(entity.IsValid());
	EXPECT_FALSE(entity.HasComponents());
	EXPECT_EQ(entity.GetWorld(), &world);
}

TEST(ECS, EntityDestruction)
{
	Yonai::World world;
	Yonai::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());
	entity.Destroy();
	EXPECT_FALSE(entity.IsValid());
}

TEST(ECS, EntityAddComponent)
{
	Yonai::World world;
	Yonai::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	Yonai::Components::DebugName* debugNameComponent = entity.AddComponent<Yonai::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_EQ(debugNameComponent->Entity.ID(), entity.ID());
	EXPECT_TRUE(entity.HasComponent<Yonai::Components::DebugName>());
}

TEST(ECS, EntityModifyNameComponent)
{
	Yonai::World world;
	Yonai::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	Yonai::Components::DebugName* debugNameComponent = entity.AddComponent<Yonai::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_TRUE(entity.HasComponent<Yonai::Components::DebugName>());

	const std::string TestDebugName = "Test Debug";
	debugNameComponent->Name = TestDebugName;

	debugNameComponent = nullptr;
	debugNameComponent = entity.GetComponent<Yonai::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, TestDebugName);
}

TEST(ECS, EntityRemoveComponent)
{
	// Create entity
	Yonai::World world;
	Yonai::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	// Add debug name component
	Yonai::Components::DebugName* debugNameComponent = entity.AddComponent<Yonai::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_TRUE(entity.HasComponent<Yonai::Components::DebugName>());

	// Remove debug name component
	entity.RemoveComponent<Yonai::Components::DebugName>();

	EXPECT_FALSE(entity.HasComponent<Yonai::Components::DebugName>());
}

TEST(ECS, EntityAddComponentTwice)
{
	// Create entity
	Yonai::World world;
	Yonai::Entity entity = world.CreateEntity();
	EXPECT_TRUE(entity.IsValid());

	// Add debug name component
	Yonai::Components::DebugName* debugNameComponent = entity.AddComponent<Yonai::Components::DebugName>();
	EXPECT_NE(debugNameComponent, nullptr);
	EXPECT_EQ(debugNameComponent->Name, "");
	EXPECT_TRUE(entity.HasComponent<Yonai::Components::DebugName>());

	Yonai::Components::DebugName* debugNameComponent2 = entity.AddComponent<Yonai::Components::DebugName>();
	EXPECT_EQ(debugNameComponent, debugNameComponent2);

	debugNameComponent2 = entity.GetComponent<Yonai::Components::DebugName>();
	EXPECT_EQ(debugNameComponent, debugNameComponent2);
}

TEST(ECS, TransformComponentChild)
{
	Yonai::World world;

	// Create entity1
	auto entity1 = world.CreateEntity();
	EXPECT_TRUE(entity1.IsValid());

	// Create entity2
	auto entity2 = world.CreateEntity();
	EXPECT_TRUE(entity2.IsValid());

	EXPECT_NE(entity1.ID(), entity2.ID());

	// Create transforms
	auto transform1 = entity1.AddComponent<Yonai::Components::Transform>();
	auto transform2 = entity2.AddComponent<Yonai::Components::Transform>();

	EXPECT_NE(transform1, nullptr);
	EXPECT_NE(transform2, nullptr);
	EXPECT_NE(transform1, transform2);
	EXPECT_EQ(transform1->Parent, nullptr);
	EXPECT_EQ(transform2->Parent, nullptr);

	// Set transform2 as child of transform1
	// TODO: Hierarchy functions
}

TEST(ECS, WorldHasEntity)
{
	Yonai::World world;

	EXPECT_FALSE(world.HasEntity(123));

	Yonai::Entity entity = world.CreateEntity();
	Yonai::EntityID entityID = entity.ID();
	EXPECT_TRUE(entity.IsValid());
	EXPECT_TRUE(world.HasEntity(entityID));

	world.DestroyEntity(entityID);
	EXPECT_FALSE(world.HasEntity(entityID));

	entity = world.CreateEntity();
	entityID = entity.ID();
	EXPECT_TRUE(world.HasEntity(entityID));
	
	entity.Destroy();
	EXPECT_FALSE(world.HasEntity(entityID));
}

TEST(ECS, WorldGetEntity)
{
	const unsigned int entityID = 123;
	Yonai::World world;

	EXPECT_FALSE(world.HasEntity(entityID));

	Yonai::Entity e = world.GetEntity(entityID);
	EXPECT_FALSE(e.IsValid());
	EXPECT_FALSE(world.HasEntity(entityID));
}