#include "App.h"

#include <string>
#include <sstream>
#include <fstream>

#include <Windows.h>

#include "json.hpp"

#include "MainWindow.h"
#include "Event.h"
#include "File.h"

namespace tower {
    App::App(HINSTANCE hInstance) :
        _hInstance(hInstance),
        _currentFile(nullptr),
        _mainWindow(nullptr) {

        _currentFile = new File();
        
        _mainWindow = new MainWindow(hInstance);
        _mainWindow->addEventListener(this);
    }

    App::~App() {
        delete _mainWindow;
        delete _currentFile;
    }

    void App::mainLoop() {
        _mainWindow->mainLoop();
    }

    void App::onEvent(Event* event) {
        if (event->getName() == "exit") {
            operationExit();
        } else if (event->getName() == "changed") {
            if (_currentFile->getState() != FileStates::uncreated) {
                _currentFile->setState(FileStates::modified);
            }
            
            _setWindowTitle();
        } else if (event->getName() == "newFile") {
            operationNewFile();
        } else if (event->getName() == "openFile") {
            operationOpenFile();
        } else if (event->getName() == "saveFile") {
            operationSaveFile();
        } else if (event->getName() == "saveFileAs") {
            operationSaveFileAs();
        }
    }

    void App::operationNewFile() {
        if (_currentFile->getState() != FileStates::saved &&
            _mainWindow->getEditor()->getTextLength() > 0) {
            if (!_askNewConfirmation()) {
                return;
            }
        }
    
        _mainWindow->getEditor()->clear();

        delete _currentFile;
        _currentFile = new File();

        _setWindowTitle();
    }

    void App::operationOpenFile() {
        if (_currentFile->getState() != FileStates::saved &&
            _mainWindow->getEditor()->getTextLength() > 0) {
            if (!_askOpenConfirmation()) {
                return;
            }
        }
    
        std::wstring filename = _askFilePath(true);

        if (filename.empty()) {
            return;
        }

        delete _currentFile;
        _currentFile = new File(filename);

        _readCurrentFile();
        _setWindowTitle();
    }

    void App::operationSaveFile() {
        if (_currentFile->getState() == FileStates::uncreated) {
            operationSaveFileAs();
        } else {
            _writeCurrentFile();
            _setWindowTitle();
        }
    }

    void App::operationSaveFileAs() {
        std::wstring filename = _askFilePath(false);

        if (filename.empty()) {
            return;
        }

        delete _currentFile;
        _currentFile = new File(filename);

        _writeCurrentFile();
        _setWindowTitle();
    }

    void App::operationExit() {
        if (_currentFile->getState() != FileStates::saved &&
            _mainWindow->getEditor()->getTextLength() > 0) {
           if (_askQuitConfirmation()) {
               _mainWindow->destroy();
           }
        } else {    
            _mainWindow->destroy();
        }
    }

    void App::_readCurrentFile() {
        std::wstring content = _currentFile->read();
    
        _mainWindow->getEditor()->setText(content.c_str());
    }

    void App::_writeCurrentFile() {
        const int length = _mainWindow->getEditor()->getTextLength();
        wchar_t* text = new wchar_t[length + 1];

        _mainWindow->getEditor()->getText(text, length + 1);

        _currentFile->write(std::wstring(text));

        delete[] text;
    }

    std::wstring App::_askFilePath(bool mustExist) {
        OPENFILENAME ofn;
        wchar_t szFile[260];

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = _mainWindow->getHandle();
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;

        if (mustExist) {
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if(GetOpenFileName(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }
        } else {
            ofn.Flags = OFN_PATHMUSTEXIST;

            if(GetSaveFileName(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }
        }

        return L"";
    }

    void App::_setWindowTitle() {
        std::wstring title = L"Tower Editor";

        if (_currentFile->getState() != FileStates::uncreated) {
            title += L" - ";
            title += _currentFile->getFilename();
        }
        
        if (_currentFile->getState() != FileStates::saved) {
            title += L" (Unsaved)";
        }

        _mainWindow->setTitle(title);
    }
    
    bool App::_askQuitConfirmation() {
        return MessageBox(nullptr, L"You have unsaved changes. Do you still want to quit?", 
                                   L"Confirm application exit", MB_ICONEXCLAMATION | MB_YESNO) == IDYES;
    }
    
    bool App::_askNewConfirmation() {
        return MessageBox(nullptr, L"You have unsaved changes. Do you still want to create a new file?", 
                                   L"Confirm new file", MB_ICONEXCLAMATION | MB_YESNO) == IDYES;
    }
    
    bool App::_askOpenConfirmation() {
        return MessageBox(nullptr, L"You have unsaved changes. Do you still want to open a file?", 
                                   L"Confirm open file", MB_ICONEXCLAMATION | MB_YESNO) == IDYES;
    }
}
