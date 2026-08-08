#include "zed_workspace_model.h"

#include <algorithm>
#include <stdexcept>

namespace zed::qtcreator {
std::vector<PackageNode> WorkspaceModel::project(const std::vector<Report>& reports) {
  std::vector<PackageNode> nodes;
  nodes.reserve(reports.size());
  for (const auto& report : reports) {
    PackageNode node;
    node.root = std::filesystem::absolute(report.workspace_root).lexically_normal();
    node.issues = report.issues;
    node.errors = std::count_if(node.issues.begin(), node.issues.end(), [](const Issue& issue) { return issue.severity == "error"; });
    node.warnings = std::count_if(node.issues.begin(), node.issues.end(), [](const Issue& issue) { return issue.severity == "warning"; });
    nodes.push_back(std::move(node));
  }
  std::sort(nodes.begin(), nodes.end(), [](const PackageNode& lhs, const PackageNode& rhs) { return lhs.root.string() < rhs.root.string(); });
  return nodes;
}

ActionPreview WorkspaceModel::preview(const Action& action, const std::filesystem::path& root) {
  if (action.kind != "command") throw std::invalid_argument("only command actions have an execution preview");
  if (!action.requires_confirmation) throw std::invalid_argument("command actions must require explicit confirmation");
  if (action.command.empty()) throw std::invalid_argument("command executable must not be empty");
  return ActionPreview{action.command, action.arguments, std::filesystem::absolute(root).lexically_normal(), true};
}
}  // namespace zed::qtcreator
