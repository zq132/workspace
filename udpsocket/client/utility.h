#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>

#ifdef __cplusplus
extern "C" {
#endif
    
    namespace ecutility{
        double GetViewScaleFactor(double srcWidth, 
                                  double srcHeight, 
                                  double sceneWidth, 
                                  double sceneHeight);
        //获取文件大小
        long GetFileSize(const char *path);
                
        //获取应用程序所在路径
        std::string GetAppDir();
        //获取应用程序名称
        std::string GetAppName();
        //创建目录
        int CreateDir(const std::string &dir);
        //字符串去空格
        std::string& TrimString(std::string &string);
        //tab转空格
        std::string& TrimTab(std::string &string);
        //判断当前是否有相同的程序在运行
        bool RunSingle();
        
        unsigned short  crc16(unsigned char *buf, int length);

        void gbk2utf8(char *src, size_t *srclen, char *dst, size_t *dstlen);
    }

#ifdef __cplusplus
}
#endif

#endif

