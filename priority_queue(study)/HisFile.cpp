#include "HisFile.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glog/logging.h>
#include <algorithm>

HisFileManager::HisFileManager()
{

}

HisFileManager& HisFileManager::GetInstance()
{
    static HisFileManager _instance;
    return _instance;
}

void HisFileManager::Init(const char *path)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
 
    if( (dir = opendir(path)) == NULL){
        LOG(ERROR) << "无法打开文件夹: " << path;
        return;
    }
 
    while( (ptr = readdir(dir)) != NULL ){
        if( strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0 ){
            continue;
        }else if( ptr->d_type == 8 ){    
            //file
            char fname[1024];
            sprintf(fname, "%s/%s", path, ptr->d_name);
            struct stat st;
            if( stat(fname, &st) < 0 ){
                continue;
            }
            FileAttr fa;
            fa.filename = fname;
            fa.filetime = st.st_mtim.tv_sec;
            Push(fa);
        }
    }
/* 
    //测试
    while(!m_files.empty()){
        const FileAttr &fa = m_files.top();
        LOG(INFO) << fa.filename << " : " << fa.filetime;
        m_files.pop();
    }
    //测试结束
     */
    closedir(dir);
}

void HisFileManager::Push(const FileAttr &fa)
{
    m_files.push(fa);
}

void HisFileManager::Push(const char* filename, time_t t)
{
    FileAttr fa;
    fa.filename = filename;
    fa.filetime = t;
    m_files.push(fa);
}

void HisFileManager::SetPeriod(int days)
{
    time_t period = days * 24 * 60 * 60;
    if( period >= MIN_PERIOD && period <= MAX_PERIOD ){
        LOG(INFO) << "历史文件最多保存" << days << "天";
        m_BeforeTime = period;
    }
}

/* 
const FileAttr& HisFileManager::Front()
{
    return m_files.front();
}

void HisFileManager::Pop()
{
    m_files.pop();
}
 */
void HisFileManager::Remove()
{
    time_t t = time(0);
    //LOG(INFO) << "文件总数: " << m_files.size();
    for( int i = 0; i < 2 && !m_files.empty(); ++i ){
        //一次最多删除两个文件
        const FileAttr &fa = m_files.top();
        //LOG(INFO) << "文件: " << fa.filename;
        if( t - fa.filetime < m_BeforeTime ){
            break;
        }
        //LOG(INFO) << "删除历史数据: " << fa.filename;
        remove(fa.filename.c_str());
        m_files.pop();
    }
}