# APOCALYPSE — Inventory, Loot and Storage

## Systems delivered
- Item catalog with stable IDs, display names, categories, stack limits and consumable effects.
- Built-in starter catalog available without creating an editor asset: canned food, water bottle, medical kit, wood, stone and cloth. A custom Data Asset can replace it.
- Replicated player/container inventory with configurable slot capacity, stack merging, partial acceptance and server-authoritative add/remove operations.
- Consumable use connected to the survival component (food, water and health restoration).
- Replicated world pickups with interaction collision, assignable visual mesh, partial pickup and remaining quantity.
- Harvestable resource nodes with configurable yield, remaining quantity, interaction collision and assignable visual mesh.
- Storage actors with their own inventory, interaction collision, assignable mesh, and a client notification sent to the owning player when opened.
- Server-authoritative transfers between inventories in the same world, within 500 Unreal units.

## Runtime setup
1. The character receives an inventory component by default. If no custom catalog is assigned, the built-in item catalog is used.
2. Place pickup, resource-node and storage-container actors in the level.
3. Set each pickup/resource ItemId to a valid catalog ID. For example: wood, stone, cloth, canned_food, water_bottle or medical_kit.
4. Assign a Static Mesh to the actor's VisualMesh component for its final art. The dedicated interaction collision blocks the Visibility trace channel.
5. Assign a custom item catalog to the player and storage inventory components when the project has its authored Data Asset.
6. Bind the player's OnStorageOpened delegate to the inventory UI. The callback provides the storage inventory component.
7. Call RequestTransferTo on the source inventory to move a selected quantity. The server revalidates world, authority, quantity and distance.

## Rules and invariants
- Unknown IDs and invalid quantities never enter an inventory.
- AddItem returns the actual accepted quantity, supporting partial pickup/harvest when full.
- Existing stacks are filled before new slots are allocated.
- Removal is all-or-nothing.
- Inventory mutations and item consumption run on the server.
- Player inventory state, pickup quantities, and resource depletion replicate from the server.
- Replicated inventory arrays notify clients through OnInventoryChanged.
- Opening a storage sends its inventory reference to the owning player through a client RPC; transfers are independently revalidated by the server.

## Integration boundaries
The gameplay backend and actor interaction/collision components are implemented. This is not a finished inventory HUD, art pack, or authored loot distribution. Final meshes, item icons, UI layout, custom catalog Data Assets and level placement remain content-authoring tasks in Unreal Editor. No placeholder UI or untextured scene is claimed as finished content.

## Acceptance checks
- Unknown item IDs, invalid quantities and client-side direct mutations are rejected.
- Stack and slot limits are respected.
- Full inventories preserve uncollected pickup/resource quantities.
- Consuming an item removes one unit and applies its configured survival effects.
- Storage transfers cannot exceed source quantity or destination capacity.
- Storage opening notifies only the owning client.
- Multiplayer inventory, pickup and resource state replicate from server-owned actors/components.
