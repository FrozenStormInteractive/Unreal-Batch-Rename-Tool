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
#include "BatchRenamingOperation.h"

class UBatchRenamingOperation;
class UBatchRenamingOperationFactory;

class FBatchRenameToolModel : public FGCObject
{
public:
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override
    {
        return TEXT("FBatchRenameToolModel");
    }

    DECLARE_MULTICAST_DELEGATE(FOnOperationListModifiedDelegate);
    FOnOperationListModifiedDelegate& OnOperationListModified()
    {
        return OnOperationListModifiedDelegate;
    }
    const TArray<TObjectPtr<UBatchRenamingOperation>>& GetOperations() const
    {
        return Operations;
    }
    void AddOperation(TObjectPtr<UBatchRenamingOperation> Operation, bool bSelect = true);
    void AddOperation(TObjectPtr<UBatchRenamingOperationFactory> Factory, bool bSelect = true);
    void RemoveOperation(TObjectPtr<UBatchRenamingOperation> Operation);
    void MoveOperationUp(TObjectPtr<UBatchRenamingOperation> Operation);
    void MoveOperationDown(TObjectPtr<UBatchRenamingOperation> Operation);

    DECLARE_MULTICAST_DELEGATE(FOnSelectionChangedDelegate);
    FOnSelectionChangedDelegate& OnSelectionChanged()
    {
        return OnSelectionChangedDelegate;
    }
    const TObjectPtr<UBatchRenamingOperation>& GetSelectedOperation() const
    {
        return SelectedOperation;
    }
    void SelectOperation(const TObjectPtr<UBatchRenamingOperation>& Operation);

private:
    TArray<TObjectPtr<UBatchRenamingOperation>> Operations;
    FOnOperationListModifiedDelegate OnOperationListModifiedDelegate;

    TObjectPtr<UBatchRenamingOperation> SelectedOperation = nullptr;
    FOnSelectionChangedDelegate OnSelectionChangedDelegate;
};
