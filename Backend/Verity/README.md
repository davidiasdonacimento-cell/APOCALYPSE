# VERITY AI backend

This service keeps provider credentials off the Android client. It exposes:
- `GET /health`
- `POST /v1/dialogue` — contextual Portuguese dialogue
- `POST /v1/voice` — Brazilian Portuguese speech returned as PCM WAV

## Deploy
Deploy this folder as a Docker web service. The repository includes a Render Blueprint at the root. Configure `OPENAI_API_KEY` as a private environment variable in the hosting dashboard. Never put the key in Unreal config, source code, or an APK.

After deployment, copy the HTTPS service URL into the `AIBackendBaseUrl` setting of the Verity companion Blueprint. The Unreal client calls only this backend; it never receives the provider key.

## Costs and limits
The chat and speech endpoints call a metered AI provider. Usage can incur charges. Set a provider spending limit and monitor usage before enabling the service. This service applies basic per-IP cooldowns and input/audio size limits; it is not a substitute for production authentication, distributed rate limiting, or abuse monitoring.

## Local run
```bash
cd Backend/Verity
python -m pip install -r requirements.txt
export OPENAI_API_KEY="your-server-side-key"
uvicorn main:app --host 127.0.0.1 --port 8000
```

## Data handling
The backend does not persist player messages. Session memory is held in the game process and only the bounded recent context is sent for a dialogue request. Do not send personal or sensitive information in game dialogue.
