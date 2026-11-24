# VERITY — companheira híbrida gratuita

## Modelo de funcionamento
A VERITY não depende de uma API paga, assinatura, conta de provedor ou servidor hospedado.

- **Modo offline padrão:** regras contextuais no jogo, memória limitada à sessão, personalidade, reações aos eventos e falas em português brasileiro.
- **Modo generativo opcional:** endpoint compatível com OpenAI Chat Completions hospedado pelo próprio usuário na rede local (por exemplo, um servidor Ollama em um PC). Não envia chave de API nem exige provedor comercial. Deixe `LocalAIBaseUrl` vazio para não usar rede.
- **Voz Android:** usa o mecanismo TextToSpeech instalado no próprio Android, selecionando pt-BR e usando o volume do sistema. A disponibilidade e a qualidade dependem dos pacotes de voz instalados no aparelho.
- **Gameplay:** navegação, detecção de ameaças e callbacks de dano continuam locais no Unreal.

## Configurar o modelo local opcional
1. Instale e execute um servidor de inferência local em um computador seu, na mesma rede Wi-Fi do dispositivo Android.
2. Habilite a API compatível com OpenAI no servidor local e escolha um modelo pequeno adequado à memória disponível.
3. Defina `LocalAIBaseUrl` no config/Blueprint da VERITY para o endereço LAN do computador, incluindo a porta, por exemplo `http://192.168.0.20:11434/v1`.
4. Defina `LocalAIModel` com o identificador exato do modelo instalado.
5. Permita conexões HTTP locais no projeto Android se o servidor não usar HTTPS. Não exponha essa porta à internet.

Se o computador desligar, sair do Wi-Fi ou o endpoint falhar, a VERITY volta automaticamente às falas contextuais locais. Não há chave, cobrança ou tráfego para serviço de terceiros.

## Voz no Android
O módulo Java `VerityTTS` inicializa Android TextToSpeech com locale pt-BR (fallback pt-PT). O usuário precisa ter um mecanismo TTS e dados de voz compatíveis instalados nas configurações de texto para voz do Android. A voz feminina depende das vozes oferecidas pelo mecanismo instalado; o jogo não baixa nem compra uma voz.

## Requisitos de integração do jogo
- Os inimigos precisam implementar `VerityHostileTarget` e encaminhar o callback abstrato de dano ao sistema de vida do próprio inimigo.
- O mapa precisa de NavMesh gerada.
- Um Skeletal Mesh e Animation Blueprint da personagem precisam ser atribuídos no Blueprint filho de `AVerityCompanionCharacter`.
- O chat do jogador nesta versão é por texto. Microfone e reconhecimento de fala ainda não foram implementados.
- A memória é limitada à sessão atual e não persiste após reiniciar o jogo.

## Privacidade e custos
O modo offline não envia mensagens para a internet. O modo generativo opcional envia somente o contexto recente do jogo ao servidor local configurado pelo usuário. Nenhum token de provedor é embutido no APK. Não há endpoint pago padrão nem serviço Render necessário.

## Validação
GitHub Actions verifica estrutura e contratos do código. Isso não substitui compilação no Unreal Engine 5.8, testes no Editor, teste de voz em Android físico ou validação do servidor local opcional.
