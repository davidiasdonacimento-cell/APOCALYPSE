# VERITY — companheira generativa híbrida e gratuita

## Arquitetura
A VERITY recebe eventos estruturados do próprio APOCALYPSE, combina esses dados com o estado atual do jogador e com a memória recente, solicita uma fala ao modelo generativo opcional e reproduz a resposta pelo Android TextToSpeech.

### Eventos observados automaticamente
- Ameaça reconhecida ou área liberada pelo sistema de percepção da companheira.
- Queda de saúde, fome ou sede abaixo de faixas configuradas.
- Atualização do inventário e mudança de equipamentos.
- Comentários espontâneos durante a exploração, com dados atuais do mapa e do jogador.

A IA só recebe fatos que o jogo fornece. Ela não lê a tela nem adivinha acontecimentos. Eventos repetidos são limitados para evitar falas excessivas; ameaça detectada e saúde crítica recebem prioridade. Se uma resposta já estiver sendo gerada, o evento mais recente fica em fila.

## Modo gratuito
- Offline: falas contextuais internas, personalidade, memória da sessão e Android TTS; não exige internet nem chave.
- Generativo local opcional: endpoint compatível com OpenAI Chat Completions executado em um computador próprio na mesma rede local. O jogo não envia chave de provedor. Se o endpoint falhar, a VERITY usa o modo offline.
- Voz Android: usa o mecanismo TextToSpeech instalado no dispositivo, seleciona pt-BR e tenta pt-PT como alternativa. A voz feminina depende das vozes instaladas no aparelho.

O modo generativo local requer que outro dispositivo execute o modelo. Não é uma API em nuvem gratuita ilimitada. Serviços de nuvem podem impor limites, exigir conta, alterar preços e ter requisitos de idade próprios.

## Configurar o modelo local opcional
1. Execute um servidor de inferência compatível com OpenAI em um computador seu, na mesma rede Wi-Fi do Android.
2. Instale nesse computador um modelo de linguagem compatível com a memória e a capacidade disponíveis.
3. Defina LocalAIBaseUrl no config/Blueprint da VERITY com o endereço LAN e a porta do servidor.
4. Defina LocalAIModel com o identificador exato do modelo instalado.
5. Mantenha o endpoint restrito à rede privada. Não exponha o servidor do modelo à internet.

Deixe LocalAIBaseUrl vazio para usar somente o modo offline.

## Limites e integração pendente
- O projeto não inclui ainda uma classe final de zumbi. Os inimigos precisam implementar VerityHostileTarget e conectar o callback abstrato ao sistema de vida do inimigo.
- O mapa precisa de NavMesh gerada.
- Um Skeletal Mesh e Animation Blueprint da VERITY precisam ser atribuídos no Blueprint filho.
- O chat manual continua disponível por texto. Microfone e reconhecimento de fala não fazem parte desta implementação.
- A memória é limitada à sessão e não persiste após reiniciar o jogo.
- Eventos de interação específicos e localização semântica de áreas ainda dependem dos sistemas correspondentes no jogo.

## Privacidade e custos
No modo offline, nenhum dado sai do dispositivo. No modo generativo local, somente o estado resumido do jogo e a memória recente são enviados ao endpoint LAN configurado. Nenhuma chave de API é embutida no APK. Não há servidor hospedado ou serviço pago obrigatório.

## Validação
A validação do GitHub verifica estrutura e contratos do código. Ela não substitui compilação C++ no Unreal Engine 5.8, testes no Editor, teste de rede local, teste de voz em Android físico ou geração de APK.