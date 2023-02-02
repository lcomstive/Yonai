# Compiling the Library {#Compiling}

## Mac OS
### Universal
```bash
# Clone repository
git clone https://github.com/lcomstive/AquaEngine.git
cd AquaEngine

# Install native dependencies
vcpkg install 

# Optional. When building on an Arm Mac for Intel Macs/universal binary
vcpkg install --triplet x64-osx

cmake -DCMAKE_OSX_ARCHITECTURES=arm64;x86_64
```