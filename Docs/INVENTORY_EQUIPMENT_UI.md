# Inventory, equipment and storage UI

## Runtime UI
- `UApocalypseInventoryWidget` builds a native UMG panel at runtime. It lists replicated inventory stacks, exposes consume/equip actions, and exposes one-item storage transfers after the storage-open event.
- The selected item is preserved across refreshes when it remains in the inventory.
- The equipment summary shows the four supported slots: head, body, backpack and utility.
- The widget is intentionally a native C++ widget class so a project can instantiate it directly or subclass it in UMG for art direction.

## Equipment data
- Equipment items are defined in the item catalog with category `Equipment` and a matching slot.
- Equipment changes are server-authoritative and replicated.
- Replacing an occupied slot returns the previous item to inventory. If inventory cannot accept the previous item, the operation is rolled back.
- Unequipping returns the item to inventory and fails safely when there is no capacity.

## Integration
- The character owns the inventory and equipment components.
- The character creates the inventory widget for its locally controlled instance and supplies both components.
- Storage access is driven by the existing inventory storage-open client event.
- The inventory and equipment components remain authoritative; UI actions request operations and do not mutate replicated state directly.

## Validation boundary
GitHub Actions validates required source files, project metadata, and key integration invariants. It does not compile Unreal Engine C++ or execute UMG in an Unreal Editor session. A packaged Android build still requires a configured Unreal Engine 5.8 toolchain and Android SDK/NDK/JDK.
