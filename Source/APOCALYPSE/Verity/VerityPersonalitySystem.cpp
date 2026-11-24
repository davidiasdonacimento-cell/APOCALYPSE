#include "Verity/VerityPersonalitySystem.h"

UVerityPersonalitySystem::UVerityPersonalitySystem()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FString UVerityPersonalitySystem::BuildSystemPrompt() const
{
    FString MoodText = TEXT("amigável, curiosa e observadora");
    switch (Mood)
    {
        case EVerityMood::Alert: MoodText = TEXT("atenta e objetiva, sem alarmismo"); break;
        case EVerityMood::Focused: MoodText = TEXT("concentrada no que acontece ao redor"); break;
        case EVerityMood::Relieved: MoodText = TEXT("aliviada e bem-humorada"); break;
        case EVerityMood::Curious: MoodText = TEXT("curiosa e interessada na exploração"); break;
        default: break;
    }
    return FString::Printf(
        TEXT("Você é VERITY, uma companheira original do jogo APOCALYPSE. Fale sempre em português brasileiro natural, com voz feminina quando houver síntese de voz. Personalidade: %s. Seja breve, espontânea, gentil, inteligente e contextual. Comente somente fatos presentes no contexto fornecido; nunca invente itens, locais, inimigos ou ações. Não repita frases recentes. Não afirme ter visto algo que os dados do jogo não confirmam. Em combate, priorize alertas curtos. Você é uma personagem fictícia e não deve pedir dados pessoais do jogador."),
        *MoodText);
}

FString UVerityPersonalitySystem::GetOfflineLine(EVerityMood InMood)
{
    static const TArray<FString> Friendly = {
        TEXT("Estou aqui com você. Vamos explorar com calma."),
        TEXT("Faz um tempo que estamos caminhando. Estou observando o caminho."),
        TEXT("Se encontrar algo interessante, me avise. Também estou procurando."),
        TEXT("O silêncio por aqui é estranho... vou continuar de olho.")
    };
    static const TArray<FString> Alert = {
        TEXT("Atenção ao redor. Melhor observar antes de avançar."),
        TEXT("Fiquei em alerta. Vamos manter atenção ao ambiente."),
        TEXT("Não quero que sejamos surpreendidos. Estou verificando os arredores.")
    };
    const TArray<FString>& Lines = InMood == EVerityMood::Alert ? Alert : Friendly;
    if (Lines.IsEmpty()) return TEXT("Estou aqui.");
    int32 Index = FMath::RandRange(0, Lines.Num() - 1);
    for (int32 Attempt = 0; Attempt < 5 && RecentOfflineLineIndices.Contains(Index); ++Attempt)
        Index = FMath::RandRange(0, Lines.Num() - 1);
    RecentOfflineLineIndices.Add(Index);
    while (RecentOfflineLineIndices.Num() > 3) RecentOfflineLineIndices.RemoveAt(0);
    return Lines[Index];
}
