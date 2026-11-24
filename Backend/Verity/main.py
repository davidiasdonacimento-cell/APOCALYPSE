import os
import time
from collections import defaultdict
from typing import Optional

import httpx
from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import Response
from pydantic import BaseModel, Field

app = FastAPI(title="APOCALYPSE VERITY AI", version="1.0.0")
_last_request = defaultdict(float)

API_KEY = os.getenv("OPENAI_API_KEY", "")
API_BASE = os.getenv("OPENAI_BASE_URL", "https://api.openai.com/v1").rstrip("/")
CHAT_MODEL = os.getenv("OPENAI_CHAT_MODEL", "gpt-4o-mini")
TTS_MODEL = os.getenv("OPENAI_TTS_MODEL", "gpt-4o-mini-tts")
TTS_VOICE = os.getenv("OPENAI_TTS_VOICE", "nova")


class DialogueRequest(BaseModel):
    trigger: str = Field(default="ambient", max_length=40)
    player_message: str = Field(default="", max_length=500)
    context: str = Field(default="", max_length=5000)
    system_prompt: str = Field(default="", max_length=2500)


class VoiceRequest(BaseModel):
    text: str = Field(min_length=1, max_length=700)
    language: str = Field(default="pt-BR", max_length=10)
    voice: str = Field(default="nova", max_length=32)


def require_key() -> None:
    if not API_KEY:
        raise HTTPException(status_code=503, detail="AI provider is not configured")


def throttle(request: Request, bucket: str, interval: float) -> None:
    address = request.client.host if request.client else "unknown"
    key = f"{bucket}:{address}"
    now = time.monotonic()
    if now - _last_request[key] < interval:
        raise HTTPException(status_code=429, detail="Please wait before the next request")
    _last_request[key] = now


@app.get("/health")
async def health():
    return {"ok": True, "service": "verity", "provider_configured": bool(API_KEY)}


@app.post("/v1/dialogue")
async def dialogue(payload: DialogueRequest, request: Request):
    require_key()
    throttle(request, "dialogue", 12.0)
    system = payload.system_prompt.strip() or (
        "Você é VERITY, companheira fictícia do jogo APOCALYPSE. "
        "Responda em português brasileiro, com naturalidade, brevidade e empatia. "
        "Use apenas o contexto fornecido; não invente eventos do jogo."
    )
    user_context = (
        f"Evento de conversa: {payload.trigger}\n"
        f"Contexto recente do jogo: {payload.context}\n"
        f"Mensagem do jogador: {payload.player_message or '(nenhuma; inicie uma fala espontânea)'}\n"
        "Responda com uma fala curta, adequada para ser ouvida durante a partida."
    )
    headers = {"Authorization": f"Bearer {API_KEY}", "Content-Type": "application/json"}
    body = {
        "model": CHAT_MODEL,
        "messages": [
            {"role": "system", "content": system},
            {"role": "user", "content": user_context},
        ],
        "temperature": 0.8,
        "max_tokens": 120,
    }
    try:
        async with httpx.AsyncClient(timeout=25.0) as client:
            result = await client.post(f"{API_BASE}/chat/completions", headers=headers, json=body)
            result.raise_for_status()
            data = result.json()
            reply = data["choices"][0]["message"]["content"].strip()
            if not reply:
                raise ValueError("empty reply")
            return {"reply": reply[:700]}
    except (httpx.HTTPError, KeyError, IndexError, ValueError) as exc:
        raise HTTPException(status_code=502, detail="Dialogue provider request failed") from exc


@app.post("/v1/voice")
async def voice(payload: VoiceRequest, request: Request):
    require_key()
    throttle(request, "voice", 3.0)
    headers = {"Authorization": f"Bearer {API_KEY}", "Content-Type": "application/json"}
    body = {
        "model": TTS_MODEL,
        "voice": payload.voice or TTS_VOICE,
        "input": payload.text,
        "instructions": "Fale em português brasileiro. Voz feminina natural, acolhedora, expressiva e clara. Não traduza o texto.",
        "response_format": "wav",
    }
    try:
        async with httpx.AsyncClient(timeout=45.0) as client:
            result = await client.post(f"{API_BASE}/audio/speech", headers=headers, json=body)
            result.raise_for_status()
            if len(result.content) < 44 or len(result.content) > 8 * 1024 * 1024:
                raise ValueError("invalid audio size")
            if result.content[:4] != b"RIFF" or result.content[8:12] != b"WAVE":
                raise ValueError("provider did not return WAV audio")
            return Response(content=result.content, media_type="audio/wav")
    except (httpx.HTTPError, ValueError) as exc:
        raise HTTPException(status_code=502, detail="Voice provider request failed") from exc
