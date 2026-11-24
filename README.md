# APOCALYPSE

Projeto de jogo 3D de sobrevivência em mundo aberto, com exploração e ameaças zumbi. Motor-alvo: **Unreal Engine 5.8**.

> **Estado atual:** fundação inicial em C++ e automação do repositório. Ainda não é um jogo jogável e não contém mapa final, assets, sistemas completos de sobrevivência ou multiplayer validado.

## Requisitos
- Unreal Engine 5.8 instalado.
- Visual Studio 2022 com workload de desenvolvimento C++ para jogos.
- Para Android: SDK, NDK e JDK compatíveis com a versão instalada do Unreal, conforme a documentação e o SetupAndroid do UE 5.8.

## Abrir
Abra APOCALYPSE.uproject pelo Unreal Engine 5.8 e permita a geração dos arquivos de projeto C++ quando solicitado.

## Estrutura
- Source/APOCALYPSE: módulo principal C++.
- Config: configurações iniciais.
- .github/workflows: validação estática e empacotamento Android.

## GitHub Actions
O workflow validate.yml verifica a estrutura sem exigir Unreal instalado.

O workflow android-build.yml exige um runner **self-hosted Windows x64** administrado pelo dono do repositório, com Unreal Engine 5.8 e toolchain Android instalados. Configure os labels UE58 e Android e a variável UE_ROOT no serviço do runner. O workflow não instala nem licencia o Unreal automaticamente.

Por segurança, o empacotamento não roda em eventos de Pull Request de forks.

## Próximas etapas
1. Abrir e compilar o projeto no Unreal Engine 5.8.
2. Criar o mapa inicial e o controlador de personagem.
3. Implementar inventário, coleta, necessidades e salvamento.
4. Desenvolver IA de inimigos e otimização do mundo.
5. Projetar e testar multiplayer.
6. Validar o empacotamento Android em runner compatível.
