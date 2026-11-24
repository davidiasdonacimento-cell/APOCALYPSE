#include "UI/ApocalypseInventoryWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"

namespace
{
    UButton* AddActionButton(UWidgetTree* Tree, UVerticalBox* Parent, const TCHAR* Label)
    {
        UButton* Button = Tree->ConstructWidget<UButton>(UButton::StaticClass());
        UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
        Text->SetText(FText::FromString(Label));
        Button->AddChild(Text);
        Parent->AddChild(Button);
        return Button;
    }

    FString SlotLabel(EApocalypseEquipmentSlot Slot)
    {
        switch (Slot)
        {
            case EApocalypseEquipmentSlot::Head: return TEXT("Cabeça");
            case EApocalypseEquipmentSlot::Body: return TEXT("Corpo");
            case EApocalypseEquipmentSlot::Backpack: return TEXT("Mochila");
            default: return TEXT("Utilitário");
        }
    }
}

void UApocalypseInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!WidgetTree) return;

    UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    WidgetTree->RootWidget = Root;

    UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Title->SetText(FText::FromString(TEXT("APOCALYPSE  |  INVENTÁRIO")));
    Root->AddChild(Title);

    ItemSelector = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass());
    Root->AddChild(ItemSelector);
    ItemSelector->OnSelectionChanged.AddDynamic(this, &UApocalypseInventoryWidget::HandleSelectionChanged);

    ItemDetailsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Root->AddChild(ItemDetailsText);

    EquipmentSlotSelector = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass());
    EquipmentSlotSelector->AddOption(TEXT("Cabeça|0"));
    EquipmentSlotSelector->AddOption(TEXT("Corpo|1"));
    EquipmentSlotSelector->AddOption(TEXT("Mochila|2"));
    EquipmentSlotSelector->AddOption(TEXT("Utilitário|3"));
    EquipmentSlotSelector->SetSelectedOption(TEXT("Cabeça|0"));
    EquipmentSlotSelector->OnSelectionChanged.AddDynamic(this, &UApocalypseInventoryWidget::HandleEquipmentSlotChanged);
    Root->AddChild(EquipmentSlotSelector);

    EquipmentText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Root->AddChild(EquipmentText);
    StorageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Root->AddChild(StorageText);
    StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Root->AddChild(StatusText);

    ConsumeButton = AddActionButton(WidgetTree, Root, TEXT("Consumir 1 unidade"));
    EquipButton = AddActionButton(WidgetTree, Root, TEXT("Equipar item selecionado"));
    UnequipButton = AddActionButton(WidgetTree, Root, TEXT("Desequipar slot selecionado"));
    StoreButton = AddActionButton(WidgetTree, Root, TEXT("Guardar 1 unidade"));
    TakeButton = AddActionButton(WidgetTree, Root, TEXT("Retirar 1 unidade"));
    ConsumeButton->OnClicked.AddDynamic(this, &UApocalypseInventoryWidget::HandleConsume);
    EquipButton->OnClicked.AddDynamic(this, &UApocalypseInventoryWidget::HandleEquip);
    UnequipButton->OnClicked.AddDynamic(this, &UApocalypseInventoryWidget::HandleUnequip);
    StoreButton->OnClicked.AddDynamic(this, &UApocalypseInventoryWidget::HandleStore);
    TakeButton->OnClicked.AddDynamic(this, &UApocalypseInventoryWidget::HandleTake);
    Refresh();
    SetStatus(FText::FromString(TEXT("Selecione um item para ver as ações.")));
}

