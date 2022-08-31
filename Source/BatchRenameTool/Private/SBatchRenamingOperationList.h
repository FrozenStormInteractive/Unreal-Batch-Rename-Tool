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
#include "Widgets/SCompoundWidget.h"

class FBatchRenameToolModel;
class UBatchRenamingOperation;
class UBatchRenamingOperationFactory;

class BATCHRENAMETOOL_API SBatchRenamingOperationList : public SCompoundWidget
{
public:
    struct FOperationListRowData : public FGCObject
    {
        FOperationListRowData(const TSharedPtr<FBatchRenameToolModel>& InModel, const TObjectPtr<UBatchRenamingOperation>& InOperation):
            Model(InModel),
            Operation(InOperation)
        {
        }

        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

        TSharedPtr<FBatchRenameToolModel> Model;
        TObjectPtr<UBatchRenamingOperation> Operation;
    };

    SLATE_BEGIN_ARGS(SBatchRenamingOperationList) {}
    SLATE_END_ARGS()
    void Construct(const FArguments& InArgs, const TSharedRef<FBatchRenameToolModel>& Model);

private:
    TSharedRef<SWidget> MakeOperationsMenuWidget();

    void RefreshList();

    TSharedPtr<FBatchRenameToolModel> Model;
    TSharedPtr<SSearchBox> FilterTextBoxWidget;

    TSharedPtr<SListView<TSharedPtr<FOperationListRowData>>> ListView;
    TArray<TSharedPtr<FOperationListRowData>> Rows;
    TSharedRef<ITableRow> GenerateRow(const TSharedPtr<FOperationListRowData> Item, const TSharedRef<STableViewBase>& OwnerTable);
    void OnListViewSelectionChanged(TSharedPtr<FOperationListRowData> ItemSelected, ESelectInfo::Type SelectInfo);
};
