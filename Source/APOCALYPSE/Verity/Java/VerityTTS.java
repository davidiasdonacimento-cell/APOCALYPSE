package com.epicgames.unreal;

import android.app.Activity;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import java.util.Locale;
import java.util.UUID;

public final class VerityTTS {
    private static TextToSpeech engine;
    private VerityTTS() {}

    public static void initialize(final Activity activity) {
        if (activity == null || engine != null) return;
        activity.runOnUiThread(() -> {
            if (engine != null) return;
            engine = new TextToSpeech(activity.getApplicationContext(), status -> {
                if (status == TextToSpeech.SUCCESS) {
                    int result = engine.setLanguage(new Locale("pt", "BR"));
                    if (result == TextToSpeech.LANG_MISSING_DATA || result == TextToSpeech.LANG_NOT_SUPPORTED)
                        result = engine.setLanguage(new Locale("pt", "PT"));
                    engine.setSpeechRate(1.0f);
                    engine.setPitch(1.04f);
                }
            });
        });
    }

    public static void speak(final String text, final float volume) {
        if (text == null || text.trim().isEmpty() || engine == null) return;
        Bundle params = new Bundle();
        params.putFloat(TextToSpeech.Engine.KEY_PARAM_VOLUME, Math.max(0.0f, Math.min(1.0f, volume)));
        engine.speak(text, TextToSpeech.QUEUE_FLUSH, params, "verity-" + UUID.randomUUID());
    }

    public static void stop() {
        if (engine != null) engine.stop();
    }
}
