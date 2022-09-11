---
sidebar_position: 1
---

# Create a custom renaming operation in C++



`Editor` module



:::note Your Title

Some **content** with _Markdown_ `syntax`. Check [this `api`](#).

:::


## Link the module

```cs title="CustomModule.Build.cs"
public class CustomModule : ModuleRules
{
    public CustomModule(ReadOnlyTargetRules Target) : base(Target)
    {
        ...

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            ...
            "BatchRenameTool",
        });

        ...
    }
}
```

## Create the operation class

Create a file at `src/pages/my-react-page.js`:

```cpp title="BatchRenamingOperation_Custom.h"
UCLASS(NotBlueprintable)
class UBatchRenamingOperation_Custom : public UBatchRenamingOperation
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "Add")
    FString Str;

    UPROPERTY(EditAnywhere, Category = "Add")
    uint32 Index = 0;

    UPROPERTY(EditAnywhere, Category = "Add")
    bool Backward = false;

    virtual FText GetDisplayName_Implementation() const override;
    virtual FString Apply_Implementation(const FString&) const override;
};
```

```cpp title="BatchRenamingOperation_Custom.cpp"
#include "BatchRenamingOperation_Custom.h"

FText UBatchRenamingOperation_Custom::GetDisplayName_Implementation() const
{
    return FText::Format(LOCTEXT("BatchRenamingOperation_Custom_DisplayName", "Add {0} at index {1}"), FText::FromString(Str), Index);
}

FString UBatchRenamingOperation_Custom::Apply_Implementation(const FString& In) const
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
```

## Create the factory

Create a file at `src/pages/my-markdown-page.md`:
```cpp title="BatchRenamingOperation_Custom.h"
UCLASS(NotBlueprintable)
class UBatchRenamingOperationFactory_Custom : public UBatchRenamingOperationFactory
{
    GENERATED_BODY()
public:
    inline FText GetDisplayName() const override
    {
        return FText::FromString("Custom");
    }

    inline TObjectPtr<UBatchRenamingOperation> Create() override
    {
        return NewObject<UBatchRenamingOperation_Custom>();
    }
};
```
