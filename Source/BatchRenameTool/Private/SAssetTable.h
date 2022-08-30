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
#include "Widgets/Views/STableRow.h"
#include "Widgets/Layout/SScrollBox.h"

struct FAssetTableRowData
{
    FAssetTableRowData(const FString& InFilename, const FString& InNewFilename, const FString& InPath):
        Filename(InFilename),
        NewFilename(InNewFilename),
        Path(InPath)
    {
    }

    FString Filename;
    FString NewFilename;
    FString Path;
};

class SAssetTable : public SCompoundWidget
{
public:
    /** IDs for list columns */
    static const FName ColumnID_Filename;
    static const FName ColumnID_NewFilename;
    static const FName ColumnID_AssetPath;

    SLATE_BEGIN_ARGS(SAssetTable) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, const TArray<TSharedPtr<FAssetTableRowData>>& InRows);

    void Refresh();

private:
    TSharedRef<ITableRow> GenerateRow(const TSharedPtr<FAssetTableRowData> Item, const TSharedRef<STableViewBase>& OwnerTable);

    FName SortByColumn;
    EColumnSortMode::Type SortMode = EColumnSortMode::None;
    void SortColumn(const EColumnSortPriority::Type SortPriority, const FName& ColumnId, const EColumnSortMode::Type InSortMode);
    EColumnSortMode::Type GetColumnSortMode(const FName ColumnId) const;

    TArray<TSharedPtr<FAssetTableRowData>> Rows;

    TSharedPtr<SListView<TSharedPtr<FAssetTableRowData>>> ListView;
};
