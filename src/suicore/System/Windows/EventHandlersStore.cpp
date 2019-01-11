// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// eventhandlersstore.cpp

#include <System/Windows/EventHandlersStore.h>

namespace suic 
{

RoutedEventHandlerInfo::RoutedEventHandlerInfo()
    : handlerType(0)
    , handler(NULL)
    , Object(false)
{
    
}

RoutedEventHandlerInfo::~RoutedEventHandlerInfo()
{
    if (handler)
    {
        handler->unref();
    }
}

void RoutedEventHandlerInfo::Clear()
{
    if (handler)
    {
        handler->unref();
        handler = NULL;
    }
}

RoutedEventHandlerInfoList::RoutedEventHandlerInfoList()
{

}

RoutedEventHandlerInfoList::~RoutedEventHandlerInfoList()
{
    Clear();
}

void RoutedEventHandlerInfoList::Clear()
{
    for (int i = 0; i < RoutedEHIList.Length(); ++i)
    {
        RoutedEHIList[i]->Clear();
        delete RoutedEHIList[i];
    }
    RoutedEHIList.Clear();
}

EventHandlersStore::EventHandlersStore()
{
}

EventHandlersStore::~EventHandlersStore()
{
    Clear();
}

void EventHandlersStore::Clear()
{
    Collection<RoutedEventHandlerInfoList*> values;
    _entries.GetValues(values);

    for (int i = 0; i < values.GetCount(); ++i)
    {
        values[i]->Clear();
        delete values[i];
    }

    /*ValueDic<int, RoutedEventHandlerInfoList*>::Enumerator enumer(&_entries, 0);
    
    while (enumer.HasNext())
    {
        delete enumer.Current()->val;
    }*/
    _entries.Clear();
}

RoutedEventHandlerInfoList* EventHandlersStore::GetRoutedEventHandlerInfoList(const RoutedEvent* routedEvent)
{
    RoutedEventHandlerInfoList* rehi = NULL;
    _entries.TryGetValue(routedEvent->GetIndex(), rehi);
    return rehi;
}

RoutedEventHandlerInfoList* EventHandlersStore::AddHandler(const RoutedEvent* routedEvent, Handler* handler, bool handledToo)
{
    RoutedEventHandlerInfoList* rehi = GetRoutedEventHandlerInfoList(routedEvent);
    RoutedEventHandlerInfo* reh(new RoutedEventHandlerInfo());

    reh->handler = handler;
    if (NULL != reh->handler)
    {
        reh->handler->ref();
    }

    reh->invokeHandledEventsToo = handledToo;

    if (NULL == rehi)
    {
        rehi = new RoutedEventHandlerInfoList();
        rehi->RoutedEHIList.Add(reh);
        _entries.Add(routedEvent->GetIndex(), rehi);

        return rehi;
    }
    else
    {
        rehi->RoutedEHIList.Add(reh);
        return rehi;
    }
}

void EventHandlersStore::RemoveHandler(const RoutedEvent* routedEvent, Handler* handler)
{
    RoutedEventHandlerInfoList* rehi = GetRoutedEventHandlerInfoList(routedEvent);

    if (NULL != rehi)
    {
        for (int i = 0; i < rehi->RoutedEHIList.Length(); ++i)
        {
            if (!handler)
            {
                delete rehi->RoutedEHIList[i];
            }
            else if (handler->Equals(rehi->RoutedEHIList[i]->handler))
            {
                delete rehi->RoutedEHIList[i];
                rehi->RoutedEHIList.RemoveAt(i);
                break;
            }
        }

        //
        // ���Ϊ�գ��������ע����¼�����
        //
        if (!handler)
        {
            rehi->RoutedEHIList.Clear();
        }

        if (rehi->RoutedEHIList.Length() == 0)
        {
            delete rehi;
            _entries.Remove(routedEvent->GetIndex());
        }
    }
}

}
