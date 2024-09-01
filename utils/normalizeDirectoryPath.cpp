#include <string>
#include <vector>

std::string normalizeDirectoryPath(const std::string &path) {
  std::vector<std::string> parts;
  std::string segment;
  std::string::size_type prev = 0, pos = 0;
  bool isRelative = false;

  // Check if the path is relative
  if (!path.empty() && path[0] == '.') {
    isRelative = true;
  }

  while ((pos = path.find('/', prev)) != std::string::npos) {
    segment = path.substr(prev, pos - prev);
    if (segment == "." || segment.empty()) {
      // Skip redundant slashes or ./ segments
      prev = pos + 1;
      continue;
    } else if (segment == "..") {
      if (!parts.empty()) {
        parts.pop_back(); // Go up one directory
      }
    } else {
      parts.push_back(segment);
    }
    prev = pos + 1;
  }

  segment = path.substr(prev); // Handle the last segment
  if (!segment.empty() && segment != ".") {
    if (segment == "..") {
      if (!parts.empty()) {
        parts.pop_back();
      }
    } else {
      parts.push_back(segment);
    }
  }

  std::string normalizedPath;
  if (isRelative) {
    normalizedPath = "."; // Start with the relative dot
  }

  for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end();
       ++it) {
    normalizedPath += "/" + *it;
  }

  if (normalizedPath.empty()) {
    return "/";
  }

  return normalizedPath;
}