#include "pages.hpp"

void applyStyles(HtmlMarkup &directoryPage) {
  // @defgroup elements styles
  const std::string sectionPStyles =
      "section > div { cursor: default; font-size: 1.5rem;"
      "min-height: 25px; padding: 0; margin-bottom: 30px;"
      "display: flex; justify-content: space-between; }";
  const std::string ulStyles =
      "ul { list-style: none;"
      "display: flex; flex-direction: column; row-gap: 10px;"
      "padding: 0; margin: 0; }";
  const std::string liStyles =
      "li { min-height: 25px;"
      "display: flex; justify-content: space-between; column-gap: 20px; }"
      "li > div { display: flex; justify-content: space-between; flex-grow: 1;"
      "column-gap: 20px; border-bottom: 1px solid #ddd;"
      "transition: all 0.2s ease-in-out; }"
      "li > div:hover { background-color: #dddede;"
      "transition: all 0.2s ease-in-out; }";
  const std::string aStyles =
      "li a { display: flex; justify-content: space-between; column-gap: 20px;"
      "width: 100%; min-height: 25px; color: #111; text-align: left; }"
      "li a:visited { color: #111; }";
  const std::string aDivStyles =
      "li a div { display: flex; column-gap: 10px; color: #aaa; }";
  const std::string divSpanStyles = "div span { font-size: 0.8rem; }";
  const std::string buttonStyles =
      "button { cursor: pointer; color: #eee; background-color: #111;"
      "border: none; padding: 0 20px; transition: opacity 0.2s ease-in-out; }"
      "button:hover { opacity: 0.8; transition: opacity 0.2s ease-in-out; }";

  // Add the styles to the page
  directoryPage.addStyle(sectionPStyles);
  directoryPage.addStyle(ulStyles);
  directoryPage.addStyle(liStyles);
  directoryPage.addStyle(aStyles);
  directoryPage.addStyle(aDivStyles);
  directoryPage.addStyle(divSpanStyles);
  directoryPage.addStyle(buttonStyles);
}

std::string formatSize(off_t size) {
  std::ostringstream oss;
  if (size < 1024) {
    oss << size << " B";
  } else if (size < 1024 * 1024) {
    oss << std::fixed << std::setprecision(2) << (size / 1024.0) << " KB";
  } else if (size < 1024 * 1024 * 1024) {
    oss << std::fixed << std::setprecision(2) << (size / (1024.0 * 1024.0))
        << " MB";
  } else {
    oss << std::fixed << std::setprecision(2)
        << (size / (1024.0 * 1024.0 * 1024.0)) << " GB";
  }
  return oss.str();
}

std::string formatTime(time_t rawTime) {
  struct tm *timeInfo;
  char buffer[80];
  timeInfo = localtime(&rawTime);
  strftime(buffer, 80, "%Y-%m-%d %H:%M", timeInfo);
  return std::string(buffer);
}

std::string getAddButtonMarkup(HtmlMarkup &directoryPage) {
  const std::string addIcon = "&#8853;";
  std::string button;

  // HTML for the button and hidden file input
  button += "<input type=\"file\" id=\"fileInput\" style=\"display:none;\" />";
  button += "<button id=\"uploadButton\">";
  button += addIcon + " add file";
  button += "</button>";

  // JavaScript to handle the file upload
  std::ostringstream script;
  script
      << "document.getElementById('uploadButton').addEventListener('click', "
         "function() {\n"
      << "    var fileInput = document.getElementById('fileInput');\n"
      << "    fileInput.click();\n"
      << "    fileInput.onchange = function() {\n"
      << "        var file = fileInput.files[0];\n"
      << "        if (file) {\n"
      << "            var formData = new FormData();\n"
      << "            formData.append('file', file);\n"
      << "            var uploadPath = window.location.pathname;\n"
      << "            var fileName = encodeURIComponent(file.name);\n"
      << "            var fullPath = `${uploadPath}/${fileName}`;\n"
      << "            fetch(fullPath, {\n"
      << "                method: 'POST',\n"
      << "                body: formData\n"
      << "            })\n"
      << "            .then(function(response) {\n"
      << "                if (!response.ok) {\n"
      << "                    throw new Error('Server error, status code: ' + "
         "response.status);\n"
      << "                }\n"
      << "                return response.text();\n"
      << "            })\n"
      << "            .then(function(data) {\n"
      << "                alert('File uploaded successfully');\n"
      << "                location.reload();\n"
      << "            })\n"
      << "            .catch(function(error) {\n"
      << "                console.error('Error:', error);\n"
      << "                alert('Failed to upload file: ' + error.message);\n"
      << "            });\n"
      << "        }\n"
      << "    };\n"
      << "});\n";

  directoryPage.addScript(script.str());

  return button;
}

