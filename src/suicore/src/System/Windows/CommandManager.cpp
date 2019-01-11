// ======================================================================
//
// Copyright (c) 2008-2022 ����(�ӵ�UI), Inc. All rights reserved.
//
// MPF�������ѭָ����ԴЭ�飬�����˾���������Э�鹺����Ȩ��
// �κθ��ˡ������˾���ܾ��ڴ˿������κ���ҵ���ʵĿ����롣
//
// ======================================================================

/////////////////////////////////////////////////////////////////////////
// commandmanager.cpp

#include <System/Windows/CommandManager.h>
#include <Framework/Controls/Control.h>

namespace suic
{

PointerAutoDic<RTTIOfInfo*, CommandBindingDic> CommandManager::_commandBindings;

void CommandManager::
RegisterClassCommandHandler(RTTIOfInfo* type, RoutedCommand* command
                            , ExecutedRoutedEventHandler handler)
{
    CommandBinding* cmdBind(new CommandBinding(command, handler));
    CommandBindingDic* cmdColl(FindCommandBinding(type));

    if (!cmdColl)
    {
        cmdColl = new CommandBindingDic();
        _commandBindings.Add(type, cmdColl);
    }

    cmdColl->Add(command->GetName(), cmdBind);
}

CommandBindingDic* CommandManager::FindCommandBinding(RTTIOfInfo* type)
{
    CommandBindingDic* cmdBindDic = NULL;
    RTTIOfInfo* classType = type;

    while (classType != NULL)
    { 
        if (_commandBindings.TryGetValue(classType, cmdBindDic)) 
        {
            break;
        }

        classType = (*(classType->baseType))();
    }

    return cmdBindDic;
}

CommandBinding* CommandManager::FindMatch(RTTIOfInfo* type, const String& command)
{
    CommandBinding* cmdBind = NULL;
    RTTIOfInfo* classType = type;

    while (classType != NULL)
    {
        CommandBindingDic* cmdBindDic = NULL;

        if (_commandBindings.TryGetValue(classType, cmdBindDic)) 
        {
            if (cmdBindDic->TryGetValue(command, cmdBind))
            {
                break;
            }
        }

        classType = (*(classType->baseType))();
    }

    return cmdBind;
}

CommandBinding* CommandManager::FindMatch(Element* elem, const String& command, FrameworkElementPtr& owner)
{
    CommandBinding* cmdBind = NULL;
    Element* temp = elem;

    while (temp)
    {
        CommandBindingDic* cmdBindDic = NULL;
        RTTIOfInfo* classType = temp->GetRTTIType();

        if (_commandBindings.TryGetValue(classType, cmdBindDic)) 
        {
            if (cmdBindDic->TryGetValue(command, cmdBind))
            {
                owner = temp;
                break;
            }
        }

        temp = temp->GetUIParent();
    }

    return cmdBind;
}

void CommandManager::OnPreExecuted(Object* sender, ExecutedRoutedEventArg* e)
{
}

void CommandManager::OnExecuted(Object* sender, ExecutedRoutedEventArg* e)
{
    RoutedCommand* command(dynamic_cast<RoutedCommand*>(e->GetCommand()));
    RTTIOfInfo* classType = sender->GetRTTIType();

    if (command)
    {
        CommandBinding* cmdBind = NULL;
        CommandBindingDic* cmdBindDic = NULL;

        while (classType != NULL)
        { 
            if (_commandBindings.TryGetValue(classType, cmdBindDic))
            {
                if (cmdBindDic->TryGetValue(command->GetName(), cmdBind))
                {
                    (cmdBind->GetCommandExecutedHandler())(sender, e);
                    if (e->IsHandled())
                    {
                        break;
                    }
                }
            }

            classType = (*(classType->baseType))();
        }
    }
}

}
