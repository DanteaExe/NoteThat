#pragma once
#include <giomm.h>
#include <string>
#include <vector>
#include <iostream>

class FileValidator
{
private:
    // Check if MIME type is in the rejected list
    static bool IsRejectedType(const std::string& contentType)
    {
        // List of MIME types we explicitly REJECT
        static const std::vector<std::string> rejectedTypes = {
            "image/",           // All images (png, jpg, gif, etc.)
            "video/",           // All videos (mp4, avi, mkv, etc.)
            "audio/",           // All audio (mp3, wav, flac, etc.)
            "application/pdf",  // PDFs
            "application/zip",  // Compressed files
            "application/x-tar",
            "application/x-rar",
            "application/x-7z-compressed",
            "application/gzip",
            "application/vnd.ms-excel",           // Excel
            "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
            "application/vnd.ms-powerpoint",      // PowerPoint
            "application/vnd.openxmlformats-officedocument.presentationml.presentation",
            "application/msword",                 // Word
            "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
            "application/octet-stream"            // Generic binary
        };
        
        for (const auto& rejectedType : rejectedTypes)
        {
            if (contentType.find(rejectedType) != std::string::npos)
            {
                return true;
            }
        }
        
        return false;
    }

public:
    // Validate if a file can be opened in a text editor
    static bool CanOpen(const Glib::RefPtr<Gio::File>& file, std::string& errorMessage)
    {
        if (!file)
        {
            errorMessage = "Invalid file";
            return false;
        }

        try
        {
            // Get file info
            auto fileInfo = file->query_info(
                G_FILE_ATTRIBUTE_STANDARD_TYPE "," 
                G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE
            );
            
            // Check if it's a regular file (not a directory)
            if (fileInfo->get_file_type() != Gio::FileType::REGULAR)
            {
                errorMessage = "Cannot open directories";
                return false;
            }
            
            // Check MIME type
            std::string contentType = fileInfo->get_content_type();
            
            if (IsRejectedType(contentType))
            {
                errorMessage = "Cannot open binary/media file (type: " + contentType + ")";
                return false;
            }
            
            // All checks passed
            return true;
        }
        catch (const Glib::Error& ex)
        {
            errorMessage = "Failed to validate file: " + std::string(ex.what());
            return false;
        }
    }
};