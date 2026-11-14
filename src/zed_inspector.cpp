#include "zed_inspector.h"

#include <regex>
#include <stdexcept>
#include <utility>

namespace zed::qtcreator {
namespace {
const std::regex kAssignment(R"((authorization|token|password|secret|api[_-]?key)\s*[:=]\s*([^\s,;]+))", std::regex::icase);
const std::regex kBearer(R"(bearer\s+[A-Za-z0-9._~+/\-=]+)", std::regex::icase);
const std::regex kGithubToken(R"(gh[pousr]_[A-Za-z0-9_]{20,})");
}
Inspector::Inspector(std::string executable) : executable_(std::move(executable)) { if (executable_.empty()) throw std::invalid_argument("zed executable must not be empty"); }
Command Inspector::command_for(const std::filesystem::path& workspace_root) const {
  const auto root = std::filesystem::absolute(workspace_root).lexically_normal();
  return Command{executable_, {"inspect", "--workspace", root.string(), "--json"}, root, std::chrono::seconds{30}};
}
Report Inspector::validate_report(Report report, const std::filesystem::path& requested_root) const {
  const auto expected_root = std::filesystem::absolute(requested_root).lexically_normal();
  if (!report.workspace_root.is_absolute() || report.workspace_root.lexically_normal() != expected_root) {
    return Report{1, expected_root, {Issue{"inspect.root.mismatch", "error", "Inspection workspace mismatch", "The CLI reported a different workspace root.", {}}}};
  }
  if (report.schema_version != 1) return Report{1, report.workspace_root, {Issue{"inspect.schema.unsupported", "error", "Unsupported Zed inspection schema", "The integration only accepts schemaVersion 1.", {}}}};
  for (auto& issue : report.issues) {
    issue.detail = redact(issue.detail);
    for (const auto& action : issue.actions) if (action.kind == "command" && !action.requires_confirmation) return Report{1, report.workspace_root, {Issue{"inspect.action.unsafe", "error", "Unsafe Zed action rejected", "Command actions must require explicit confirmation.", {}}}};
  }
  return report;
}
std::string Inspector::redact(std::string_view text) {
  std::string output{text};
  output = std::regex_replace(output, kAssignment, "$1=[REDACTED]");
  output = std::regex_replace(output, kBearer, "Bearer [REDACTED]");
  return std::regex_replace(output, kGithubToken, "[REDACTED]");
}
}  // namespace zed::qtcreator
