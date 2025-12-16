#pragma once
#include <gtkmm.h>
#include <fstream>
#include <iostream>
#include <functional>

class DocumentManager
{
private:
    Glib::RefPtr<Gtk::TextBuffer> buffer;
    std::string currentFile; //which file is open?
    bool isModified = false; //are there changes unsave?
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
        
        // Notificar que se guardó exitosamente
        if (onSaveCallback)
            onSaveCallback(path);
    }

public:
    DocumentManager(Glib::RefPtr<Gtk::TextBuffer> buffer)
    {
        this.buffer = buffer;
    }

    // Getters
    bool IsModified() const { return isModified; }
    std::string GetCurrentFile() const { return currentFile; }
    
    // Setters
    void SetModified(bool modified) { isModified = modified; }
    
    // Callback para cuando se guarda exitosamente
    void SetOnSaveCallback(std::function<void(const std::string&)> callback)
    {
        onSaveCallback = callback;
    }

    // File operations
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
};