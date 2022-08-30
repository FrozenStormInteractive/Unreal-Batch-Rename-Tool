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

#include "SBatchRenamingOperationList.h"
#include "BatchRenameToolModel.h"
#include "BatchRenamingOperation.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSearchBox.h"
#include "SPositiveActionButton.h"

#define LOCTEXT_NAMESPACE "BatchRenameTool"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBatchRenamingOperationList::Construct(const FArguments& InArgs, const TSharedRef<FBatchRenameToolModel>& InModel)
{
    Model = InModel;

    Model->OnOperationListModified().AddSP(this, &SBatchRenamingOperationList::RefreshList);

	ChildSlot
	[
	    SNew(SVerticalBox)

	    +SVerticalBox::Slot()
	    .AutoHeight()
        .Padding( 8.0f, 8.0f, 8.0f, 4.0f )
        [
            SNew(SHorizontalBox)

            +SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
                SNew(SPositiveActionButton)
                .OnGetMenuContent(this, &SBatchRenamingOperationList::MakeOperationsMenuWidget)
                .Icon(FAppStyle::Get().GetBrush("Icons.Plus"))
                .Text(LOCTEXT("AddAssetButton", "Add"))
            ]

            +SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .Padding(4.f, 0.f, 0.f, 0.f)
            [
                SAssignNew(FilterTextBoxWidget, SSearchBox)
                .HintText(LOCTEXT("OperationDetailsFilterSearch", "Search..."))
                .ToolTipText(LOCTEXT("OperationDetailsFilterSearchHint", "Type here to search (pressing enter selects the results)"))
                // .OnTextChanged(this, &SSceneOutliner::OnFilterTextChanged)
                // .OnTextCommitted(this, &SSceneOutliner::OnFilterTextCommitted)
            ]
        ]

        +SVerticalBox::Slot()
        .FillHeight(1.0)
        [
            SAssignNew(ListView, SListView<TSharedPtr<FOperationListRowData>>)
            .ListItemsSource(&Rows)
            .OnGenerateRow(this, &SBatchRenamingOperationList::GenerateRow)
            .SelectionMode(ESelectionMode::Single)
            .OnSelectionChanged(this, &SBatchRenamingOperationList::OnListViewSelectionChanged)
        ]
	];
}

struct FFactoryListItem : public FGCObject
{
    TObjectPtr<UBatchRenamingOperationFactory> Factory;
    FText DisplayName;

    FFactoryListItem(UBatchRenamingOperationFactory* InFactory, const FText& InDisplayName)
        : Factory(InFactory), DisplayName(InDisplayName)
    {}

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        Collector.AddReferencedObject(Factory);
    }
};

TArray<FFactoryListItem> FindFactories()
{
    TArray<FFactoryListItem> Factories;
    for (TObjectIterator<UClass> It; It; ++It)
    {
        const UClass* Class = *It;

        if (Class->IsChildOf(UBatchRenamingOperationFactory::StaticClass()) && !Class->HasAnyClassFlags(CLASS_Abstract))
        {
            UBatchRenamingOperationFactory* Factory = Class->GetDefaultObject<UBatchRenamingOperationFactory>();
            if (ensure(!Factory->GetDisplayName().IsEmpty()))
            {
                Factories.Add(FFactoryListItem(Factory, Factory->GetDisplayName()));
            }
        }
    }

    return Factories;
}

TSharedRef<SWidget> SBatchRenamingOperationList::MakeOperationsMenuWidget()
{
    FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection=*/true, nullptr);

    TArray<FFactoryListItem> Factories = FindFactories();
    if (Factories.Num() > 0)
    {
        MenuBuilder.BeginSection("ContentBrowserGetContent", LOCTEXT("GetContentMenuHeading", "Content"));
        {
            for (const FFactoryListItem& FactoryItem : Factories)
            {
                MenuBuilder.AddMenuEntry(
                    FactoryItem.DisplayName,
                    FText::GetEmpty(),
                    FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AddContent"),
                    FUIAction(FExecuteAction::CreateSP(Model.ToSharedRef(), &FBatchRenameToolModel::AddOperation, FactoryItem.Factory))
                );
            }
        }
        MenuBuilder.EndSection();
    }

    FDisplayMetrics DisplayMetrics;
    FSlateApplication::Get().GetCachedDisplayMetrics(DisplayMetrics);

    const FVector2D DisplaySize(
        DisplayMetrics.PrimaryDisplayWorkAreaRect.Right - DisplayMetrics.PrimaryDisplayWorkAreaRect.Left,
        DisplayMetrics.PrimaryDisplayWorkAreaRect.Bottom - DisplayMetrics.PrimaryDisplayWorkAreaRect.Top);

    return
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .MaxHeight(DisplaySize.Y * 0.9)
        [
            MenuBuilder.MakeWidget()
        ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBatchRenamingOperationList::RefreshList()
{
    Rows.Empty();

    for (const TObjectPtr<UBatchRenamingOperation>& Op : Model->GetOperations())
    {
        Rows.Add(MakeShared<FOperationListRowData>(Op));
    }
    ListView->RebuildList();
}

void SBatchRenamingOperationList::FOperationListRowData::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(Operation);
}

class SBatchRenamingOperationListRow : public STableRow<TSharedPtr<SBatchRenamingOperationList::FOperationListRowData>>
{
public:
    SLATE_BEGIN_ARGS(SBatchRenamingOperationListRow) {}
    SLATE_END_ARGS()

    BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
    void Construct(const FArguments& InArgs, TSharedRef<SBatchRenamingOperationList::FOperationListRowData> InLine, TSharedRef<STableViewBase> InOwnerTableView)
    {
        STableRow<TSharedPtr<SBatchRenamingOperationList::FOperationListRowData>>::Construct(
            STableRow<TSharedPtr<SBatchRenamingOperationList::FOperationListRowData>>::FArguments(), InOwnerTableView);

        Data = InLine;
        ChildSlot
        [
            SNew(STextBlock)
            .Margin(FMargin(6.0f, 2.5f))
            .Text(this, &SBatchRenamingOperationListRow::GetDisplayText)
        ];
    }
    END_SLATE_FUNCTION_BUILD_OPTIMIZATION

    FText GetDisplayText() const
    {
        return Data->Operation->GetDisplayName();
    }

private:
    TSharedPtr<SBatchRenamingOperationList::FOperationListRowData> Data;
};

TSharedRef<ITableRow> SBatchRenamingOperationList::GenerateRow(const TSharedPtr<FOperationListRowData> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SBatchRenamingOperationListRow, Item.ToSharedRef(), OwnerTable);
}

void SBatchRenamingOperationList::OnListViewSelectionChanged(TSharedPtr<FOperationListRowData> ItemSelected, ESelectInfo::Type SelectInfo)
{
    if (Model.IsValid() && ItemSelected.IsValid() && IsValid(ItemSelected->Operation))
    {
        Model->SelectOperation(ItemSelected->Operation);
    }
}

#undef LOCTEXT_NAMESPACE
