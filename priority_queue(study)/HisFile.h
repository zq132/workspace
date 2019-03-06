#ifndef __HISTORY_FILE_MANAGER_H__
#define __HISTORY_FILE_MANAGER_H__

#include <time.h>
#include <string>
#include <queue>
#include <vector>

const time_t MIN_PERIOD = 1296000;          //15天
const time_t MAX_PERIOD = 5184000;          //60天

struct FileAttr
{
    std::string filename;   //历史文件名称，全路径
    time_t filetime;        //历史文件创建时间
};

class CompareFileAttr
{
public:
    bool operator()(const FileAttr &a, const FileAttr &b){
        return a.filetime > b.filetime;
    }
};

typedef std::priority_queue<FileAttr, std::vector<FileAttr>, CompareFileAttr> QFILE;

class HisFileManager
{
private:
    HisFileManager();
public:
    static HisFileManager& GetInstance();
    void Init(const char* dir);
    
    void Push(const char* filename, time_t t);
    void SetPeriod(int days);
    //const FileAttr& Front();
    //void Pop();
    void Remove();
private:
    void Push(const FileAttr &fa);

    //默认15天以前的数据清除
    time_t m_BeforeTime = 1296000;
    QFILE m_files;          //文件列表
};

#endif
