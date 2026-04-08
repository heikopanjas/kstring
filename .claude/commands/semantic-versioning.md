
Read this skill when you need to bump the project version. It explains when to
increment PATCH, MINOR, or MAJOR, and how to include the bump in a commit.

---

## Semantic Versioning Protocol

**AUTOMATICALLY track version changes using semantic versioning (SemVer) in CMakeLists.txt.**

The current version is defined in `CMakeLists.txt` under the `project()` command as `project(KString VERSION X.Y.Z LANGUAGES C)`.

### Version Format: MAJOR.MINOR.PATCH

**When to increment:**

1. **PATCH version** (X.Y.Z → X.Y.Z+1)
   - Bug fixes and minor corrections
   - Performance improvements without API changes
   - Documentation updates
   - Internal refactoring that doesn't affect public API
   - Example: `1.0.0` → `1.0.1`

2. **MINOR version** (X.Y.Z → X.Y+1.0)
   - New features added
   - New API functions or parameters
   - New functionality that maintains backward compatibility
   - Example: `1.0.1` → `1.1.0`

3. **MAJOR version** (X.Y.Z → X+1.0.0)
   - Breaking changes to public API
   - Removal of functions or parameters
   - Changes that require user action or code updates
   - Incompatible API changes
   - Example: `1.1.0` → `2.0.0`

### Process

After making ANY code changes:

1. Determine the type of change (fix, feature, or breaking change)
2. Update the version in `CMakeLists.txt` accordingly
3. Include the version change in the same commit as the code change
4. Mention version bump in commit message footer if significant

**Note:** Version changes should be included in the commit with the actual code changes, not as a separate commit.
