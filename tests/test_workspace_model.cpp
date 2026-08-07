#include "zed_workspace_model.h"

#include <cassert>
#include <filesystem>
#include <stdexcept>
#include <vector>

using namespace zed::qtcreator;

int main() {
  Report second{1, "zeta", {Issue{"lock.stale", "warning", "Stale", "", {Action{"install", "Install", "command", "zed", {"install"}, true}}}}};
  Report first{1, "alpha", {Issue{"manifest.invalid", "error", "Invalid", "", {}}}};
  auto nodes = WorkspaceModel::project({second, first});
  assert(nodes.size() == 2);
  assert(nodes[0].root.filename() == "alpha");
  assert(nodes[0].errors == 1);
  assert(nodes[1].warnings == 1);

  auto preview = WorkspaceModel::preview(second.issues[0].actions[0], second.workspace_root);
  assert(preview.executable == "zed");
  assert(preview.arguments == std::vector<std::string>{"install"});
  assert(preview.requires_confirmation);

  bool rejected = false;
  try {
    static_cast<void>(WorkspaceModel::preview(Action{"bad", "Bad", "command", "zed", {"install"}, false}, "."));
  } catch (const std::invalid_argument&) {
    rejected = true;
  }
  assert(rejected);
}
