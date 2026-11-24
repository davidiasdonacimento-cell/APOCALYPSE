# Inventory, equipment and storage UI

## Runtime UI
- `UApocalypseInventoryWidget` builds a native UMG panel at runtime and is instantiated for the locally controlled character.
- The item selector displays catalog names and stack quantities; the selected item is preserved across refreshes when it remains in inventory.
- Actions support consuming one unit, equipping the selected item, and one-unit transfers to or from an opened storage container.
- A separate equipment-slot selector supports unequipping the selected slot.
- The equipment summary shows head, body, backpack and utility slots.
- The native widget can be subclassed in UMG for final art direction without replacing its gameplay bindings.

## Built-in equipment catalog
The default catalog includes four non-stackable starter equipment definitions:
- `cloth_cap` — head
- `reinforced_vest` — body
- `field_pack` — backpack
- `utility_belt` — utility

Projects can add further equipment through a custom item Data Asset. Each item must use category `Equipment` and specify its target slot.

## Authority and consistency
- Equipment changes are server-authoritative and replicated.
- Replacing an occupied slot returns the previous item to inventory. If the inventory cannot accept the previous item, the equip request is rolled back.
- Unequipping returns the item to inventory and fails safely when there is no capacity.
- Storage transfers are requested through the owning player's inventory component for both directions; the server validates world, authority, and 500-unit interaction distance before changing either inventory.
- UI actions request operations and never mutate replicated arrays directly.

## Validation boundary
GitHub Actions validates required source files, project metadata, and key integration invariants. It does not compile Unreal Engine C++ or execute UMG in an Unreal Editor session. A packaged Android build still requires a configured Unreal Engine 5.8 toolchain and Android SDK/NDK/JDK.
