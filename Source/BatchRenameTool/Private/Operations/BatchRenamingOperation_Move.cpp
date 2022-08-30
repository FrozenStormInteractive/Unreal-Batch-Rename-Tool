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

#include "BatchRenamingOperation_Move.h"

#define LOCTEXT_NAMESPACE "BatchRenameTool"

FText UBatchRenamingOperation_Move::GetDisplayName_Implementation() const
{
    return FText::Format(LOCTEXT("UBatchRenamingOperation_Move_DisplayName", "Move {0} characters from index {1} to index {2}"), Count, From, To);
}

FString UBatchRenamingOperation_Move::Apply_Implementation(const FString& In) const
{
    FString Copy = In;

    int FromIdx = From;
    if (FromIdx < 0 || FromIdx > Copy.Len())
    {
        return Copy;
    }

    int ToIdx = To;
    if (ToIdx < 0 || ToIdx > Copy.Len())
    {
        return Copy;
    }

    int C = Count;
    if (FromIdx + C > Copy.Len())
    {
        C = Copy.Len() - FromIdx;
    }
    if (C >= Copy.Len())
    {
        return Copy;
    }

    FString Part = Copy.Mid(FromIdx, C);
    Copy.InsertAt(ToIdx, Part);
    Copy.RemoveAt(FromIdx, C);

    return Copy;
}

#undef LOCTEXT_NAMESPACE
