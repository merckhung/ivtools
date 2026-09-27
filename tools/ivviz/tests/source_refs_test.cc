// Every source file a tour cites exists under ivtools/src, and each call
// trace line names at least one method or function found in its file (or,
// without a call, appears there verbatim).
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "src/scene/design_script.h"

namespace fs = std::filesystem;

namespace {

int failures = 0;

void Fail(const std::string& what) {
  std::fprintf(stderr, "FAIL: %s\n", what.c_str());
  ++failures;
}

std::string Slurp(const fs::path& p) {
  std::ifstream in(p);
  std::stringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

bool IsIdent(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }

// Identifiers that are called or qualified in `text`: "Viewer::UseTool(" gives
// "UseTool", "glyph_->request(" gives "request".
std::vector<std::string> Callees(const std::string& text) {
  std::vector<std::string> out;
  for (size_t i = 0; i < text.size(); ++i) {
    if (!IsIdent(text[i]) || (i > 0 && IsIdent(text[i - 1]))) continue;
    size_t j = i;
    while (j < text.size() && IsIdent(text[j])) ++j;
    const std::string word = text.substr(i, j - i);
    const bool qualified = i >= 2 && text.compare(i - 2, 2, "::") == 0;
    const bool called = j < text.size() && text[j] == '(';
    if ((qualified || called) && !std::isdigit(static_cast<unsigned char>(word[0]))) {
      out.push_back(word);
    }
    i = j;
  }
  return out;
}

bool Contains(const fs::path& p, const std::string& word) {
  if (fs::is_directory(p)) {
    for (const auto& e : fs::recursive_directory_iterator(p)) {
      if (e.is_regular_file() && Contains(e.path(), word)) return true;
    }
    return false;
  }
  const std::string body = Slurp(p);
  for (size_t at = body.find(word); at != std::string::npos; at = body.find(word, at + 1)) {
    const bool left = at == 0 || !IsIdent(body[at - 1]);
    const bool right = at + word.size() >= body.size() || !IsIdent(body[at + word.size()]);
    if (left && right) return true;
  }
  return false;
}

}  // namespace

int main() {
  using namespace ivviz::scene;
  const char* srcdir = std::getenv("TEST_SRCDIR");
  const fs::path runfile =
      fs::path(srcdir ? srcdir : ".") / "_main" / "src" / "scene" / "design_script.cc";
  std::error_code ec;
  const fs::path real = fs::canonical(runfile, ec);
  // real = <repo>/tools/ivviz/src/scene/design_script.cc
  const fs::path src = real.parent_path().parent_path().parent_path().parent_path()
                           .parent_path() / "ivtools" / "src";
  if (ec || !fs::is_directory(src / "Unidraw")) {
    std::printf("source_refs_test: skipped (ivtools/src not found from %s)\n",
                runfile.string().c_str());
    return 0;
  }
  const fs::path include = src / "include";
  int checked = 0;
  for (const Tour& t : DesignTours()) {
    // "Unidraw/viewer.c: Viewer::UseTool" or "include/InterViews/glyph.h: Glyph"
    const size_t colon = t.where.find(": ");
    const fs::path where = src / t.where.substr(0, colon);
    if (!fs::exists(where)) Fail(t.name + ": missing " + where.string());
    else {
      std::string names = t.where.substr(colon + 2);
      for (size_t k = 0; k < names.size(); ++k) if (names[k] == ',') names[k] = ' ';
      std::stringstream ss(names);
      for (std::string sym; ss >> sym;) {
        const std::string last = sym.substr(sym.rfind(':') == std::string::npos ? 0
                                                                                : sym.rfind(':') + 1);
        if (!Contains(where, last)) Fail(t.name + ": " + last + " not in " + where.string());
      }
    }
    for (const TraceLine& l : t.trace) {
      fs::path file = src / l.file;
      if (!fs::exists(file)) file = include / l.file;
      if (!fs::exists(file)) {
        Fail(t.name + ": missing " + l.file);
        continue;
      }
      // Lines without a call (declarations, returns) must appear verbatim.
      const std::vector<std::string> callees = Callees(l.text);
      bool any = callees.empty() && Slurp(file).find(l.text) != std::string::npos;
      for (const std::string& callee : callees) any = any || Contains(file, callee);
      if (!any) Fail(t.name + ": nothing from \"" + l.text + "\" found in " + l.file);
      ++checked;
    }
  }
  if (failures) return 1;
  std::printf("source_refs_test: %d trace lines match %s\n", checked, src.string().c_str());
  return 0;
}
