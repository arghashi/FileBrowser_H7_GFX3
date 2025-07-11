#ifndef MODEL_HPP
#define MODEL_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/Application.hpp>
#include <cmsis_os2.h>
#include "file_browser.h"


class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();


    // FileSystemInterface implementations
    virtual void createFolder(const char* folderName);
    virtual void deleteItem(const char* path);
    virtual void createFile(const char* fileName);
    virtual void writeToFile(const char* fileName, const char* data, bool append);
    virtual void readFile(const char* fileName);
    virtual void renameItem(const char* oldName, const char* newName);
    virtual void enterFolder(const char* folderName);
    virtual void exitFolder();
    virtual void goToRoot();
    virtual void formatSDCard();
    virtual void getStorageInfo();
    virtual void getCurrentPath(char* buffer, size_t size);



protected:
    ModelListener* modelListener;

private:
    void processFileCommands();
    void updateFileList();
    void updatePath();
    void updateStorageInfo();
    void processFileContent();
};

#endif // MODEL_HPP
