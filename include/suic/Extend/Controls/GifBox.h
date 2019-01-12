// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����GifBox.h
// ��  �ܣ�ʵ��GIF�ļ����š�
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2011-07-02
// 
// ============================================================================

#ifndef _UIGIFBOX_H_
#define _UIGIFBOX_H_

#include <Extend/suiwgxconfig.h>
#include <System/Graphics/GIFParser.h>
#include <Framework/Controls/Content.h>

namespace suic
{

class SUIWGX_API GifBox : public suic::FrameworkElement
{
public:

    static suic::DpProperty* UriProperty;
    static suic::DpProperty* PlayProperty;

    static void OnUriPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);
    static void OnPlayPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e);

    static void StaticInit();

    GifBox();
    virtual ~GifBox();

    RTTIOfClass(GifBox)

    suic::String GetUri();
    void SetUri(suic::String uri);

    bool IsPlay();
    bool IsValid() const;

protected:

    void OnInitialized(suic::EventArg* e);
    void OnLoaded(suic::LoadedEventArg* e);
    void OnTick(suic::Object* sender, suic::EventArg* e);

    suic::Size OnMeasure(const suic::Size& constraint);
    void OnArrange(const suic::Size& arrangeSize);

    void OnRender(suic::Drawing* drawing);

    void StartGif();

private:

    suic::Bitmap _bitmap;
    //suic::GIFParser* _parser;
    suic::AssignerTimer* _timer;
};

inline bool GifBox::IsValid() const
{
    //if (NULL == _parser || !_parser->IsLoaded())
	if(1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

}

#endif
