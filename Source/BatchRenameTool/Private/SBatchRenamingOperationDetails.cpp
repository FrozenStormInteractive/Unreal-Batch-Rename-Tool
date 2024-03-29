﻿/*
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

#include "SBatchRenamingOperationDetails.h"

#include "BatchRenameToolModel.h"
#include "SlateOptMacros.h"
#include "PropertyEditorModule.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBatchRenamingOperationDetails::Construct(const FArguments& InArgs, const TSharedRef<FBatchRenameToolModel>& InModel)
{
    Model = InModel;
    Model->OnSelectionChanged().AddSP(this, &SBatchRenamingOperationDetails::OnSelectionChanged);

    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bAllowSearch = true;
    DetailsViewArgs.bShowOptions = true;
    DetailsViewArgs.bHideSelectionTip = true;
    DetailsViewArgs.bShowPropertyMatrixButton = false;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsView->OnFinishedChangingProperties().AddSP(this, &SBatchRenamingOperationDetails::OnPropertiesChanged);

	ChildSlot
	[
	    DetailsView.ToSharedRef()
	];
}

void SBatchRenamingOperationDetails::OnSelectionChanged()
{
    DetailsView->SetObject(Model->GetSelectedOperation(), true);
}

void SBatchRenamingOperationDetails::OnPropertiesChanged(const FPropertyChangedEvent& InEvent)
{
    if (Model.IsValid())
    {
        Model->OnOperationListModified().Broadcast();
    }
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
