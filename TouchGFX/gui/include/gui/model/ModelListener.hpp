#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
#include <touchgfx/hal/Types.hpp>
#include <queue>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    // توابع برای ارسال دستورات به کتابخانه file_browser
//    virtual void addFileItem(const char* name, uint32_t size, bool isDir) {}
    virtual void updateFileList(FileInfo items, int count){}
    virtual void updateCurrentPath(const char* path)  {}
    virtual void updateStorageInfo(uint32_t totalKB, uint32_t usedKB, uint32_t freeKB)  {}
    virtual void addFileContent(const char* content)  {}
    virtual void fileReadComplete()  {}
    virtual void showSystemMessage(SystemMessage msg)  {}


protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
