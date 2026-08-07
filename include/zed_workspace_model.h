#pragma once

#include "zed_inspector.h"

#include <filesystem>
#include <string>
#include <vector>

namespace zed::qtcreator {
struct PackageNode {
  std::filesystem::path root;
  std::size_t errors{};
  std::size_t warnings{};
  std::vector<Issue> issues;
};

struct ActionPreview {
  std::string executable;
  std::vector<std::string> arguments;
  std::filesystem::path working_directory;
  bool requires_confirmation{};
};

class WorkspaceModel {
 public:
  [[nodiscard]] static std::vector<PackageNode> project(const std::vector<Report>& reports);
  [[nodiscard]] static ActionPreview preview(const Action& action, const std::filesystem::path& root);
};
}  // namespace zed::qtcreator
