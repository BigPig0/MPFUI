// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����ImageResourcePool.h
// ��  �ܣ�ʵ�ֽ�����Դ�ļ��Ľ�����
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-08
// 
// ============================================================================

#ifndef _IMAGERESOURCEPOOL_H_
#define _IMAGERESOURCEPOOL_H_

#include <System/Windows/ResourceUri.h>
#include <System/Graphics/Bitmap.h>
#include <System/Types/Structure.h>

namespace suic
{

class ImageSourcePool
{
public:

    static ImageSource* Lookup(const String& uri);
    static void AddImage(ImageSource* img);
    static void RemoveImage(const String& uri);
    static void FreeImage(const String& uri);
    static void Close();

protected:

    ImageSourcePool() {};
};

}

#endif
