/////////////////////////////////////////////////////
// UnzipWindow.cpp

#include "stdafx.h"
#include "UnzipWindow.h"

ImplementRTTIOfClass(UnzipData, suic::NotifyPropChanged)

UnzipWindow::UnzipWindow()
{
    _unzipData = NULL;
    _driverGrp = new DriverGroup();
    _driverGrp->ref();
}

UnzipWindow::~UnzipWindow()
{
    if (NULL != _unzipData)
    {
        _unzipData->unref();
    }
    _driverGrp->unref();
}

void UnzipWindow::UpdateWindowTitle()
{
    SetTitle(suic::String().Format(_U("��ѹ�ļ���%s"), _unzipData->GetUnzipFile().c_str()));
}

void UnzipWindow::OnLoaded(suic::LoadedEventArg* e)
{
    suic::Window::OnLoaded(e);

    CenterWindow();

    _driverGrp->InitRootItems();

    _unzipData = new UnzipData();
    _unzipData->ref();

    _unzipData->SetUnzipProgress(20);
    _unzipData->SetUnzipFile("c:/test.tar");

    UpdateWindowTitle();

    SetDataContext(_unzipData);
}

bool UnzipWindow::OnBuild(suic::IXamlNode* pNode, suic::ObjectPtr& obj)
{
    return false;
}

void UnzipWindow::OnConnect(suic::IXamlNode* pNode, suic::Object* target)
{
}

void UnzipWindow::OnClickChangeDir(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // �������Ŀ¼��ť������Ӧҵ����

    e->SetHandled(true);

    _unzipData->SetUnzipDir(_selectedDir);

    /*
    suic::DirBrowser db;
    if (db.Open())
    {
        _zipData->SetZipFile(db.GetDir());
        UpdateWindowTitle();
    }
    */
}

void UnzipWindow::OnClickUnzip(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // ���������ѹ��ť������Ӧҵ����

    e->SetHandled(true);
    _unzipData->DoUnzip();

    //InvalidateDataProp(_unzipData);
}

void UnzipWindow::OnClickPause(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // �����ͣ��ť������Ӧҵ����

    e->SetHandled(true);
}

void UnzipWindow::OnClickCancel(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    // ���ȡ����ť������Ӧҵ����

    e->SetHandled(true);
    AsyncClose();
}

void UnzipWindow::OnCheckedDown(suic::DpObject* sender, suic::RoutedEventArg* e)
{
    e->SetHandled(true);

    DriverItem* pItem = NULL;
    suic::TreeView* pTreeView = NULL;
    suic::Popup* pPopup = NULL;
    suic::ToggleButton* pTB = NULL;
    
    pPopup = new suic::Popup();
    pPopup->ref();

    pTB = suic::DynamicCast<suic::ToggleButton>(sender);

    pTreeView = new suic::TreeView();
    pTreeView->SetWidth(pTB->GetParent()->GetParent()->GetActualWidth());
    pTreeView->SetHeight(160);

    pTreeView->SetItemsSource(_driverGrp);
  
    //suic::InfoBox::Show(_U("����ѹ���ļ����Ƿ�ȷ���˳� ��"), _U("��ʾ"), suic::InfoBox::IB_YesNo);

    pPopup->SetPlacementTarget(pTB->GetParent()->GetParent());
    pPopup->SetPlacement(suic::PlacementMode::pmBottom);

    pPopup->SetChild(pTreeView);
    pPopup->TrackingPopup();

    pItem = suic::DynamicCast<DriverItem>(pTreeView->GetSelectedItem());

    if (NULL != pItem)
    {
        _selectedDir = pItem->GetFilePath();
    }

    pTB->SetChecked(suic::Boolean::False);
    pPopup->unref();
}
