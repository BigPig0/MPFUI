// ���ڿƼ���Ȩ���� 2010-2011
// 
// �ļ�����Encry.h
// ��  �ܣ������㷨�ļ���
// 
// ��  �ߣ�����
// ʱ  �䣺2012-02-02
// 
// ============================================================================

#ifndef _UIDECRYPT_H_
#define _UIDECRYPT_H_

#include <System/Windows/Object.h>
#include <System/Types/String.h>
#include <System/Types/StreamDef.h>

class UIDecrypt
{
public:

    static int Decrypt(const suic::String& pwd, suic::String path, suic::String outFile);
    static int Decrypt(const suic::String& pwd, suic::Byte* data, int iSize, suic::ByteStream* outData);

protected:

    UIDecrypt();
};

#endif
