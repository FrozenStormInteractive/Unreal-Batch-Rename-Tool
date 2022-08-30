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

#include "BatchRenamingOperation_Add.h"

#define LOCTEXT_NAMESPACE "BatchRenameTool"

FText UBatchRenamingOperation_Add::GetDisplayName_Implementation() const
{
    return FText::Format(LOCTEXT("BatchRenamingOperation_Add_DisplayName", "Add {0} at index {1}"), FText::FromString(Str), Index);
}

FString UBatchRenamingOperation_Add::Apply_Implementation(const FString& In) const
{
    FString Copy = In;

    int Idx = Index;
    if(Idx < 0 || Idx > Copy.Len())
    {
        return Copy;
    }

    if(Backward)
    {
        Idx = Copy.Len() - Index;
    }

    Copy.InsertAt(Idx, Str);
    return Copy;
}

#undef LOCTEXT_NAMESPACE
