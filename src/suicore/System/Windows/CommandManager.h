// ���ڿƼ���Ȩ���� 2008-2022
// 
// �ļ�����commandmanager.h
// ��  �ܣ�������Ŀ���������
// 
// ��  �ߣ�MPF������
// ʱ  �䣺2010-07-02
// 
// ============================================================================

#ifndef _UICOMMANDMANAGER_H_
#define _UICOMMANDMANAGER_H_

#include <System/Windows/RoutedCommand.h>
#include <System/Windows/FrameworkElement.h>
#include <System/Tools/Collection.h>

namespace suic
{

class CommandBindingDic : public StringAutoDic<CommandBinding>
                        , public Object
{
public:

    CommandBindingDic()
    {

    }
};
class CommandManager
{
public:
    
    static CommandBindingDic* FindCommandBinding(RTTIOfInfo* type);
    static CommandBinding* FindMatch(RTTIOfInfo* type, const String& command);
    static CommandBinding* FindMatch(Element* elem, const String& command, FrameworkElementPtr& owner);

    static void RegisterClassCommandHandler(RTTIOfInfo* type, RoutedCommand* command, ExecutedRoutedEventHandler handler);

    static void OnExecuted(Object* sender, ExecutedRoutedEventArg* e);
    static void OnPreExecuted(Object* sender, ExecutedRoutedEventArg* e);

protected:

    static PointerAutoDic<RTTIOfInfo*, CommandBindingDic> _commandBindings;
};

};

#endif
