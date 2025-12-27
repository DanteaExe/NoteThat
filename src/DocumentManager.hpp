#pragma once
#include <gtkmm.h>
#include <fstream>
#include <iostream>
#include <functional>
#include "FileValidator.hpp"

class DocumentManager
{
private:
    Glib::RefPtr<Gtk::TextBuffer> buffer;
    std::string currentFile; // Which file is opened?
    bool isModified = false; // Are there unsave changes?
    std::function<void(const std::string&)> onSaveCallback;

private:
    void WriteToFile(const std::string& path, Gtk::Window& parent)
    {
        std::ofstream out(path);
        if (!out)
        {
            std::cerr << "Could not open file for writing\n";
            
            // Show error dialog to user
            auto errorDialog = Gtk::AlertDialog::create();
            errorDialog->set_message("Cannot save file");
            errorDialog->set_detail("Failed to write to file: " + path);
            errorDialog->set_modal(true);
            
            errorDialog->show(parent);
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

    void ReadFromFile(const std::string& path, Gtk::Window& parent)
    {
        std::ifstream in(path);
        if (!in)
        {
            std::cerr << "Could not open file for reading\n";
            
            // Show error dialog to user
            auto errorDialog = Gtk::AlertDialog::create();
            errorDialog->set_message("Cannot read file");
            errorDialog->set_detail("Failed to open file: " + path);
            errorDialog->set_modal(true);
            
            errorDialog->show(parent);
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

    // Helper: Ask user what to do with unsaved changes
    void ConfirmUnsavedChanges(
        Gtk::Window& parent,
        std::function<void()> onProceed)
    {
        // If no changes, just proceed
        if (!isModified)
        {
            onProceed();
            return;
        }

        // Create confirmation dialog
        auto dialog = Gtk::AlertDialog::create();
        dialog->set_message("You have unsaved changes");
        dialog->set_detail("Do you want to save before proceeding?");
        
        std::vector<Glib::ustring> buttons = {"Cancel", "Don't Save", "Save"};
        dialog->set_buttons(buttons);
        dialog->set_default_button(2);  // "Save" is default
        dialog->set_cancel_button(0);   // "Cancel" on Escape

        dialog->choose(
            parent,
            [this, dialog, onProceed, &parent](const Glib::RefPtr<Gio::AsyncResult>& result)
            {
                try
                {
                    int response = dialog->choose_finish(result);
                    
                    if (response == 2)  // "Save"
                    {
                        // Save first, then proceed
                        auto originalCallback = onSaveCallback;
                        
                        // Temporarily replace callback to proceed after save
                        onSaveCallback = [this, originalCallback, onProceed](const std::string& path) {
                            // Restore original callback
                            onSaveCallback = originalCallback;
                            
                            // Call it
                            if (originalCallback)
                                originalCallback(path);
                            
                            // Now proceed with the action
                            onProceed();
                        };
                        
                        Save(parent);
                    }
                    else if (response == 1)  // "Don't Save"
                    {
                        // Just proceed without saving
                        onProceed();
                    }
                    // response == 0 is "Cancel" -> do nothing
                }
                catch (const Glib::Error& ex)
                {
                    // User cancelled -> do nothing
                }
            }
        );
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
        WriteToFile(currentFile, parent);
    }

    void SaveAs(Gtk::Window& parent)
    {
        auto dialog = Gtk::FileDialog::create();
        dialog->set_title("Save File");
        dialog->save(
            parent,
            [this, dialog, &parent](const Glib::RefPtr<Gio::AsyncResult>& result)
            {
                try
                {
                    auto file = dialog->save_finish(result);
                    if (!file)
                        return;
                    auto path = file->get_path();
                    if (path.empty())
                        return;
                    WriteToFile(path, parent);
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
        // Ask user about unsaved changes first
        ConfirmUnsavedChanges(parent, [this, &parent]() {
            // Now show the open dialog
            auto dialog = Gtk::FileDialog::create();
            dialog->set_title("Open File");
            
            dialog->open(
                parent,
                [this, dialog, &parent](const Glib::RefPtr<Gio::AsyncResult>& result)
                {
                    try
                    {
                        auto file = dialog->open_finish(result);
                        if (!file)
                            return;
                        
                        auto path = file->get_path();
                        if (path.empty())
                            return;
                        
                        // Validate file using FileValidator
                        std::string errorMessage;
                        if (!FileValidator::CanOpen(file, errorMessage))
                        {
                            std::cerr << "Error: " << errorMessage << std::endl;
                            
                            // Show error dialog to user
                            auto errorDialog = Gtk::AlertDialog::create();
                            errorDialog->set_message("Cannot open file");
                            errorDialog->set_detail(errorMessage);
                            errorDialog->set_modal(true);
                            
                            errorDialog->show(parent);
                            return;
                        }
                        
                        ReadFromFile(path, parent);
                    }
                    catch (const Glib::Error& ex)
                    {
                        std::cerr << "Open failed: " << ex.what() << std::endl;
                    }
                }
            );
        });
    }

    void New(Gtk::Window& parent)
    {
        // Ask user about unsaved changes first
        ConfirmUnsavedChanges(parent, [this]() {
            // Now create new document
            buffer->set_text("");
            currentFile.clear();
            isModified = false;
            
            std::cout << "New file created" << std::endl;
            
            // Notify to update title
            if (onSaveCallback)
                onSaveCallback("");  // Empty path = "Untitled"
        });
    }
};