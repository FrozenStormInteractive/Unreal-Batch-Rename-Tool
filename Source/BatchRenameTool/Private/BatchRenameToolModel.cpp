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

#include "BatchRenameToolModel.h"

void FBatchRenameToolModel::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObjects(Operations);
    Collector.AddReferencedObject(SelectedOperation);
}

void FBatchRenameToolModel::AddOperation(TObjectPtr<UBatchRenamingOperation> Operation)
{
    if (IsValid(Operation) && !Operations.Contains(Operation))
    {
        Operations.Add(Operation);
        OnOperationListModifiedDelegate.Broadcast();
    }
}

void FBatchRenameToolModel::AddOperation(TObjectPtr<UBatchRenamingOperationFactory> Factory)
{
    AddOperation(Factory->Create());
}

void FBatchRenameToolModel::RemoveOperation(TObjectPtr<UBatchRenamingOperation> Operation)
{
    if (IsValid(Operation))
    {
        Operations.Remove(Operation);
        OnOperationListModifiedDelegate.Broadcast();
    }
}

void FBatchRenameToolModel::SelectOperation(const TObjectPtr<UBatchRenamingOperation>& Operation)
{
    if ((Operation == nullptr || Operations.Contains(Operation)) && Operation != SelectedOperation)
    {
        SelectedOperation = Operation;
        OnSelectionChangedDelegate.Broadcast();
    }
}
