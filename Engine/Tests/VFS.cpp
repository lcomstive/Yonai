#include <gtest/gtest.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/IO/Files.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;

namespace fs = std::filesystem;

string TestMountPath = "./test";
string TestFileContents = "This is a VFS test!";

TEST(VFS, CreateMount)
{
	// Ensure mount doesn't exist prior to test
	EXPECT_FALSE(VFS::HasMount("/TestMount"));

	// Create mount point
	VFS::Mount("/TestMount", TestMountPath);

	// Check mount exists
	EXPECT_TRUE(VFS::HasMount("/TestMount"));
}

TEST(VFS, CreateDirectory)
{
	EXPECT_FALSE(VFS::Exists("/TestMount/TestDir"));

	VFSMapping* mapping = VFS::GetMapping("/TestMount");
	EXPECT_NE(mapping, nullptr);

	EXPECT_TRUE(mapping->CreateDirectory(mapping->GetMountedPath("/TestMount/TestDir")));

	EXPECT_TRUE(VFS::Exists("/TestMount/TestDir"));
}

TEST(VFS, MountAccess)
{
	// Check mount exists
	EXPECT_TRUE(VFS::HasMount("/TestMount"));
	// Make sure test file does not exist
	EXPECT_FALSE(VFS::Exists("/TestMount/test.txt"));

	// Create test text file
	IO::Write(TestMountPath + "/test.txt", TestFileContents);

	// Ensure test text file can be found
	EXPECT_TRUE(VFS::Exists("/TestMount/test.txt"));

	// Check if test file contents match what was written
	EXPECT_EQ(VFS::ReadText("/TestMount/test.txt"), TestFileContents);
}

TEST(VFS, Copy)
{
	EXPECT_TRUE(VFS::Exists("/TestMount/test.txt"));
	VFS::Copy("/TestMount/test.txt", "/TestMount/TestDir/test.txt");

	EXPECT_TRUE(VFS::Exists("/TestMount/TestDir/test.txt"));
	EXPECT_EQ(VFS::ReadText("/TestMount/TestDir/test.txt"), TestFileContents);
}

TEST(VFS, Move)
{
	EXPECT_TRUE(VFS::Exists("/TestMount/test.txt"));

	VFS::Move("/TestMount/test.txt", "/TestMount/move_test.txt");

	EXPECT_FALSE(VFS::Exists("/TestMount/test.txt"));
	EXPECT_TRUE(VFS::Exists("/TestMount/move_test.txt"));
}

TEST(VFS, RemoveFile)
{
	EXPECT_TRUE(VFS::Exists("/TestMount/move_test.txt"));

	VFS::Remove("/TestMount/move_test.txt");

	EXPECT_FALSE(VFS::Exists("/TestMount/move_test.txt"));
}

TEST(VFS, RemoveDirectory)
{
	EXPECT_TRUE(VFS::Exists("/TestMount/"));

	VFS::Remove("/TestMount/");

	EXPECT_FALSE(VFS::Exists("/TestMount/"));
}