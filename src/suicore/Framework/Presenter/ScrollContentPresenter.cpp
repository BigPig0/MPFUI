
#include <Framework/Controls/ScrollViewer.h>
#include <Framework/Presenter/ItemsPresenter.h>
#include <Framework/Presenter/ScrollContentPresenter.h>

namespace suic
{

ImplementRTTIOfClass(ScrollContentPresenter, ContentPresenter)

ScrollContentPresenter::ScrollContentPresenter()
    : _scrollInfo(NULL)
    , _scrollClient(false)
{
    _adornerLayer = new AdornerLayer();
    _adornerLayer->ref();
}

ScrollContentPresenter::~ScrollContentPresenter()
{
    if (_scrollInfo && _scrollClient)
    {
        delete _scrollInfo;
    }
    _adornerLayer->unref();
}

int ScrollContentPresenter::GetVisualChildrenCount()
{
    if (GetTemplateChild() != NULL)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

Element* ScrollContentPresenter::GetVisualChild(int index)
{
    switch (index)
    {
    case 0:
        return ContentPresenter::GetTemplateChild();

    case 1:
        return _adornerLayer;

    default:

        return NULL;
    }
}

bool ScrollContentPresenter::CanContentScroll()
{
    return true;
}

IScrollInfo* ScrollContentPresenter::GetScrollInfo()
{
    if (_scrollClient)
    {
        return _scrollInfo;
    }
    else
    {
        return NULL;
    }
}

ScrollData* ScrollContentPresenter::GetScrollData()
{
    ScrollInfo* info = dynamic_cast<ScrollInfo*>(_scrollInfo);
    if (NULL == info)
    {
        return NULL;
    }
    else
    {
        return info->GetScrollData();
    }
}

void ScrollContentPresenter::EnsureScrollInfo()
{
    if (!_scrollInfo)
    {
        _scrollInfo = new ScrollInfo();
        ((ScrollInfo*)_scrollInfo)->SetOwner(this);
    }
}

void ScrollContentPresenter::OnApplyTemplate()
{
}

void ScrollContentPresenter::LinkScrollInfo()
{
    if (_scrollClient && _scrollInfo)
    {
        delete _scrollInfo;
        _scrollInfo = NULL;
    }

    // ȡ��ģ�常Ԫ��
    // ��Ԫ����ΪScrollViewer��ControlTemplateһ���ִ���
    ScrollViewer* templatedParent = DynamicCast<ScrollViewer>(GetTemplatedParent());

    if (templatedParent != NULL)
    {
        FrameworkElement* fe = NULL;
        IScrollInfo* content = NULL;

        // �Ƿ�֧�ֹ�������ֵ��ScrollViewer�õ���
        if (CanContentScroll())
        {
            // ʵ�ֽӿ�֧��IScrollInfo�ӿ�
            content = DynamicCast<IScrollInfo>(GetContent());
            fe = DynamicCast<FrameworkElement>(GetContent());

            if (content == NULL && fe != NULL)
            {
                // ͨ�����ScrollInfoʵ��IScrollInfo
                content = fe->GetScrollInfo();

                // ����֧�ֹ���
                if (content == NULL)
                {
                    ItemsPresenter* reference = DynamicCast<ItemsPresenter>(GetContent());

                    if (reference == NULL)
                    {
                        FrameworkElement* element = DynamicCast<FrameworkElement>(templatedParent->GetTemplatedParent());
                        if (element != NULL)
                        {
                            reference = DynamicCast<ItemsPresenter>(element->GetTemplateChild(_U("ItemsPresenter")));
                        }
                    }

                    if (reference != NULL)
                    {
                        FrameworkElement* itemPanel = NULL;
                        // Ӧ��ģ��
                        // ItemsPresenterֻ��һ����Ԫ�أ����ұ����Panel�̳�
                        reference->ApplyTemplate();
                        itemPanel = reference->GetTemplateChild();

                        if (itemPanel != NULL)
                        {
                            content = DynamicCast<IScrollInfo>(itemPanel);
                            if (!content)
                            {
                                content = itemPanel->GetScrollInfo();
                            }
                        }
                    }
                    else
                    {
                        fe = templatedParent->FindElem<FrameworkElement>(_U("PART_ScrollHost"));
                        content = DynamicCast<IScrollInfo>(fe);
                        if (content)
                        {
                            content = fe->GetScrollInfo();
                        }
                    }
                }
            }
        }

        // û���ҵ�IScrollInfo�ӿ�
        // ��ʹ������ʵ�ֵ�IScrollInfo
        if (content == NULL)
        {
            _scrollClient = true;
            EnsureScrollInfo();
        }
        else
        {
            // �ҵ�������ָ��
            _scrollClient = false;
            _scrollInfo = content;
        }

        // ����ScrollViewer
        _scrollInfo->SetScrollOwner(templatedParent);
        templatedParent->SetScrollInfo(_scrollInfo);
    }
    else if (_scrollInfo != NULL)
    {
        if (_scrollInfo->GetScrollOwner() != NULL)
        {
            _scrollInfo->GetScrollOwner()->SetScrollInfo(NULL);
        }

        _scrollInfo->SetScrollOwner(NULL);

        // ����Ǳ���ʵ�֣�ɾ��
        if (_scrollClient)
        {
            delete _scrollInfo;
        }

        _scrollInfo = NULL;
    }

    //MessageBox(NULL, _T("1"), _T("tt"), MB_OK);
}

Size ScrollContentPresenter::OnMeasure(const Size& constraint)
{
    Size size;
    int count = GetVisualChildrenCount();
    ScrollData* scrollData = GetScrollData();

    if (NULL == scrollData)
    {
        LinkScrollInfo();
        scrollData = GetScrollData();
    }

    if (count > 0)
    {
        _adornerLayer->Measure(constraint);

        if (!IsScrollClient())
        {
            size = ContentPresenter::OnMeasure(constraint);
        }
        else
        {
            Size measureSize = constraint;

            if (_scrollInfo->CanHorizontalScroll())
            {
                measureSize.cx = suic::Numeric::MeasureInt;
            }
            if (_scrollInfo->CanVerticalScroll())
            {
                measureSize.cy = suic::Numeric::MeasureInt;
            }

            size = ContentPresenter::OnMeasure(measureSize);
        }
    }

    if (IsScrollClient())
    {
        VerifyScrollData(constraint, size);
    }

    size.cx = min(constraint.cx, size.cx);
    size.cy = min(constraint.cy, size.cy);

    return size;
}

void ScrollContentPresenter::OnArrange(const Size& arrangeSize)
{
    int count = GetVisualChildrenCount();
    ScrollData* scrollData = GetScrollData();

    if (IsScrollClient())
    {
        VerifyScrollData(arrangeSize, scrollData->extent);
    }

    if (count <= 0)
    {
        return;
    }

    _adornerLayer->Arrange(Rect(Point(), arrangeSize));

    Element* child = GetVisualChild(0);
    if (child != NULL)
    {
        int x = 0;
        int y = 0;
        int w = max(child->GetDesiredSize().Width(), arrangeSize.Width());
        int h = max(child->GetDesiredSize().Height(), arrangeSize.Height());
        
        if (IsScrollClient())
        {
            x = -scrollData->computedOffset.x;
            y = -scrollData->computedOffset.y;
        }

        Rect finalRect(x, y, w, h);
        child->Arrange(finalRect);
    }
}

FrameworkElement* ScrollContentPresenter::GetTemplateChild()
{
    return ContentPresenter::GetTemplateChild();
}

void ScrollContentPresenter::SetTemplateChild(FrameworkElement* elem)
{
    FrameworkElement* templateChild = ContentPresenter::GetTemplateChild();
    
    if (elem != templateChild)
    {
        if ((templateChild != NULL) && (elem == NULL))
        {
            ContentPresenter::RemoveVisualChild(_adornerLayer);
        }

        ContentPresenter::SetTemplateChild(elem);

        if ((templateChild == NULL) && (elem != NULL))
        {
            ContentPresenter::AddVisualChild(_adornerLayer);
        }
    }
}

void ScrollContentPresenter::VerifyScrollData(Size viewport, Size extent)
{
    int flag = 0;
    if (viewport.cx < 0)
    {
        viewport.cx = extent.cx;
    }
    if (viewport.cy < 0)
    {
        viewport.cy = extent.cy;
    }

    ScrollData* scrollData = GetScrollData();

    if (viewport != scrollData->viewport)
    {
        flag++;
    }
    if (extent != scrollData->extent)
    {
        flag++;
    }

    Point offset = scrollData->offset;

    offset.x = max(0.0, min(offset.x, extent.Width() - viewport.Width()));
    offset.y = max(0.0, min(offset.y, extent.Height() - viewport.Height()));

    //scrollData->offset = offset;
    scrollData->viewport = viewport;
    scrollData->extent = extent;
    scrollData->SetComputedOffset(offset);

    if (flag > 0)
    {
        GetScrollInfo()->GetScrollOwner()->InvalidateScrollInfo(true);
    }
}

}
