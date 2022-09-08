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

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Layout/SScrollBox.h"

class UBatchRenamingOperationFactory;
class SBatchRenamingOperationList;
class SAssetTable;
struct FAssetTableRowData;
class FBatchRenameToolModel;

class FBatchRenameEditorToolkit : public FAssetEditorToolkit
{
public:
    FBatchRenameEditorToolkit();

    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

    virtual FName GetToolkitFName() const override;

    virtual FText GetBaseToolkitName() const override;

    virtual FText GetToolkitName() const override;

    virtual FText GetToolkitToolTipText() const override;

    FLinearColor GetWorldCentricTabColorScale() const override;

    FString GetWorldCentricTabPrefix() const override;

    virtual bool IsPrimaryEditor() const override { return false; };

    static TSharedRef<FBatchRenameEditorToolkit> CreateEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, TObjectPtr<UObject> ObjectToEdit);
    static TSharedRef<FBatchRenameEditorToolkit> CreateEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<TObjectPtr<UObject>>& ObjectsToEdit);

private:
    FReply Run();
    FString ApplyOperations(const FString& Str) const;
    void OnOperationsChanged();

    static TSharedPtr<FBatchRenameEditorToolkit> FindExistingEditor(TObjectPtr<UObject> Object);

    void Initialize(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<TObjectPtr<UObject>>& ObjectsToEdit);

    TSharedRef<SDockTab> SpawnTab_AssetTable(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_OperationList(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnTab_OperationDetails(const FSpawnTabArgs& Args);

    static const FName ToolkitFName;
    static const FName ApplicationId;
    static const FName AssetTableTabId;
    static const FName MethodListTabId;
    static const FName OperationDetailsTabId;

    TSharedRef<FBatchRenameToolModel> Model;

    TSharedPtr<SBatchRenamingOperationList> OperationList;

    TSharedPtr<SAssetTable> AssetTable;
    TArray<TSharedPtr<FAssetTableRowData>> AssetTableRows;
};
