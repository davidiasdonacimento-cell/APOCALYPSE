# VERITY — AI companion integration

## Runtime modules
- `AVerityCompanionCharacter`: replicated companion actor, follows its owning player and owns the systems below.
- `AVerityAIController`: Unreal AI controller used by the companion.
- `UVerityCompanionAI`: server-side threat scan using collision overlaps and the game target interface.
- `UVerityCombatSystem`: server-authoritative, abstract in-game damage callback. It does not implement real-world weapon behavior.
- `UVerityNavigationSystem`: follows the player or moves toward the current hostile target using Unreal path following.
- `UVerityDialogueSystem`: spontaneous dialogue timer, player text messages, contextual request and offline fallback.
- `UVerityVoiceSystem`: async HTTPS TTS request, bounded WAV cache, PCM WAV playback and volume control.
- `UVerityMemorySystem`: bounded current-session event memory.
- `UVerityPersonalitySystem`: Brazilian Portuguese personality prompt and offline lines.
- `UVeritySettingsWidget`: runtime controls for companion, automatic dialogue, voice, volume and text chat.

The player character spawns one replicated Verity companion on the server. The settings panel is created for the locally controlled player.

## Required game integration

### Enemy actors
The current repository does not yet contain a zombie actor class. To be recognized as a hostile target, each zombie Blueprint/C++ class must implement `VerityHostileTarget`:
- `IsValidVerityTarget` returns true only while that enemy can still be targeted.
- `ReceiveVerityDamage` routes the abstract game damage amount into that enemy's existing health/damage system.
- The enemy must have a collision component using Pawn or WorldDynamic object type so the bounded overlap scan can detect it.

This interface avoids coupling Verity to a future zombie implementation or replacing an existing enemy system.

### Navigation and visuals
- The playable map needs a NavMeshBoundsVolume covering walkable ground and navigation data generated in the Unreal Editor.
- The companion actor exposes `VerityMesh` for a project-owned Skeletal Mesh. Assign the mesh and an Animation Blueprint in a Blueprint child of `AVerityCompanionCharacter` for final appearance and locomotion.
- No character mesh, face rig, voice identity asset, animation set, zombie class, or authored map content is present in the current repository. Those assets cannot be fabricated by C++ source alone.

## AI dialogue and voice service
The Android game never stores the provider API key. It calls a small backend in `Backend/Verity`:
- `POST /v1/dialogue` returns a contextual Portuguese reply.
- `POST /v1/voice` returns WAV audio for playback.
- `GET /health` reports backend status.

The root `render.yaml` declares a free Render web service. Deploy the Blueprint, then set `OPENAI_API_KEY` privately in the service environment. The Unreal config points to `https://apocalypse-verity-ai.onrender.com`; verify the actual URL after deployment. If the backend is unavailable, local personality lines still appear as dialogue text, but generated speech is unavailable.

AI chat and TTS are metered provider operations and can incur charges. Set a spending limit and monitor usage before enabling the backend. The free hosting instance may sleep when idle, causing a delay on the first request.

## Current limitations
- Player-to-Verity input is text through the in-game field. Microphone capture and speech-to-text are not implemented yet.
- Dialogue memory is bounded to the current game session and is not saved across restarts.
- Automatic conversation is scheduled roughly every 48–78 seconds and pauses while a hostile target is active.
- The voice service expects PCM WAV, 16-bit, mono or stereo. Unsupported audio formats are rejected.
- AI output is asynchronous; offline lines are used after network failures.
- No Unreal Engine 5.8 C++ compile, Editor automation run, Android package, or on-device audio/navigation test has been performed in this repository session.

## Validation
GitHub Actions checks source structure, project dependencies, backend Python syntax and service contracts. This is not a substitute for an Unreal Engine 5.8 build or in-Editor multiplayer testing.
