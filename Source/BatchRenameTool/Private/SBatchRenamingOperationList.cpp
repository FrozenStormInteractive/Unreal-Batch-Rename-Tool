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
        ]

        +SVerticalBox::Slot()
        .FillHeight(1.0)
        [
            SAssignNew(ListView, SListView<TSharedPtr<FOperationListRowData>>)
            .ListItemsSource(&Rows)
            .OnGenerateRow(this, &SBatchRenamingOperationList::GenerateRow)
            .SelectionMode(ESelectionMode::Single)
            .ClearSelectionOnClick(false)
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

    virtual FString GetReferencerName() const override
    {
        return TEXT("FFactoryListItem");
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
                    FUIAction(FExecuteAction::CreateSP(Model.ToSharedRef(), &FBatchRenameToolModel::AddOperation, FactoryItem.Factory, true))
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
        Rows.Add(MakeShared<FOperationListRowData>(Model, Op));
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
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(STextBlock)
                .Margin(FMargin(6.0f, 2.5f))
                .Text(this, &SBatchRenamingOperationListRow::GetDisplayText)
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ContentPadding(0)
                .ButtonStyle(FAppStyle::Get(), "SimpleButton")
                .IsEnabled(this, &SBatchRenamingOperationListRow::IsMoveUpButtonEnabled)
                .Visibility(this, &SBatchRenamingOperationListRow::GetButtonVisibility)
                .OnClicked(this, &SBatchRenamingOperationListRow::OnMoveUpButtonClicked)
                .ToolTipText(LOCTEXT("MoveUpButton", "Move up."))
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .ForegroundColor_Lambda([this]() { return IsSelected() ? FSlateColor::UseForeground() : FSlateColor::UseStyle(); })
                .Content()
                [
                    SNew(SImage)
                    .Image(FAppStyle::Get().GetBrush("Icons.ChevronUp"))
                    .ColorAndOpacity(FSlateColor::UseForeground())
                ]
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ContentPadding(0)
                .ButtonStyle(FAppStyle::Get(), "SimpleButton")
                .IsEnabled(this, &SBatchRenamingOperationListRow::IsMoveDownButtonEnabled)
                .Visibility(this, &SBatchRenamingOperationListRow::GetButtonVisibility)
                .OnClicked(this, &SBatchRenamingOperationListRow::OnMoveDownButtonClicked)
                .ToolTipText(LOCTEXT("MoveDownButton", "Move down."))
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .ForegroundColor_Lambda([this]() { return IsSelected() ? FSlateColor::UseForeground() : FSlateColor::UseStyle(); })
                .Content()
                [
                    SNew(SImage)
                    .Image(FAppStyle::Get().GetBrush("Icons.ChevronDown"))
                    .ColorAndOpacity(FSlateColor::UseForeground())
                ]
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ContentPadding(0)
                .ButtonStyle(FAppStyle::Get(), "SimpleButton")
                .Visibility(this, &SBatchRenamingOperationListRow::GetButtonVisibility)
                .OnClicked(this, &SBatchRenamingOperationListRow::OnRemoveOperationButtonClicked)
                .ToolTipText(LOCTEXT("RemoveOperationButton", "Remove this operation."))
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .ForegroundColor_Lambda([this]() { return IsSelected() ? FSlateColor::UseForeground() : FSlateColor::UseStyle(); })
                .Content()
                [
                    SNew(SImage)
                    .Image(FAppStyle::Get().GetBrush("Icons.X"))
                    .ColorAndOpacity(FSlateColor::UseForeground())
                ]
            ]
        ];
    }
    END_SLATE_FUNCTION_BUILD_OPTIMIZATION

private:
    FText GetDisplayText() const
    {
        return Data->Operation->GetDisplayName();
    }

    EVisibility GetButtonVisibility() const
    {
        return IsHovered() || IsSelected() ? EVisibility::Visible : EVisibility::Hidden;
    }

    bool IsMoveUpButtonEnabled() const
    {
        if (Data.IsValid())
        {
            const int32 Index = Data->Model->GetOperations().Find(Data->Operation);
            return Index != INDEX_NONE && Index > 0;
        }
        return false;
    }

    bool IsMoveDownButtonEnabled() const
    {
        if (Data.IsValid())
        {
            const int32 Index = Data->Model->GetOperations().Find(Data->Operation);
            return Index != INDEX_NONE && Index < Data->Model->GetOperations().Num() - 1;
        }
        return false;
    }

    FReply OnMoveUpButtonClicked() const
    {
        if (Data.IsValid())
        {
            Data->Model->MoveOperationUp(Data->Operation);
        }
        return FReply::Handled();
    }

    FReply OnMoveDownButtonClicked() const
    {
        if (Data.IsValid())
        {
            Data->Model->MoveOperationDown(Data->Operation);
        }
        return FReply::Handled();
    }

    FReply OnRemoveOperationButtonClicked() const
    {
        if (Data.IsValid())
        {
            Data->Model->RemoveOperation(Data->Operation);
        }
        return FReply::Handled();
    }

    TSharedPtr<SBatchRenamingOperationList::FOperationListRowData> Data;
};

TSharedRef<ITableRow> SBatchRenamingOperationList::GenerateRow(const TSharedPtr<FOperationListRowData> Item, const TSharedRef<STableViewBase>& OwnerTable) const
{
    return SNew(SBatchRenamingOperationListRow, Item.ToSharedRef(), OwnerTable);
}

void SBatchRenamingOperationList::OnListViewSelectionChanged(TSharedPtr<FOperationListRowData> ItemSelected, ESelectInfo::Type SelectInfo) const
{
    if (Model.IsValid() && ItemSelected.IsValid() && IsValid(ItemSelected->Operation))
    {
        Model->SelectOperation(ItemSelected->Operation);
    }
}

#undef LOCTEXT_NAMESPACE