std::string getDeleteButtonMarkup(HtmlMarkup &directoryPage,
                                  std::string &filePath) {
  const std::string deleteIcon = "&#8854;";
  std::string button;

  // Use a unique ID based on the file path
  std::string uniqueButtonId = "deleteButton_" + filePath;
  std::replace(uniqueButtonId.begin(), uniqueButtonId.end(), '/',
               '_'); // Replace '/' with '_'

  button += "<button id=\"" + uniqueButtonId + "\">";
  button += deleteIcon;
  button += "</button>";

  std::ostringstream script;
  script << "document.getElementById('" << uniqueButtonId
         << "').addEventListener('click', "
            "function() {\n"
         << "    console.log('Deleting file: " << filePath << "');\n"
         << "    if (confirm('Are you sure you want to delete this file?')) {\n"
         << "        var filePath = '" << filePath << "';\n"
         << "        fetch(filePath, {\n"
         << "            method: 'DELETE'\n"
         << "        })\n"
         << "        .then(function(response) {\n"
         << "            if (!response.ok) {\n"
         << "                throw new Error('Server error, status code: ' + "
            "response.status);\n"
         << "            }\n"
         << "            alert('File deleted successfully');\n"
         << "            location.reload();\n"
         << "        })\n"
         << "        .catch(function(error) {\n"
         << "            console.error('Error:', error);\n"
         << "            alert('Failed to delete file: ' + error.message);\n"
         << "        });\n"
         << "    }\n"
         << "});\n";

  directoryPage.addScript(script.str());

  return button;
}

std::string getFolderItemMarkup(const std::string &fullPath,
                                struct stat path_stat,
                                const std::string &name) {
  const std::string folderIcon = "&#8627;";
  std::string item;

  item += "<li>";
  item += "<div>";
  item += "<a href=\"" + fullPath + "/\">";
  item += folderIcon + " " + name + "/";
  item += "<div>";
  item += "<span>" + formatTime(path_stat.st_mtime) + "</span>";
  item += "</div>";
  item += "</a>";
  item += "</div>";
  item += "</li>";
  return item;
}

std::string getFileItemMarkup(HtmlMarkup &directoryPage, std::string &fullPath,
                              struct stat path_stat, const std::string &name) {
  // @defgroup icons
  const std::string fileIcon = "&#10141;";
  const std::string notAvailableFileIcon = "&#10509";
  std::string item;

  if (fullPath.substr(0, 1) == ".")
    fullPath = fullPath.substr(1);

  item += "<li>";
  item += "<div>";
  item += "<a href=\"" + fullPath + "\">" + fileIcon + " " + name;
  item += "<div>";
  item += "<span>" + formatSize(path_stat.st_size) + "</span>";
  item += "<p>|</p>";
  item += "<span>" + formatTime(path_stat.st_mtime) + "</span>";
  item += "</div>";
  item += "</a>";
  item += "</div>";
  item += getDeleteButtonMarkup(directoryPage, fullPath);
  item += "</li>";
  return item;
}

std::string getParentPath(const std::string &path) {
  // Ensure we are not dealing with the root path "./"
  if (path == "./" || path == "/") {
    return path;
  }

  std::string parentPath = path;

  // Remove the trailing slash if it's there and it's not the root
  if (*parentPath.rbegin() == '/' && parentPath.length() > 1) {
    parentPath.resize(parentPath.size() - 1);
  }

  // Find the last '/' in the path
  size_t pos = parentPath.find_last_of('/');

  // Remove everything after the last '/' to go up one directory
  if (pos != std::string::npos) {
    parentPath = parentPath.substr(0, pos);
  }

  // Remove leading "./" if present
  if (parentPath.find("./") == 0) {
    parentPath = parentPath.substr(1);
  }

  // Ensure that if the path is empty after this, we return "."
  return parentPath.empty() ? "." : parentPath;
}

std::string getDirectoryPage(State &state) {
  HtmlMarkup directoryPage;
  std::ostringstream mainContent;
  std::vector<std::string> directories;
  std::vector<std::string> files;
  std::string backIcon = "&#8229;";

  mainContent << "<section>";

  // Add title line
  mainContent << "<div>";
  mainContent << "<p>" << normalizeDirectoryPath(state.file_path) << "/</p>";
  mainContent << getAddButtonMarkup(directoryPage);
  mainContent << "</div>";

  mainContent << "<ul>";

  if (state.file_path != "/") {
    std::string parentPath =
        normalizeDirectoryPath(getParentPath(state.file_path));
    mainContent << "<li><div><a href=\"" << parentPath << "\">";
    mainContent << backIcon << "</a></div></li>";
  }

  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(state.file_path.c_str())) != NULL) {
    while ((entry = readdir(dir)) != NULL) {
      std::string name = entry->d_name;

      // Skip the current and parent directory entries
      if (name == "." || name == "..")
        continue;

      std::string fullPath =
          normalizeDirectoryPath(state.file_path) + "/" + name;
      struct stat path_stat;
      stat(fullPath.c_str(), &path_stat);

      std::string item;
      if (S_ISDIR(path_stat.st_mode)) {
        directories.push_back(getFolderItemMarkup(fullPath, path_stat, name));
      } else {
        files.push_back(
            getFileItemMarkup(directoryPage, fullPath, path_stat, name));
      }
    }
    closedir(dir);
  } else {
    mainContent << "<li>Error: Could not open directory</li>";
  }

  // Sort directories and files alphabetically
  std::sort(directories.begin(), directories.end());
  std::sort(files.begin(), files.end());

  // Add sorted directories to the list
  for (std::vector<std::string>::iterator it = directories.begin();
       it != directories.end(); ++it) {
    mainContent << *it;
  }

  // Add sorted files to the list
  for (std::vector<std::string>::iterator it = files.begin(); it != files.end();
       ++it) {
    mainContent << *it;
  }

  mainContent << "</ul></section>";
  directoryPage.addHeaderContent(getHeaderContent(directoryPage, state));
  directoryPage.addMainContent(mainContent.str());
  applyStyles(directoryPage);
  return directoryPage.generateMarkup();
}