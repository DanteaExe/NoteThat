#pragma once
#include <gtkmm.h>
#include "DocumentManager.hpp"
#include "EditorView.hpp"

class Window
{
private:
    // Widgets
    Gtk::ApplicationWindow* window = nullptr;

    // Components
    std::unique_ptr<EditorView> editorView;     //We need an editor to manage text
    std::unique_ptr<DocumentManager> docManager; //We need to handle files

private:
    // ---------------- TITLE UPDATE ----------------
    void UpdateTitle()
    {
        std::string title = "NoteThat - ";
        title += docManager->GetFileName();
        
        // Add asterisk if modified
        if (docManager->IsModified())
            title += " *";
        
        window->set_title(title);
    }

    //Just the menu options
    void SetupMenu()
    {
        auto menu = Gio::Menu::create();
        menu->append("New", "app.file_new");
        menu->append("Open", "app.open");
        menu->append("Save", "app.save");
        menu->append("Save As", "app.save_as");
        menu->append("Quit", "app.quit");

        auto header = Gtk::make_managed<Gtk::HeaderBar>();
        window->set_titlebar(*header);

        auto menu_button = Gtk::make_managed<Gtk::MenuButton>();
        menu_button->set_label("File");
        menu_button->set_menu_model(menu);

        header->pack_start(*menu_button);
    }

    // ---------------- CLOSE CONFIRM ----------------
    bool OnCloseRequest()
    {
        // If document is NOT modified, allow closing
        if (!docManager->IsModified())
            return false; // allow close

        // Create confirmation dialog (GTK4 style)
        auto dialog = Gtk::AlertDialog::create();
        dialog->set_message("You have unsaved changes");
        dialog->set_detail("Do you want to save your changes before closing?");
        
        // Set button labels
        std::vector<Glib::ustring> buttons = {"Cancel", "Don't Save", "Save"};
        dialog->set_buttons(buttons);
        dialog->set_default_button(2);  // "Save" is default
        dialog->set_cancel_button(0);   // "Cancel" on Escape

        // Show dialog and handle response
        dialog->choose(
            *window,
            [this, dialog](const Glib::RefPtr<Gio::AsyncResult>& result)
            {
                try
                {
                    int response = dialog->choose_finish(result);
                    
                    if (response == 2)  // "Save"
                    {
                        // Set a callback to close AFTER saving completes
                        auto originalCallback = docManager->GetOnSaveCallback();
                        
                        docManager->SetOnSaveCallback(
                            [this, originalCallback](const std::string& path) {
                                // Call original callback first
                                if (originalCallback)
                                    originalCallback(path);
                                
                                // Now force close without asking again
                                docManager->SetModified(false);
                                window->close();
                            }
                        );
                        
                        docManager->Save(*window);
                    }
                    else if (response == 1)  // "Don't Save"
                    {
                        // Force close without saving
                        docManager->SetModified(false);
                        window->close();
                    }
                    // response == 0 is "Cancel" -> do nothing
                }
                catch (const Glib::Error& ex)
                {
                    // User cancelled or closed dialog
                    // Do nothing, keep window open
                }
            }
        );

        // Block closing for now (until user decides)
        return true;
    }

public:
    void OnActivate(const Glib::RefPtr<Gtk::Application>& app)
    {
        // ---- Window ----
        window = Gtk::make_managed<Gtk::ApplicationWindow>(app);
        window->set_title("NoteThat");
        window->set_default_size(800, 600);

        // Intercept window close (X button, Alt+F4, etc.)
        window->signal_close_request().connect(
            sigc::mem_fun(*this, &Window::OnCloseRequest),
            false
        );

        // ---- Editor View ----
        editorView = std::make_unique<EditorView>();

        // ---- Document Manager ----
        docManager = std::make_unique<DocumentManager>(
            editorView->GetBuffer()
        );

        // conect signal modification
        editorView->ConnectOnChanged([this]() {
            //If there is modifications on text
            docManager->SetModified(true);
            UpdateTitle();
        });

        // Callback to update title on save
        docManager->SetOnSaveCallback(
            [this](const std::string& path) {
                //If file is save or modified
                UpdateTitle();
            }
        );

        // ---- Set editor as main child ----
        window->set_child(*editorView->GetWidget());
        
        // ---- Initialize title ----
        UpdateTitle();

        // ---- App actions ----
        app->add_action("file_new", [this]() {
            docManager->New(*window);
        });

        app->add_action("save", [this]() {
            docManager->Save(*window);
        });

        app->add_action("save_as", [this]() {
            docManager->SaveAs(*window);
        });

        app->add_action("open", [this]() {
            docManager->Open(*window);
        });

        // Quit uses the SAME close logic
        app->add_action("quit", [this]() {
            window->close();
        });

        // ---- Menu ----
        SetupMenu();

        // ---- Show window (LAST) ----
        window->present();
    }
};