/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychen2 <ychen2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/25 21:11:22 by ychen2            #+#    #+#             */
/*   Updated: 2024/08/25 21:16:19 by ychen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "helper.hpp"

// Function to split a string by a delimiter
std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);

  while (std::getline(tokenStream, token, delimiter)) {
      tokens.push_back(token);
  }

  return tokens;
}

// Function to check if a file is executable
bool isExecutable(const std::string &path) {
  struct stat sb;
  return (stat(path.c_str(), &sb) == 0 && sb.st_mode & S_IXUSR);
}

// Function to find the full path of the program
std::string find_cgi_path(const std::string &program) {
  const char *pathEnv = std::getenv("PATH");
  if (!pathEnv) {
    throw std::runtime_error("PATH environment variable not found.\n");
  }
  std::string pathStr(pathEnv);
  std::vector<std::string> directories = split(pathStr, ':');

  for (std::vector<std::string>::iterator it = directories.begin(); it != directories.end(); ++it) {
      std::string fullPath = *it + "/" + program;
      if (isExecutable(fullPath)) {
          return fullPath;
      }
  }
  throw std::runtime_error("Program not found.\n");
}

std::string getMimeType(const std::string& fileName) {
    // Define the MIME types using a map
    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".ico"] = "image/x-icon";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".zip"] = "application/zip";
    mimeTypes[".rar"] = "application/x-rar-compressed";
    mimeTypes[".tar"] = "application/x-tar";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".wav"] = "audio/wav";
    mimeTypes[".ogg"] = "audio/ogg";
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".avi"] = "video/x-msvideo";
    mimeTypes[".mpeg"] = "video/mpeg";
    mimeTypes[".webm"] = "video/webm";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".csv"] = "text/csv";
    mimeTypes[".rtf"] = "application/rtf";
    mimeTypes[".xls"] = "application/vnd.ms-excel";
    mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeTypes[".doc"] = "application/msword";
    mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
    mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mimeTypes[".epub"] = "application/epub+zip";
    mimeTypes[".otf"] = "font/otf";
    mimeTypes[".ttf"] = "font/ttf";
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"] = "font/woff2";
    mimeTypes[".eot"] = "application/vnd.ms-fontobject";

    // Find the last dot in the filename
    size_t dotPos = fileName.find_last_of('.');

    if (dotPos != std::string::npos) {
        // Extract the extension from the file name
        std::string extension = fileName.substr(dotPos);

        // Convert extension to lowercase
        for (size_t i = 0; i < extension.length(); ++i) {
            extension[i] = std::tolower(extension[i]);
        }

        // Look up the MIME type in the map
        std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }

    // Default MIME type if no match found
    return "application/octet-stream";
}
