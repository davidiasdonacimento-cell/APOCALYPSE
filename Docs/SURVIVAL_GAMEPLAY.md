# APOCALYPSE — Survival Gameplay Core

## Scope
This stage introduces authoritative player survival state and a reusable interaction contract. It is a gameplay systems layer, not a finished HUD, inventory, crafting system, or authored world.

## Player survival component
Attach `UApocalypseSurvivalComponent` to the player character. It owns health, hunger, thirst, and stamina. Values are clamped to configurable maxima and replicated from the server. Only the authority may change state through the public mutation methods.

- Hunger and thirst drain on a one-second simulation step.
- Starvation and dehydration apply configurable health damage only after the corresponding need reaches zero.
- Stamina recovers at a configurable rate and can be spent by sprinting or other gameplay actions.
- `OnSurvivalChanged` is the UI/gameplay notification point.
- Food, water, healing, damage, and stamina costs are exposed as validated methods.

The component uses a 0.25-second component tick interval but advances its simulation in whole-second steps. It does not run a per-frame survival simulation.

## Interaction contract
Implement `IApocalypseInteractable` on world actors (Blueprint or C++). Provide `CanInteract` and `Interact` implementations. The player's `UApocalypseInteractionComponent`:
- refreshes local focus at 10 Hz using a visibility-channel line trace;
- exposes the focused actor for prompts;
- validates range and interface again on the server before executing interaction;
- exposes `TryInteract` for Enhanced Input or UI binding.

The server performs the authoritative interaction trace; clients cannot choose an arbitrary target actor through the RPC.

## Integration
`AApocalypseCharacter` owns both components by default. Bind an Enhanced Input action to `TryInteract` in the eventual input mapping context. Bind HUD widgets to `OnSurvivalChanged` and the read-only getters. Configure collision responses so interactable actors block the selected trace channel.

## Deliberate boundaries
This change does not claim a completed inventory, item database, loot spawning, crafting, combat, animation, HUD, save-game persistence, or multiplayer session flow. Those are separate systems and require authored assets/input/UI integration.

## Acceptance criteria
- Survival values stay within configured ranges.
- Only the server mutates replicated survival values.
- Needs drain at the configured per-minute rates; depletion damages health.
- Stamina spend rejects invalid/insufficient requests.
- Interaction only executes for actors implementing the interface and passing server-side range/permission checks.
- Focus updates at a bounded 10 Hz cadence, not every rendered frame.
