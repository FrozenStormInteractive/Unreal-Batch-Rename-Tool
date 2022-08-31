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

#include "BatchRenameToolModule.h"
#include "BatchRenameToolStyle.h"
#include "BatchRenameToolCommands.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ContentBrowserModule.h"
#include "BatchRenameEditorToolkit.h"

#define LOCTEXT_NAMESPACE "BatchRenameTool"

void FBatchRenameToolModule::StartupModule()
{
    FBatchRenameToolStyle::Initialize();
    FBatchRenameToolStyle::ReloadTextures();

    FBatchRenameToolCommands::Register();

    Commands = MakeShareable(new FUICommandList);

    Commands->MapAction(
        FBatchRenameToolCommands::Get().BatchRename,
        FExecuteAction::CreateRaw(this, &FBatchRenameToolModule::OpenBulkRenameWindow),
        FCanExecuteAction());

    {
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
        TArray<FContentBrowserMenuExtender_SelectedAssets>& ContentBrowserAssetMenuExtenderDelegates = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
        ContentBrowserAssetMenuExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FBatchRenameToolModule::OnExtendContentBrowserAssetContextMenu));
        ContentBrowserAssetExtenderDelegateHandle = ContentBrowserAssetMenuExtenderDelegates.Last().GetHandle();
    }

}

void FBatchRenameToolModule::ShutdownModule()
{
    FContentBrowserModule* ContentBrowserModule = FModuleManager::GetModulePtr<FContentBrowserModule>(TEXT("ContentBrowser"));
    if (ContentBrowserModule)
    {
        TArray<FContentBrowserMenuExtender_SelectedAssets>& ContentBrowserMenuExtenderDelegates = ContentBrowserModule->GetAllAssetViewContextMenuExtenders();
        ContentBrowserMenuExtenderDelegates.RemoveAll([this](const FContentBrowserMenuExtender_SelectedAssets& Delegate) { return Delegate.GetHandle() == ContentBrowserAssetExtenderDelegateHandle; });
    }

    FBatchRenameToolStyle::Shutdown();

    FBatchRenameToolCommands::Unregister();
}

void FBatchRenameToolModule::OpenBulkRenameWindow()
{
    TArray<TObjectPtr<UObject>> AssetsToEdit;

    for(const FAssetData& AssetData: SelectedAssets)
    {
        if(!AssetData.IsRedirector())
        {
            AssetsToEdit.Add(AssetData.GetAsset());
        }
    }

    if (AssetsToEdit.Num() > 0)
    {
        FBatchRenameEditorToolkit::CreateEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), AssetsToEdit);
    }
}

TSharedRef<FExtender> FBatchRenameToolModule::OnExtendContentBrowserAssetContextMenu(const TArray<FAssetData>& Assets)
{
    SelectedAssets = Assets;

    const TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
    MenuExtender->AddMenuExtension("AssetContextAdvancedActions", EExtensionHook::After, Commands,
        FMenuExtensionDelegate::CreateRaw(this, &FBatchRenameToolModule::AddContentBrowserMenuExtension));
    return MenuExtender.ToSharedRef();
}

void FBatchRenameToolModule::AddContentBrowserMenuExtension(FMenuBuilder& MenuBuilder)
{
    if (SelectedAssets.Num() > 1)
    {
        MenuBuilder.AddMenuEntry(FBatchRenameToolCommands::Get().BatchRename, NAME_None, LOCTEXT("BatchRename", "Batch Rename"), LOCTEXT("BatchRename_Tooltip", "Rename the selected assets."), FSlateIcon(FEditorStyle::GetStyleSetName(), "GenericCommands.Rename"));
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBatchRenameToolModule, BatchRenameTool)
