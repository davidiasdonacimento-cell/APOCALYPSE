# APOCALYPSE — Lobby principal e conversa VERITY

## Implementado nesta etapa
- O GameMode deixa de criar o personagem automaticamente na entrada. O primeiro widget é o lobby.
- O botão INICIAR JOGO remove o lobby e solicita o spawn do personagem no mapa atual.
- A conversa da VERITY mantém histórico local no perfil SaveGame.
- Mensagens do jogador podem ser substituídas pela ação de editar a última mensagem enviada.
- O texto pessoal é um campo separado do histórico e entra no prompt generativo.
- Memórias persistentes autorizadas podem ser guardadas separadamente.
- O endpoint generativo é opcional e compatível com OpenAI Chat Completions em rede local privada; sem endpoint, o modo offline informa claramente que não está gerando respostas livres.
- Limpar conversa é uma ação explícita do botão de limpeza.

## Limites que não devem ser confundidos com recursos concluídos
- O repositório não possui sistema de autenticação/contas. O SaveGame atual é um perfil local do dispositivo, não uma memória sincronizada ou isolada por conta online.
- O mapa padrão ainda é /Engine/Maps/Entry. Não existe mapa de lobby 3D próprio nem mapa de campanha incluído nesta branch. O lobby atual é UI funcional; o cenário 3D, personagem visual, animações e cosméticos exigem assets e mapas reais.
- CONTINUAR depende de um sinal de progresso. O sistema de campanha ainda não grava estado de progresso real.
- A conversa generativa depende de um servidor de modelo local configurado; não há API em nuvem gratuita e ilimitada.
- A tela de lobby ainda não integra microfone/STT nem sincronização labial.
- A voz TTS existente está no sistema de companheira da partida; a voz da conversa de lobby ainda precisa de ligação direta ao serviço Android.
- CONFIGURAÇÕES e PERSONALIZAR informam honestamente que os sistemas de opções/cosméticos ainda não existem; não simulam que estejam implementados.
- Não foi feita compilação Unreal, execução no Editor ou pacote Android nesta etapa.

## Segurança e privacidade
O texto pessoal e o histórico ficam no armazenamento local do jogo. Não inclua senhas, endereços ou informações sensíveis. A sincronização de conta requer primeiro um sistema de identidade real.
