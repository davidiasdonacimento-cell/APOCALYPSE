package com.epicgames.unreal;

import android.app.Activity;
import android.speech.tts.TextToSpeech;
import java.util.Locale;
import java.util.UUID;

public final class VerityTTS {
    private static TextToSpeech engine;
    private static boolean ready = false;
    private VerityTTS() {}

    public static void initialize(final Activity activity) {
        if (activity == null || engine != null) return;
        activity.runOnUiThread(() -> {
            if (engine != null) return;
            engine = new TextToSpeech(activity.getApplicationContext(), status -> {
                ready = status == TextToSpeech.SUCCESS;
                if (ready) {
                    int result = engine.setLanguage(new Locale("pt", "BR"));
                    if (result == TextToSpeech.LANG_MISSING_DATA || result == TextToSpeech.LANG_NOT_SUPPORTED)
                        result = engine.setLanguage(new Locale("pt", "PT"));
                    ready = result != TextToSpeech.LANG_MISSING_DATA && result != TextToSpeech.LANG_NOT_SUPPORTED;
                    engine.setSpeechRate(1.0f);
                    engine.setPitch(1.04f);
                }
            });
        });
    }

    public static void speak(final String text, final float volume) {
        if (text == null || text.trim().isEmpty() || !ready || engine == null) return;
        engine.setVolume(Math.max(0.0f, Math.min(1.0f, volume)));
        engine.speak(text, TextToSpeech.QUEUE_FLUSH, null, "verity-" + UUID.randomUUID());
    }

    public static void stop() {
        if (engine != null) engine.stop();
    }
}
