// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// FlashBox.cpp

#include "stdafx.h"

#if _VSMSC_VER_FLASH < 1600
#import "Flash11.tlb" raw_interfaces_only, named_guids, rename("IDispatchEx","IMyDispatchEx")
#endif

#include <System/Windows/CoreTool.h>
#include <System/Tools/CoreHelper.h>
#include <Extend/ActiveX/FlashBox.h>

namespace suic
{

ImplementRTTIOfClass(FlashBox, ActiveX)

suic::DpProperty* FlashBox::SourceProperty;
suic::DpProperty* FlashBox::PlayProperty;

void FlashBox::OnPlayPropChanged(suic::DpObject* d, suic::DpPropChangedEventArg* e)
{
    FlashBox* pFlash = suic::RTTICast<FlashBox>(d);
    if (NULL != pFlash)
    {
        if (e->GetNewValue()->ToBool())
        {
            pFlash->Play();
        }
        else
        {
            pFlash->Stop();
        }
    }
}

void FlashBox::StaticInit()
{
    if (NULL == SourceProperty)
    {
        SourceProperty = suic::DpProperty::Register(_U("Source"), RTTIType(), suic::OString::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::OString::EmptyString, suic::PropMetadataOptions::AffectsRender));
        PlayProperty  = suic::DpProperty::Register(_U("Play"), RTTIType(), suic::Boolean::RTTIType()
            , suic::DpPropMemory::GetPropMeta(suic::Boolean::False, suic::PropMetadataOptions::AffectsRender, &FlashBox::OnPlayPropChanged));
    }
}

FlashBox::FlashBox()
    : ActiveX(new ActiveXSite())
{
}

FlashBox::~FlashBox()
{
}

// flash�ؼ������ʶ����������flash�ؼ�
const suic::String FLASH_CLSID = _T("{D27CDB6E-AE6D-11CF-96B8-444553540000}");

bool FlashBox::LoadMovie(suic::String uri)
{
    if (uri.Empty())
    {
        return false;
    }

    // 
    // ���������flash�ļ��ľ���·����ַ
    //
    suic::String strUri = suic::FileDir::CalculatePath(uri);

#if _VSMSC_VER < 1600

    ShockwaveFlashObjects::IShockwaveFlash* pFlash = NULL;
    QueryControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void**)&pFlash);

    if (pFlash != NULL)
    {
        // 
        // ���ļ�����flash�����в���
        //
        HRESULT hr = pFlash->LoadMovie(0, _bstr_t(strUri.c_str()));

        // 
        // �ؼ�ʹ����ϼǵü������ü���
        //
        pFlash->Release();

        //
        // ����ؼ�
        //
        DoVerb(OLEIVERB_INPLACEACTIVATE);

        return true;
    }
    else
#endif
    {
        return false;
    }
}

void FlashBox::SetTransparent(bool bTransparent)
{
#if _VSMSC_VER < 1600
    ShockwaveFlashObjects::IShockwaveFlash* pFlash = NULL;
    QueryControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void**)&pFlash);

    if (pFlash != NULL) 
    {
        pFlash->put_WMode(_bstr_t(_T("Transparent")));
        pFlash->Release();
    }
#endif
}

void FlashBox::Play()
{
#if _VSMSC_VER < 1600
    LoadActiveX(FLASH_CLSID);

    ShockwaveFlashObjects::IShockwaveFlash* pFlash = NULL;
    QueryControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void**)&pFlash);

    if (pFlash != NULL) 
    {
        pFlash->Play();
        pFlash->Release();
    }
#endif
}

void FlashBox::Pause()
{
#if _VSMSC_VER < 1600
    ShockwaveFlashObjects::IShockwaveFlash* pFlash = NULL;
    QueryControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void**)&pFlash);

    if (pFlash != NULL) 
    {
        if (_site)
        {
            SetTransparent(true);
            LoadMovie(GetSource());
        }

        pFlash->StopPlay();
        pFlash->Release();
    }
#endif
}

void FlashBox::Stop()
{
#if _VSMSC_VER < 1600
    ShockwaveFlashObjects::IShockwaveFlash* pFlash = NULL;
    QueryControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (void**)&pFlash);

    if (pFlash != NULL) 
    {
        pFlash->StopPlay();
        pFlash->Release();
    }
#endif
}

/////////////////////////////////////////////////////////////////
//
void FlashBox::OnInitialized(suic::EventArg* e)
{
    ActiveX::OnInitialized(e);

    suic::ObjectPtr obj(GetValue(SourceProperty));

    if (obj)
    {
        SetSource(obj->ToString());
    }
}

void FlashBox::OnRender(suic::Drawing* drawing)
{
    ActiveX::OnRender(drawing);
}

void FlashBox::OnRenderSizeChanged(suic::SizeChangedInfo& e)
{
    suic::Rect rect(GetRenderBound(this));
    if (_site)
    {
        _site->SetControlRect(&rect);
    }
}

void FlashBox::OnVisibilityChanged()
{
    //ActiveX::OnVisibilityChanged();
}

bool FlashBox::OpenOverride()
{
    if (IsLoaded())
    {
        suic::Rect rect(GetRenderBound(this));
        if (_site)
        {
            _site->SetControlRect(&rect);

            SetTransparent(true);
            LoadMovie(GetSource());
        }
    }

    return true;
}

void FlashBox::OnLoaded(suic::LoadedEventArg* e)
{
    LoadActiveX(FLASH_CLSID);

    suic::Rect rect(GetRenderBound(this));
    if (_site)
    {
        _site->SetControlRect(&rect);

        SetTransparent(true);
        LoadMovie(GetSource());
    }

    ActiveX::OnLoaded(e);
}

void FlashBox::OnUnloaded(suic::LoadedEventArg* e)
{
    Stop();
    ActiveX::OnUnloaded(e);
}

}
