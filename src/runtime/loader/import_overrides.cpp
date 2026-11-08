// Import overrides - see import_overrides.hpp. Part of openjoan_game (the modules that register overrides are
// there); the loader (joan_re.cpp) consults the table while it resolves joan.exe's imports.
#include "import_overrides.hpp"

namespace joan::loader {

namespace {

// Function-local: registrations run during static initialisation in any order.
std::vector<ImportOverride>& table() {
    static std::vector<ImportOverride> t;
    return t;
}

char lower(char c) { return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c; }

bool active(const ImportOverride& o) { return o.enabled == nullptr || o.enabled(); }

}  // namespace

void register_import_override(const ImportOverride& o) { table().push_back(o); }

const std::vector<ImportOverride>& import_overrides() { return table(); }

bool same_dll(const char* a, const char* b) {
    if (a == nullptr || b == nullptr) return false;
    for (; *a != '\0' && *b != '\0'; ++a, ++b)
        if (lower(*a) != lower(*b)) return false;
    return *a == *b;
}

bool matches(const ImportOverride& o, const char* dll, const char* name, unsigned ordinal) {
    if (!same_dll(o.dll, dll)) return false;
    if (o.name == nullptr) return name == nullptr && ordinal != 0 && o.ordinal == ordinal;
    return name != nullptr && std::strcmp(o.name, name) == 0;
}

int find_import_override(const char* dll, const char* name, unsigned ordinal, int* duplicate) {
    const std::vector<ImportOverride>& t = table();
    int found = -1;
    for (std::size_t i = 0; i < t.size(); ++i) {
        if (!matches(t[i], dll, name, ordinal) || !active(t[i])) continue;
        if (found < 0) {
            found = static_cast<int>(i);
        } else {
            if (duplicate) *duplicate = static_cast<int>(i);
            break;
        }
    }
    return found;
}

}  // namespace joan::loader
