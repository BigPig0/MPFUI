
#include <Framework/Items/ItemContainerGeneratorEx.h>

namespace suic
{

DpProperty* ItemContainerGeneratorEx::ItemForItemContainerProperty;

//=================================================
// ItemContainerGeneratorEx

ItemContainerGeneratorEx::ItemContainerGeneratorEx(IGeneratorHost* host)
{
    _parent = NULL;
    SetHost(host);
    InitData(dynamic_cast<DpObject*>(host), 0);
}

ItemContainerGeneratorEx::ItemContainerGeneratorEx(ItemContainerGeneratorEx* parent, GroupItem* groupItem)
{
    _parent = parent;
    SetHost(parent->GetHost());
    InitData(groupItem, parent->GetLevel() + 1);
}

ItemContainerGeneratorEx::ItemContainerGeneratorEx(ItemContainerGeneratorEx* parent, IGeneratorHost* host, DpObject* peer, int level)
{
    _parent = parent;
    SetHost(host);
    InitData(peer, level);
}

void ItemContainerGeneratorEx::InitData(DpObject* peer, int level)
{
    _peer = peer;
    _level = level;
    _groupStyle = NULL;
    _alternationCount = 0;
    _direction = GeneratorDirection::Forward;
}

Object* ItemContainerGeneratorEx::ItemFromContainer(DpObject* container)
{
    if (container == NULL)
    {
        throw ArgumentNullException("container", __FILELINE__);
    }
    Object* unsetValue = container->ReadLocalValue(ItemForItemContainerProperty, false);
    if ((unsetValue != DpProperty::UnsetValue()) && !GetHost()->IsHostForItemContainer(container))
    {
        unsetValue = DpProperty::UnsetValue();
    }
    return unsetValue;
}

int ItemContainerGeneratorEx::IndexFromContainer(DpObject* container)
{
    if (container == NULL)
    {
        throw ArgumentNullException("container", __FILELINE__);
    }
    Object* item = NULL;
    item = container->ReadLocalValue(ItemForItemContainerProperty, false);
    return DoLinearSearch(container, item);
}

DpObject* ItemContainerGeneratorEx::ContainerFromItem(Object* item)
{
    DpObject* container = NULL;
    int num = DoLinearSearch(container, item);
    return container;
}

DpObject* ItemContainerGeneratorEx::ContainerFromIndex(int index)
{
    return _itemMap[index].container;
}

int ItemContainerGeneratorEx::DoLinearSearch(DpObject*& container, Object* item)
{
    int index = _items->IndexOf(item);
    if (index >= 0)
    {
        container = _itemMap[index].container;
    }
    return index;
}

void ItemContainerGeneratorEx::SetItems(IList* val)
{
    if (_items != val)
    {
        _items = val;
    }
}

void ItemContainerGeneratorEx::Recycle()
{
}

void ItemContainerGeneratorEx::Refresh()
{

}

ItemEntry* ItemContainerGeneratorEx::GetItemEntry(int index)
{
    return &(_itemMap[index]);
}

DpObject* ItemContainerGeneratorEx::PopFromRecycle()
{
    return NULL;
}

bool ItemContainerGeneratorEx::ShouldHide(CollectionViewGroup* group) const
{
    return (GetGroupStyle()->HidesIfEmpty() && group->GetItemCount() == 0);
}

DpObject* ItemContainerGeneratorEx::ContainerForGroup(CollectionViewGroup* group)
{
    if (!ShouldHide(group))
    {
        GroupItem* container = new GroupItem();
        LinkContainerToItem(container, group);
        container->SetGenerator(new ItemContainerGeneratorEx(this, container));
        return container;
    }
    AddEmptyGroupItem(group);
    return NULL;
}

void ItemContainerGeneratorEx::AddEmptyGroupItem(CollectionViewGroup* group)
{
    EmptyGroupItem* container = new EmptyGroupItem();
    LinkContainerToItem(container, group);
    container->SetGenerator(new ItemContainerGeneratorEx(this, container));
    _emptyGroupItems.Add(container);
}

void ItemContainerGeneratorEx::SetAlternationCount()
{
    int alternationCount;
    if (IsGrouping() && (GetGroupStyle() != NULL))
    {
        if (GetGroupStyle()->GetIsAlternationCountSet())
        {
            alternationCount = GetGroupStyle()->GetAlternationCount();
        }
        else if (_parent != NULL)
        {
            alternationCount = _parent->_alternationCount;
        }
        else
        {
            alternationCount = GetHost()->GetAlternationCount();
        }
    }
    else
    {
        alternationCount = GetHost()->GetAlternationCount();
    }

    ChangeAlternationCount(alternationCount);
}

// ��ż�У�����У��
void ItemContainerGeneratorEx::ChangeAlternationCount(int newAlternationCount)
{
    if (_alternationCount != newAlternationCount)
    {
        ;
    }
}

void ItemContainerGeneratorEx::LinkContainerToItem(DpObject* container, Object* item)
{
    LinkContainerToItem(container, item, false);
}

void ItemContainerGeneratorEx::LinkContainerToItem(DpObject* container, Object* item, bool isRecycled)
{
    container->ClearValue(ItemForItemContainerProperty);
    container->SetValue(ItemForItemContainerProperty, item);
    if (container != item)
    {
        container->SetValue(Element::DataContextProperty, item);
    }
    else
    {
        container->ClearValue(Element::DataContextProperty);
    }
}

// �ƶ�λ��
int ItemContainerGeneratorEx::StartAt(int itemIndex)
{
    _cacheState.index = itemIndex;
    return itemIndex;
}

// ����һ������������
DpObject* ItemContainerGeneratorEx::GenerateNext()
{
    DpObject* container = NULL;
    // ��ǰ��ļ���λ������
    int itemIndex = _cacheState.index;
    Object* item = GetItem(itemIndex);
    // �Ƿ�Ϊ������
    CollectionViewGroup* group = RTTICast<CollectionViewGroup>(item);

    // ��ͨ������
    if ((group == NULL) || !IsGrouping())
    {
        // �������������������Ҫ��������
        // �����Host����ItemsControl����
        if (GetHost()->IsItemItsOwnContainer(item))
        {
            container = RTTICast<DpObject>(item);
        }
        // �л������õĶ���ֱ�ӻ�ȡ
        else if (_recyclableContainers.GetCount() > 0)
        {
            container = PopFromRecycle();
        }
        else
        {
            container = GetHost()->GetContainerForItem(item);
        }
    }
    // ������
    else
    {
        // ��ȡ�������GroupItem
        container = ContainerForGroup(group);
    }

    // �����������ɹ�
    if (container != NULL)
    {
        container->ref();
        _itemMap[itemIndex].container = container;
        //Realize(_cacheState.block, _cacheState.offset, item, container);
        //SetAlternationIndex(_cacheState.block, _cacheState.offset, _direction);
    }

    // �ƶ���һ��λ��
    _cacheState.index = itemIndex;
    if (_direction == GeneratorDirection::Forward)
    {
        _cacheState.index++;
    }
    else
    {
        _cacheState.index--;
    }

    return container;
}

void ItemContainerGeneratorEx::OnCollectionChanged(Object* sender, NotifyCollChangedEventArg* e)
{
    int index = -1;

    switch (e->GetAction())
    {
    case NotifyCollectionChangedAction::Add:
        OnItemAdded(e->GetNewItem(), e->GetNewIndex());
        return;

    case NotifyCollectionChangedAction::Remove:
        OnItemRemoved(e->GetOldItem(), e->GetOldIndex());
        return;

    case NotifyCollectionChangedAction::Replace:
        OnItemReplaced(e->GetOldItem(), e->GetNewItem(), e->GetNewIndex());
        return;

    case NotifyCollectionChangedAction::Move:
        OnItemMoved(e->GetOldItem(), e->GetOldIndex(), e->GetNewIndex());
        return;

    case NotifyCollectionChangedAction::Reset:
        OnRefresh();
        return;
    }
}

void ItemContainerGeneratorEx::OnItemAdded(Object* item, int index)
{
    _itemMap.Add(ItemEntry(NULL));
}

void ItemContainerGeneratorEx::OnItemRemoved(Object* item, int index)
{
    if (NULL != _itemMap[index].container)
    {
        _itemMap[index].container->unref();
    }
    _itemMap.RemoveAt(index);
}

void ItemContainerGeneratorEx::OnItemReplaced(Object* oldItem, Object* newItem, int newIndex)
{
    ;
}

void ItemContainerGeneratorEx::OnItemMoved(Object* oldItem, int oldIndex, int newIndex)
{
}

void ItemContainerGeneratorEx::OnRefresh()
{
    _itemMap.Clear();
}

}
