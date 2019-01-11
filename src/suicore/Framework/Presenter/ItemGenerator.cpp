//////////////////////////////////////////////////////////////////////////////
// uiitemgenerator.cpp

#include <System/Tools/Debug.h>
#include <Framework/Controls/ItemsElement.h>
#include <Framework/Presenter/ItemGenerator.h>

namespace suic
{

ImplementTypeofClass(ItemGenerator, RefBase)

ItemGenerator::ItemGenerator(ItemsElement* owner)
    : _currPos(0)
    , _itemsOwner(owner)
    , _bCache(false)
{
}

ItemGenerator::~ItemGenerator()
{
}

ItemsList* ItemGenerator::GetItemsList()
{
    return _itemsOwner->GetItems();
}

Element* ItemGenerator::GetNext()
{
    if (_currPos < _itemsOwner->GetCount())
    {
        RefBase* item(_itemsOwner->GetItem(_currPos));
        Element* elem(ContainerFromItem(item, _currPos));

        _currPos++;
        return elem;
    }
    else
    {
        return NULL;
    }
}

RefBase* ItemGenerator::GetItem()
{
    return _itemsOwner->GetItem(_currPos - 1);
}

Size ItemGenerator::Estimate(RefBase* item)
{
    return Size(20,20);
}

Element* ItemGenerator::ContainerFromIndex(int pos)
{
    RefBase* item(GetItemsList()->GetItem(pos));
    Element* elem = ContainerFromItem(item, pos);
    elem->SetItemIndex(pos);
    return elem;
}

void ItemGenerator::CacheRecycElement(Element* elem)
{
    if (elem->IsRecycCacheElement())
    {
        _recycCache.push_back(elem);
        elem->__addRef();
        /*if (elem->IsFocused() && !_itemsOwner->GetFocusedItem())
        {
            _itemsOwner->SetFocus();
        }*/

        //
        // ֪ͨԪ���Ѿ���ѭ������
        //
        elem->OnOffVisualView(_itemsOwner);
    }
    else
    {
        Debug::Assert(false, _U("not a recyc element"));
    }
}

Element* ItemGenerator::ContainerFromItem(RefBase* item, int index)
{
    Element* container = NULL;

    // �����������
    if (!_itemsOwner->ItemIsContainer(item))
    {
        if (_bCache && _recycCache.size() > 0)
        {
            //
            // Ϊ����߷���Ч�ʣ��Ӻ���ȡ�����Ԫ��
            //
            int iSize = _recycCache.size() - 1;
            container = _recycCache[iSize];
            bool autodel = container->__setAutoDelete(false);

            //
            // �Ƴ����һ��Ԫ��
            //
            _recycCache.erase(_recycCache.begin() + iSize);

            //
            // �������������Ӧ����Ԫ��
            //
            container->SetDataContext(item);
            container->OnInVisualView(_itemsOwner);

            container->__release();
            container->__setAutoDelete(autodel);
        }
        else
        {
            //
            // ȡ����������������������������������
            // ��ֱ�ӷ���
            //
            container = _itemsOwner->GetContainer();
            //
            // �������������Ӧ����Ԫ��
            //
            container->SetDataContext(item);
            container->OnInVisualView(_itemsOwner);
            container->WriteViewFlag(ViewFlags::IsRecycCacheElement, true);
        }
    }
    else
    {
        container = CastElem<Element>(item);
        container->OnInVisualView(_itemsOwner);
        container->WriteViewFlag(ViewFlags::IsRecycCacheElement, false);
    }

    return container;
}

}
