#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

enum class SortKey : uint32_t {
    Shader = static_cast<uint32_t>(1) << 31,  // Most Significant bit (for shader comparison)
    Depth = static_cast<uint32_t>(1) << 16,   // Middle bit (for depth comparison)
    Material = static_cast<uint32_t>(1) << 0, // Least significant bit (for material comparison)
    NULL_SORT = 0
};

// A sample Shader structure with an ID and other data
struct Shader {
    uint32_t id;
    std::string name;

    bool operator<(const Shader& other) const {
        return id < other.id;
    }

    bool operator>(const Shader& other) const {
        return id > other.id;
    }
};

// A Command structure which uses the SortKey to guide comparison
struct Command {
    SortKey sortKey;
    Shader* shader;
    float depth;
    std::string material;

    // Add more fields if needed, such as textures, lighting, etc.
};

// A comparator for multi-priority sorting (grouped by SortKey)
bool compareCommands(const Command& a, const Command& b) {
    // Compare based on the SortKey flag: highest bit first
    if (a.sortKey == SortKey::Shader && b.sortKey == SortKey::Shader) {
        return *a.shader > *b.shader;  // Compare by shader ID (or other shader properties)
    }
    if (a.sortKey == SortKey::Depth && b.sortKey == SortKey::Depth) {
        return a.depth > b.depth;  // Compare by depth value
    }
    if (a.sortKey == SortKey::Material && b.sortKey == SortKey::Material) {
        return a.material > b.material;  // Compare by material name
    }

    // Fallback: return false to preserve original order if no comparison is needed
    return false;
}

// A function to sort and group commands
void renderPass(std::vector<Command>& commands) {
    // Sort the commands with a stable sort to maintain relative order within the same SortKey group
    std::stable_sort(commands.begin(), commands.end(), compareCommands);

    // Output sorted results to check the order
    for (const auto& cmd : commands) {
        std::cout << "SortKey: " << static_cast<uint32_t>(cmd.sortKey)
                  << ", Shader ID: " << cmd.shader->id
                  << ", Depth: " << cmd.depth
                  << ", Material: " << cmd.material << std::endl;
    }
}

int main() {
    // Create some shaders
    Shader shader1 { 101, "ShaderA" };
    Shader shader2 { 102, "ShaderB" };
    Shader shader3 { 103, "ShaderC" };

    // Create commands with SortKey priorities and data (depth, material)
    std::vector<Command> commands = {
        { SortKey::Shader, &shader1, 10.5f, "Material1" },
        { SortKey::Depth, &shader2, 5.5f, "Material2" },
        { SortKey::Material, &shader3, 2.5f, "Material3" },
        { SortKey::Shader, &shader2, 7.5f, "Material4" }
    };

    // Render pass that sorts the commands
    renderPass(commands);

    return 0;
}