void UApocalypseInventoryWidget::SetInventoryContext(UApocalypseInventoryComponent* InInventory,
                                                       UApocalypseEquipmentComponent* InEquipment)
{
    if (Inventory)
    {
        Inventory->OnInventoryChanged.RemoveDynamic(this, &UApocalypseInventoryWidget::HandleInventoryChanged);
        Inventory->OnStorageOpened.RemoveDynamic(this, &UApocalypseInventoryWidget::HandleStorageOpened);
    }
    if (Equipment)
        Equipment->OnEquipmentChanged.RemoveDynamic(this, &UApocalypseInventoryWidget::HandleEquipmentChanged);
    if (OpenStorage)
        OpenStorage->OnInventoryChanged.RemoveDynamic(this, &UApocalypseInventoryWidget::HandleInventoryChanged);

    Inventory = InInventory;
    Equipment = InEquipment;
    OpenStorage = nullptr;

    if (Inventory)
    {
        Inventory->OnInventoryChanged.AddDynamic(this, &UApocalypseInventoryWidget::HandleInventoryChanged);
        Inventory->OnStorageOpened.AddDynamic(this, &UApocalypseInventoryWidget::HandleStorageOpened);
    }
    if (Equipment)
        Equipment->OnEquipmentChanged.AddDynamic(this, &UApocalypseInventoryWidget::HandleEquipmentChanged);
    Refresh();
}

void UApocalypseInventoryWidget::HandleSelectionChanged(FString Selected, ESelectInfo::Type SelectionType)
{
    const int32 Separator = Selected.Find(TEXT("|"));
    SelectedItemId = Separator == INDEX_NONE ? NAME_None : FName(*Selected.Mid(Separator + 1));
    if (ItemDetailsText && Inventory && !SelectedItemId.IsNone())
    {
        const UApocalypseItemCatalog* Catalog = Inventory->GetCatalog();
        const FApocalypseItemDefinition* Definition = Catalog ? Catalog->FindItem(SelectedItemId) : nullptr;
        ItemDetailsText->SetText(Definition ? Definition->DisplayName :
            FText::FromName(SelectedItemId));
    }
}

void UApocalypseInventoryWidget::HandleEquipmentSlotChanged(FString Selected, ESelectInfo::Type SelectionType)
{
    const int32 Separator = Selected.Find(TEXT("|"));
    if (Separator == INDEX_NONE) return;
    const int32 SlotValue = FCString::Atoi(*Selected.Mid(Separator + 1));
    if (SlotValue >= 0 && SlotValue <= 3)
        SelectedEquipmentSlot = static_cast<EApocalypseEquipmentSlot>(SlotValue);
}

void UApocalypseInventoryWidget::HandleConsume()
{
    if (!Inventory || SelectedItemId.IsNone()) return;
    Inventory->ConsumeItem(SelectedItemId);
    SetStatus(FText::FromString(TEXT("Pedido de consumo enviado ao servidor.")));
}

void UApocalypseInventoryWidget::HandleEquip()
{
    if (!Equipment || SelectedItemId.IsNone()) return;
    Equipment->RequestEquip(SelectedItemId);
    SetStatus(FText::FromString(TEXT("Pedido de equipamento enviado ao servidor.")));
}

void UApocalypseInventoryWidget::HandleUnequip()
{
    if (!Equipment) return;
    Equipment->RequestUnequip(SelectedEquipmentSlot);
    SetStatus(FText::FromString(TEXT("Pedido para desequipar enviado ao servidor.")));
}

void UApocalypseInventoryWidget::HandleStore()
{
    if (!Inventory || !OpenStorage || SelectedItemId.IsNone()) return;
    Inventory->RequestTransferTo(OpenStorage, SelectedItemId, 1);
    SetStatus(FText::FromString(TEXT("Pedido para guardar enviado ao servidor.")));
}

void UApocalypseInventoryWidget::HandleTake()
{
    if (!Inventory || !OpenStorage || SelectedItemId.IsNone()) return;
    Inventory->RequestTransferFrom(OpenStorage, SelectedItemId, 1);
    SetStatus(FText::FromString(TEXT("Pedido para retirar enviado ao servidor.")));
}

void UApocalypseInventoryWidget::HandleInventoryChanged() { Refresh(); }
void UApocalypseInventoryWidget::HandleEquipmentChanged() { Refresh(); }

