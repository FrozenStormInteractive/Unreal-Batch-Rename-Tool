/*
 * MIT License
 *
 * Copyright (c) 2022 Frozen Storm Interactive, Yoann Potinet
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "BatchRenameEditorToolkit.h"

#include "AssetToolsModule.h"
#include "EditorStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "SBatchRenamingOperationDetails.h"
#include "SBatchRenamingOperationList.h"
#include "IAssetTools.h"
#include "BatchRenameToolModel.h"
#include "BatchRenameToolModule.h"
#include "SAssetTable.h"
#include "ToolMenus.h"
#include "Subsystems/AssetEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "BatchRenameTool"

class FAssetToolsModule;

const FName FBatchRenameEditorToolkit::ToolkitFName(TEXT("BatchRenameEditor"));
const FName FBatchRenameEditorToolkit::ApplicationId(TEXT("BatchRenameEditorToolkitApp"));
const FName FBatchRenameEditorToolkit::AssetTableTabId(TEXT("BatchRenameEditorToolkit_AssetTable"));
const FName FBatchRenameEditorToolkit::MethodListTabId(TEXT("BatchRenameEditorToolkit_MethodList"));
const FName FBatchRenameEditorToolkit::OperationDetailsTabId(TEXT("BatchRenameEditorToolkit_OperationDetails"));

FBatchRenameEditorToolkit::FBatchRenameEditorToolkit():
    Model(MakeShared<FBatchRenameToolModel>())
{
}

void FBatchRenameEditorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    InTabManager->RegisterTabSpawner(AssetTableTabId, FOnSpawnTab::CreateSP(this, &FBatchRenameEditorToolkit::SpawnTab_AssetTable))
        .SetDisplayName(LOCTEXT("AssetTableTab", "Assets"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "PropertyEditor.Grid.TabIcon"));

    InTabManager->RegisterTabSpawner(MethodListTabId, FOnSpawnTab::CreateSP(this, &FBatchRenameEditorToolkit::SpawnTab_OperationList))
        .SetDisplayName(LOCTEXT("OperationListTab", "Operations"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Details"));

    InTabManager->RegisterTabSpawner(OperationDetailsTabId, FOnSpawnTab::CreateSP(this, &FBatchRenameEditorToolkit::SpawnTab_OperationDetails))
        .SetDisplayName(LOCTEXT("OperationDetailsTab", "Operation Details"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Details"));
}

void FBatchRenameEditorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
    InTabManager->UnregisterTabSpawner(AssetTableTabId);
    InTabManager->UnregisterTabSpawner(MethodListTabId);
    InTabManager->UnregisterTabSpawner(OperationDetailsTabId);
}

FName FBatchRenameEditorToolkit::GetToolkitFName() const
{
    return ToolkitFName;
}

FText FBatchRenameEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("AppLabel", "Bulk Renamer");
}

FText FBatchRenameEditorToolkit::GetToolkitName() const
{
    const TArray<UObject*>& EditingObjs = GetEditingObjects();

    const int32 NumEditingObjects = EditingObjs.Num();

    check(NumEditingObjects > 0);

    if (NumEditingObjects == 1)
    {
        const UObject* EditingObject = EditingObjs[0];


        FFormatNamedArguments Args;
        Args.Add(TEXT("ObjectName"), FText::FromString(EditingObject->GetName()));
        Args.Add(TEXT("ToolkitName"), GetBaseToolkitName());
        return FText::Format(LOCTEXT("ToolkitName_SingleObject", "{ObjectName} - {ToolkitName}"), Args);
    }
    else
    {
        const UClass* SharedBaseClass = nullptr;
        for (int32 x = 0; x < NumEditingObjects; ++x)
        {
            UObject* Obj = EditingObjs[x];
            check(Obj);

            const UClass* ObjClass = Cast<UClass>(Obj);
            if (ObjClass == nullptr)
            {
                ObjClass = Obj->GetClass();
            }
            check(ObjClass);

            // Initialize with the class of the first object we encounter.
            if (SharedBaseClass == nullptr)
            {
                SharedBaseClass = ObjClass;
            }

            // If we've encountered an object that's not a subclass of the current best baseclass,
            // climb up a step in the class hierarchy.
            while (!ObjClass->IsChildOf(SharedBaseClass))
            {
                SharedBaseClass = SharedBaseClass->GetSuperClass();
            }
        }

        FFormatNamedArguments Args;
        Args.Add(TEXT("NumberOfObjects"), EditingObjs.Num());
        Args.Add(TEXT("ClassName"), FText::FromString(SharedBaseClass->GetName()));
        return FText::Format(LOCTEXT("ToolkitName_MultiObject", "{NumberOfObjects} {ClassName} Objects - Bulk Renamer"), Args);
    }
}

FText FBatchRenameEditorToolkit::GetToolkitToolTipText() const
{
    const TArray<UObject*>& EditingObjs = GetEditingObjects();

    const int32 NumEditingObjects = EditingObjs.Num();

    check(NumEditingObjects > 0);

    if (NumEditingObjects == 1)
    {
        const UObject* EditingObject = EditingObjs[0];
        return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
    }
    else
    {
        const UClass* SharedBaseClass = nullptr;
        for (int32 x = 0; x < NumEditingObjects; ++x)
        {
            UObject* Obj = EditingObjs[x];
            check(Obj);

            const UClass* ObjClass = Cast<UClass>(Obj);
            if (ObjClass == nullptr)
            {
                ObjClass = Obj->GetClass();
            }
            check(ObjClass);

            // Initialize with the class of the first object we encounter.
            if (SharedBaseClass == nullptr)
            {
                SharedBaseClass = ObjClass;
            }

            // If we've encountered an object that's not a subclass of the current best baseclass,
            // climb up a step in the class hierarchy.
            while (!ObjClass->IsChildOf(SharedBaseClass))
            {
                SharedBaseClass = SharedBaseClass->GetSuperClass();
            }
        }

        FFormatNamedArguments Args;
        Args.Add(TEXT("NumberOfObjects"), NumEditingObjects);
        Args.Add(TEXT("ClassName"), FText::FromString(SharedBaseClass->GetName()));
        return FText::Format(LOCTEXT("ToolkitName_MultiObjectToolTip", "{NumberOfObjects} {ClassName} Objects - Bulk Renamer"), Args);
    }
}

FLinearColor FBatchRenameEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor();
}

FString FBatchRenameEditorToolkit::GetWorldCentricTabPrefix() const
{
    return TEXT("");
}

TSharedRef<SDockTab> FBatchRenameEditorToolkit::SpawnTab_AssetTable(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == AssetTableTabId);

    return SNew(SDockTab)
        .TabColorScale(GetTabColorScale())
        .Content()
        [
            SNew(SBorder)
            .Padding(FMargin(3))
            .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            [
                SAssignNew(AssetTable, SAssetTable, AssetTableRows)
            ]
        ];
}

TSharedRef<SDockTab> FBatchRenameEditorToolkit::SpawnTab_OperationList(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == MethodListTabId);

    return SNew(SDockTab)
        .TabColorScale(GetTabColorScale())
        .Content()
        [
            SAssignNew(OperationList, SBatchRenamingOperationList, Model)
        ];
}

TSharedRef<SDockTab> FBatchRenameEditorToolkit::SpawnTab_OperationDetails(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == OperationDetailsTabId);

    return SNew(SDockTab)
        .TabColorScale(GetTabColorScale())
        .Content()
        [
            SNew(SBatchRenamingOperationDetails, Model)
        ];
}

void FBatchRenameEditorToolkit::Initialize(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<TObjectPtr<UObject>>& ObjectsToEdit)
{
    Model->OnOperationListModified().AddSP(this, &FBatchRenameEditorToolkit::OnOperationsChanged);

    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_BatchRenameEditorToolkit_Layout")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)
            ->Split
            (
                FTabManager::NewStack()
                ->SetSizeCoefficient(0.8f)
                ->SetHideTabWell(true)
                ->AddTab(AssetTableTabId, ETabState::OpenedTab)
            )
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetOrientation(Orient_Vertical)
                ->SetSizeCoefficient(0.2f)
                ->Split
                (
                FTabManager::NewStack()
                    ->SetSizeCoefficient(0.7f)
                    ->SetHideTabWell(true)
                    ->AddTab(MethodListTabId, ETabState::OpenedTab)
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.3f)
                    ->SetHideTabWell(true)
                    ->AddTab(OperationDetailsTabId, ETabState::OpenedTab)
                )
            )
        );

    for(const TObjectPtr<UObject> Asset: ObjectsToEdit)
    {
        TSharedPtr<FAssetTableRowData> RowData = MakeShared<FAssetTableRowData>(Asset->GetName(), Asset->GetName(), Asset->GetPathName());
        AssetTableRows.Add(RowData);
    }


    constexpr bool bCreateDefaultStandaloneMenu = true;
    constexpr bool bCreateDefaultToolbar = true;
    FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, ApplicationId, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit);

    ExtendToolbar();
    RegisterToolBar();
}

void FBatchRenameEditorToolkit::ExtendToolbar()
{
    FBatchRenameToolModule* BatchRenameToolModule = &FModuleManager::LoadModuleChecked<FBatchRenameToolModule>("BatchRenameTool");
    AddToolbarExtender(BatchRenameToolModule->GetBatchRenameEditorToolBarExtensibilityManager()->GetAllExtenders());
}

void FBatchRenameEditorToolkit::RegisterToolBar()
{
    UToolMenus* ToolMenus = UToolMenus::Get();
    FName ParentName;
    const FName MenuName = GetToolMenuToolbarName(ParentName);
    UToolMenu* ToolBarBuilder;
    if (ToolMenus->IsMenuRegistered(MenuName))
    {
        ToolBarBuilder = ToolMenus->ExtendMenu(MenuName);
    }
    else
    {
        ToolBarBuilder = ToolMenus->RegisterMenu(MenuName, ParentName, EMultiBoxType::ToolBar);
    }

    {
        FToolMenuSection& Section = ToolBarBuilder->AddSection("BatchRename");
        FToolMenuEntry RunToolbarButton = FToolMenuEntry::InitToolBarButton(
            "ExecuteAction",
            FUIAction(FExecuteAction::CreateSP(this, &FBatchRenameEditorToolkit::Run)),
            LOCTEXT("Execute", "Execute"),
            TAttribute<FText>(),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Play"));
        Section.AddEntry(RunToolbarButton);
    }
}

TSharedPtr<FBatchRenameEditorToolkit> FBatchRenameEditorToolkit::FindExistingEditor(TObjectPtr<UObject> Object)
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    const TArray<IAssetEditorInstance*> Editors = AssetEditorSubsystem->FindEditorsForAsset(Object);

    IAssetEditorInstance* const * ExistingInstance = Editors.FindByPredicate([&](IAssetEditorInstance* Editor) {
        return Editor->GetEditorName() == ToolkitFName;
    });

    if (ExistingInstance)
    {
        FBatchRenameEditorToolkit* PropertyEditor = StaticCast<FBatchRenameEditorToolkit*>(*ExistingInstance);
        return PropertyEditor->SharedThis(PropertyEditor);
    }

    return nullptr;
}

TSharedRef<FBatchRenameEditorToolkit> FBatchRenameEditorToolkit::CreateEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, TObjectPtr<UObject> ObjectToEdit)
{
    const TSharedPtr<FBatchRenameEditorToolkit> ExistingEditor = FindExistingEditor(ObjectToEdit);
    if (ExistingEditor.IsValid())
    {
        ExistingEditor->FocusWindow();
        return ExistingEditor.ToSharedRef();
    }

    TSharedRef<FBatchRenameEditorToolkit> NewEditor(new FBatchRenameEditorToolkit());

    TArray<TObjectPtr<UObject>> ObjectsToEdit;
    ObjectsToEdit.Add(ObjectToEdit);
    NewEditor->Initialize(Mode, InitToolkitHost, ObjectsToEdit);

    return NewEditor;
}


TSharedRef<FBatchRenameEditorToolkit> FBatchRenameEditorToolkit::CreateEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<TObjectPtr<UObject>>& ObjectsToEdit)
{
    if (ObjectsToEdit.Num() == 1)
    {
        const TSharedPtr<FBatchRenameEditorToolkit> ExistingEditor = FindExistingEditor(ObjectsToEdit[0]);
        if (ExistingEditor.IsValid())
        {
            ExistingEditor->FocusWindow();
            return ExistingEditor.ToSharedRef();
        }
    }

    TSharedRef<FBatchRenameEditorToolkit> NewEditor(new FBatchRenameEditorToolkit());
    NewEditor->Initialize(Mode, InitToolkitHost, ObjectsToEdit);

    return NewEditor;
}

void FBatchRenameEditorToolkit::Run()
{
    if(!Model->GetOperations().IsEmpty())
    {
        const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
        TArray<FAssetRenameData> AssetRenameData;

        for (UObject* Asset : GetEditingObjects())
        {
            const FString PackagePath = FPackageName::GetLongPackagePath(Asset->GetOutermost()->GetName());
            AssetRenameData.Add(FAssetRenameData(Asset, PackagePath, ApplyOperations(Asset->GetName())));
        }

        if(AssetToolsModule.Get().RenameAssets(AssetRenameData))
        {
            CloseWindow(EAssetEditorCloseReason::AssetEditorHostClosed);
        }
    }
}

FString FBatchRenameEditorToolkit::ApplyOperations(const FString& Str) const
{
    FString StrCopy = Str;
    for(const TObjectPtr<UBatchRenamingOperation>& Rule: Model->GetOperations())
    {
        StrCopy = Rule->Apply(StrCopy);
    }
    return StrCopy;
}

void FBatchRenameEditorToolkit::OnOperationsChanged()
{
    for (const TSharedPtr<FAssetTableRowData>& Asset : AssetTableRows)
    {
        Asset->NewFilename = ApplyOperations(Asset->Filename);
    }
    AssetTable->Refresh();
}

#undef LOCTEXT_NAMESPACE
