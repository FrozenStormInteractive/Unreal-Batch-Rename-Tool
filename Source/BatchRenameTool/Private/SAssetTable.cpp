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

#include "SAssetTable.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Views/SListView.h"

const FName SAssetTable::ColumnID_Filename = "Filename";
const FName SAssetTable::ColumnID_NewFilename = "NewFilename";
const FName SAssetTable::ColumnID_AssetPath = "AssetPath";

class SAssetTableRow : public SMultiColumnTableRow<TSharedPtr<FAssetTableRowData>>
{
public:
    SLATE_BEGIN_ARGS(SAssetTableRow) {}
    SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, TSharedRef<FAssetTableRowData> InLine, TSharedRef<STableViewBase> InOwnerTableView);

    virtual TSharedRef< SWidget > GenerateWidgetForColumn(const FName& ColumnID) override;

private:
    TSharedPtr<FAssetTableRowData> Data;
};

void SAssetTableRow::Construct(const FArguments& InArgs, TSharedRef<FAssetTableRowData> InData,
    TSharedRef<STableViewBase> InOwnerTableView)
{
    Data = InData;

    SMultiColumnTableRow<TSharedPtr<FAssetTableRowData>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SAssetTableRow::GenerateWidgetForColumn(const FName& ColumnID)
{
    TSharedPtr< SWidget > TableRowContent;

    static const FTextBlockStyle TextStyle = FTextBlockStyle(FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText")).SetFontSize(10);

    if (ColumnID == SAssetTable::ColumnID_Filename)
    {
        SAssignNew(TableRowContent, SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(2.0f)
            .FillWidth(100.0f)
            [
                SNew(SMultiLineEditableText)
                .Text(FText::FromString(Data->Filename))
                .TextStyle(&TextStyle)
                .IsReadOnly(true)
            ];
    }
    else if (ColumnID == SAssetTable::ColumnID_NewFilename)
    {
        SAssignNew(TableRowContent, SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(2.0f)
            .FillWidth(400.0f)
            [
                SNew(SMultiLineEditableText)
                .Text(FText::FromString(Data->NewFilename))
                .TextStyle(&TextStyle)
                .IsReadOnly(true)
                .AutoWrapText(true)
            ];
    }
    else if (ColumnID == SAssetTable::ColumnID_AssetPath)
    {
        SAssignNew(TableRowContent, SHorizontalBox)
            + SHorizontalBox::Slot()
            .Padding(2.0f)
            .FillWidth(400.0f)
            [
                SNew(SMultiLineEditableText)
                .Text(FText::FromString(Data->Path))
                .TextStyle(&TextStyle)
                .IsReadOnly(true)
                .AutoWrapText(true)
            ];
    }
    else
    {
        checkf(false, TEXT("Unknown ColumnID provided to SAssetTable"));
    }

    return TableRowContent.ToSharedRef();
}

void SAssetTable::Construct(const FArguments& InArgs, const TArray<TSharedPtr<FAssetTableRowData>>& InRows)
{
    Rows = InRows;

    SortByColumn = SAssetTable::ColumnID_Filename;
    SortMode = EColumnSortMode::Ascending;

    ChildSlot
    [
        SAssignNew(ListView, SListView<TSharedPtr<FAssetTableRowData>>)
        .ListItemsSource(&Rows)
        .OnGenerateRow(this, &SAssetTable::GenerateRow)
        .HeaderRow
        (
            SNew(SHeaderRow)
            + SHeaderRow::Column(SAssetTable::ColumnID_Filename)
            .DefaultLabel(FText::FromString(TEXT("Asset Filename")))
            .SortMode(this, &SAssetTable::GetColumnSortMode, SAssetTable::ColumnID_Filename)
            .OnSort(this, &SAssetTable::SortColumn)
            + SHeaderRow::Column(SAssetTable::ColumnID_NewFilename)
            .DefaultLabel(FText::FromString(TEXT("New Asset Filename")))
            .SortMode(this, &SAssetTable::GetColumnSortMode, SAssetTable::ColumnID_NewFilename)
            .OnSort(this, &SAssetTable::SortColumn)
            + SHeaderRow::Column(SAssetTable::ColumnID_AssetPath)
            .DefaultLabel(FText::FromString(TEXT("Path")))
            .SortMode(this, &SAssetTable::GetColumnSortMode, SAssetTable::ColumnID_AssetPath)
            .OnSort(this, &SAssetTable::SortColumn)
        )
    ];

    ListView->RequestListRefresh();
}

void SAssetTable::Refresh()
{
    ListView->RebuildList();
}

TSharedRef<ITableRow> SAssetTable::GenerateRow(const TSharedPtr<FAssetTableRowData> Item,
    const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SAssetTableRow, Item.ToSharedRef(), OwnerTable);
}

void SAssetTable::SortColumn(const EColumnSortPriority::Type SortPriority, const FName& ColumnId,
    const EColumnSortMode::Type InSortMode)
{
    SortByColumn = ColumnId;
    SortMode = InSortMode;


    if (ColumnId == SAssetTable::ColumnID_Filename)
    {
        if (SortMode == EColumnSortMode::Ascending)
        {
            Rows.Sort([](const TSharedPtr<FAssetTableRowData>& A, const TSharedPtr<FAssetTableRowData>& B) {
                return A->Filename < B->Filename;
            });
        }
        else if (SortMode == EColumnSortMode::Descending)
        {
            Rows.Sort([](const TSharedPtr<FAssetTableRowData>& A, const TSharedPtr<FAssetTableRowData>& B) {
                return A->Filename >= B->Filename;
            });
        }
    }
    else if (ColumnId == SAssetTable::ColumnID_NewFilename)
    {
        if (SortMode == EColumnSortMode::Ascending)
        {
            Rows.Sort([](const TSharedPtr<FAssetTableRowData>& A, const TSharedPtr<FAssetTableRowData>& B) {
                return A->NewFilename < B->NewFilename;
            });
        }
        else if (SortMode == EColumnSortMode::Descending)
        {
            Rows.Sort([](const TSharedPtr<FAssetTableRowData>& A, const TSharedPtr<FAssetTableRowData>& B) {
                return A->NewFilename >= B->NewFilename;
            });
        }
    }
    else if (ColumnId == SAssetTable::ColumnID_AssetPath)
    {
        if (SortMode == EColumnSortMode::Ascending)
        {
            Rows.Sort([](const TSharedPtr<FAssetTableRowData>& A, const TSharedPtr<FAssetTableRowData>& B) {
                return A->Path < B->Path;
            });
        }
        else if (SortMode == EColumnSortMode::Descending)
        {
            Rows.Sort([](const TSharedPtr<FAssetTableRowData>& A, const TSharedPtr<FAssetTableRowData>& B) {
                return A->Path >= B->Path;
            });
        }
    }
    ListView->RequestListRefresh();
}

EColumnSortMode::Type SAssetTable::GetColumnSortMode(const FName ColumnId) const
{
    if (SortByColumn != ColumnId)
    {
        return EColumnSortMode::None;
    }

    return SortMode;
}