void UApocalypseInventoryWidget::HandleStorageOpened(UApocalypseInventoryComponent* StorageInventory)
{
    if (OpenStorage)
        OpenStorage->OnInventoryChanged.RemoveDynamic(this, &UApocalypseInventoryWidget::HandleInventoryChanged);
    OpenStorage = StorageInventory;
    if (OpenStorage)
        OpenStorage->OnInventoryChanged.AddDynamic(this, &UApocalypseInventoryWidget::HandleInventoryChanged);
    Refresh();
    SetStatus(FText::FromString(TEXT("Armazenamento conectado.")));
}

void UApocalypseInventoryWidget::SetStatus(const FText& Message)
{
    if (StatusText) StatusText->SetText(Message);
}

void UApocalypseInventoryWidget::Refresh()
{
    if (!ItemSelector) return;
    const FString PreviousSelection = SelectedItemId.ToString();
    ItemSelector->ClearOptions();
    SelectedItemId = NAME_None;

    if (Inventory)
    {
        for (const FApocalypseInventoryStack& Stack : Inventory->GetItems())
        {
            if (Stack.ItemId.IsNone() || Stack.Quantity <= 0) continue;
            const UApocalypseItemCatalog* Catalog = Inventory->GetCatalog();
            const FApocalypseItemDefinition* Definition = Catalog ? Catalog->FindItem(Stack.ItemId) : nullptr;
            const FString DisplayName = Definition && !Definition->DisplayName.IsEmpty()
                ? Definition->DisplayName.ToString() : Stack.ItemId.ToString();
            const FString Option = FString::Printf(TEXT("%s  x%d|%s"), *DisplayName,
                Stack.Quantity, *Stack.ItemId.ToString());
            ItemSelector->AddOption(Option);
            if (SelectedItemId.IsNone() || Stack.ItemId.ToString() == PreviousSelection)
                SelectedItemId = Stack.ItemId;
        }
    }

    if (!SelectedItemId.IsNone())
    {
        const FString Suffix = TEXT("|") + SelectedItemId.ToString();
        for (int32 Index = 0; Index < ItemSelector->GetOptionCount(); ++Index)
        {
            const FString Option = ItemSelector->GetOptionAtIndex(Index);
            if (Option.EndsWith(Suffix)) { ItemSelector->SetSelectedOption(Option); break; }
        }
    }
    if (ItemDetailsText)
    {
        const UApocalypseItemCatalog* Catalog = Inventory ? Inventory->GetCatalog() : nullptr;
        const FApocalypseItemDefinition* Definition = Catalog ? Catalog->FindItem(SelectedItemId) : nullptr;
        ItemDetailsText->SetText(Definition ? Definition->DisplayName : FText::FromString(TEXT("Nenhum item selecionado")));
    }

    if (EquipmentText && Equipment)
    {
        FString Summary = TEXT("EQUIPAMENTOS");
        for (int32 Slot = 0; Slot < 4; ++Slot)
        {
            const EApocalypseEquipmentSlot Type = static_cast<EApocalypseEquipmentSlot>(Slot);
            const FName ItemId = Equipment->GetEquippedItem(Type);
            Summary += FString::Printf(TEXT("\n%s: %s"), *SlotLabel(Type),
                ItemId.IsNone() ? TEXT("Vazio") : *ItemId.ToString());
        }
        EquipmentText->SetText(FText::FromString(Summary));
    }

    if (StorageText)
    {
        FString Summary = OpenStorage ? TEXT("ARMAZENAMENTO CONECTADO") :
            TEXT("ARMAZENAMENTO: aproxime-se de um baú e interaja");
        if (OpenStorage)
            Summary += FString::Printf(TEXT("\nPilhas no baú: %d"), OpenStorage->GetItems().Num());
        StorageText->SetText(FText::FromString(Summary));
    }
}
