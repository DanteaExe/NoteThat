#pragma once
#include <gtkmm.h>
#include <fstream>
#include <iostream>
#include <functional>

class DocumentManager
{
private:
    Glib::RefPtr<Gtk::TextBuffer> buffer;
    std::string currentFile; // Which file is opened?
    bool isModified = false; // Are there unsave changes?
    std::function<void(const std::string&)> onSaveCallback;

private:
    void WriteToFile(const std::string& path)
    {
        std::ofstream out(path);
        if (!out)
        {
            std::cerr << "Could not open file for writing\n";
            return;
        }
        
        out << buffer->get_text();
        out.close();
        
        currentFile = path;
        isModified = false;
        
        std::cout << "Saved file: " << path << std::endl;
        
        // Notify that was save successfully
        if (onSaveCallback)
            onSaveCallback(path);
    }

    void ReadFromFile(const std::string& path)
    {
        std::ifstream in(path);
        if (!in)
        {
            std::cerr << "Could not open file for reading\n";
            return;
        }
        
        std::string content(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>()
        );
        
        buffer->set_text(content);
        currentFile = path;
        isModified = false;
        
        std::cout << "Opened file: " << path << std::endl;
        
        // Notify that was opened successfully
        if (onSaveCallback)
            onSaveCallback(path);
    }

public:
    DocumentManager(Glib::RefPtr<Gtk::TextBuffer> buffer)
        : buffer(buffer)
    {
    }

    // ---- Getters ----
    bool IsModified() const { return isModified; }
    std::string GetCurrentFile() const { return currentFile; }
    
    // Get just the filename (not full path)
    std::string GetFileName() const 
    {
        if (currentFile.empty())
            return "Untitled";
        
        // Find last '/' to get just the filename
        size_t lastSlash = currentFile.find_last_of('/');
        if (lastSlash != std::string::npos)
            return currentFile.substr(lastSlash + 1);
        
        return currentFile;
    }
    
    std::function<void(const std::string&)> GetOnSaveCallback() const
    {
        return onSaveCallback;
    }
    
    // ---- Setters ----
    void SetModified(bool modified) { isModified = modified; }
    
    void SetOnSaveCallback(std::function<void(const std::string&)> callback)
    {
        onSaveCallback = callback;
    }

    // ---- File operations ----
    void Save(Gtk::Window& parent)
    {
        if (currentFile.empty())
        {
            SaveAs(parent);
            return;
        }
        WriteToFile(currentFile);
    }

    void SaveAs(Gtk::Window& parent)
    {
        auto dialog = Gtk::FileDialog::create();
        dialog->set_title("Save File");
        dialog->save(
            parent,
            [this, dialog](const Glib::RefPtr<Gio::AsyncResult>& result)
            {
                try
                {
                    auto file = dialog->save_finish(result);
                    if (!file)
                        return;
                    auto path = file->get_path();
                    if (path.empty())
                        return;
                    WriteToFile(path);
                }
                catch (const Glib::Error& ex)
                {
                    std::cerr << "Save failed: " << ex.what() << std::endl;
                }
            }
        );
    }

    void Open(Gtk::Window& parent)
    {
        auto dialog = Gtk::FileDialog::create();
        dialog->set_title("Open File");
        dialog->open(
            parent,
            [this, dialog](const Glib::RefPtr<Gio::AsyncResult>& result)
            {
                try
                {
                    auto file = dialog->open_finish(result);
                    if (!file)
                        return;
                    
                    auto path = file->get_path();
                    if (path.empty())
                        return;
                    
                    ReadFromFile(path);
                }
                catch (const Glib::Error& ex)
                {
                    std::cerr << "Open failed: " << ex.what() << std::endl;
                }
            }
        );
    }

    void New()
    {
        buffer->set_text("");
        currentFile.clear();
        isModified = false;
        
        std::cout << "New file created" << std::endl;
        
        // Notify to update title
        if (onSaveCallback)
            onSaveCallback("");  // Empty path = "Untitled"
    }
};