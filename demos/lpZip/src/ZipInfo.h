// ���ڿƼ���Ȩ���� 2010-2022
// 
// �ļ�����ZipInfo.h
// ��  �ܣ��󶨵������洰�ڵ����ݶ���
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2017-09-02
// 
// ============================================================================

#ifndef _ZIPINFO_H_
#define _ZIPINFO_H_

#include "DriverItem.h"

class ZipInfo : public  suic::NotifyPropChanged
{
public:

    BeginMember(ZipInfo, suic::NotifyPropChanged)
        MemberInt(FileCount)
        MemberGetString(ZipSize)
        MemberGetString(FileSize)
        MemberGetString(ZipRate)
        MemberString(ZipType)
        MemberGetObject(DriverItems)
    EndMember()

    RTTIOfClass(ZipInfo)

    ZipInfo()
    {
        _zipSize = 0;
        _fileSize = 0;
        _zipRate = 0.1f;
        SetFileCount(0);
        SetZipSize(0);
        SetZipType("RAR");
        _driverGrp = new DriverGroup();
        _driverGrp->ref();
    }

    virtual ~ZipInfo()
    {
        _driverGrp->unref();
    }

    DefineInt(FileCount);
    DefineString(ZipType);

    suic::String GetZipSize()
    {
        suic::String strVal;
        strVal.Format(_U("%.2f KB"), _zipSize / 1024.0f);
        return strVal;
    }

    void SetZipSize(int size)
    {
        _zipSize = size;
        NotifyChanged("ZipSize");
    }

    suic::String GetFileSize()
    {
        suic::String strVal;
        strVal.Format(_U("��%.2f KB��%d �ֽڣ�"), _fileSize / 1024.0f, _fileSize);
        return strVal;
    }

    void SetFileSize(int size)
    {
        _fileSize = size;
        NotifyChanged("FileSize");
    }

    suic::String GetZipRate()
    {
        suic::String strVal;
        strVal.Format(_U("%.2f"), _zipRate);
        return strVal;
    }

    void SetZipRate(suic::Float val)
    {
        _zipRate = val;
        NotifyChanged("ZipRate");
    }

    suic::Object* GetDriverItems()
    {
        return _driverGrp;
    }

    DriverGroup* GetDriverGrp()
    {
        return _driverGrp;
    }

private:

    int _zipSize;
    int _fileSize;
    suic::Float _zipRate;
    DriverGroup* _driverGrp;
};

class ZipEntry : public  suic::NotifyPropChanged
{
public:

    BeginMember(ZipEntry, suic::NotifyPropChanged)
        MemberString(Name)
        MemberString(ZipBefore)
        MemberString(ZipAfter)
        MemberString(ZipType)
        MemberString(Date)
        MemberObject(Icon)
    EndMember()

    RTTIOfClass(ZipEntry)

    DefineString(Name);
    DefineString(ZipBefore);
    DefineString(ZipAfter);
    DefineString(ZipType);
    DefineString(Date);
    DefineObject(Icon);
};

#endif
