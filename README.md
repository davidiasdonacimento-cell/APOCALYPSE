# APOCALYPSE

Jogo 3D de sobrevivência em mundo aberto, com exploração, regiões, clima dinâmico e ameaças zumbi. Motor-alvo: **Unreal Engine 5.8**.

## Estado do projeto

- Fundação C++ e automação inicial: PR #1.
- Sistemas-base do mundo aberto: PR #2 (em revisão).
- O mapa de produção, terrenos, cidades, florestas, estradas, assets artísticos e integração visual completa ainda precisam ser criados no Unreal Editor. O repositório não declara esses conteúdos como concluídos.

## Sistemas de mundo

- Relógio do mundo com ciclo de 24 horas e duração configurável.
- Estado climático regional com pesos configuráveis e seed reproduzível.
- Definições de regiões/biomas baseadas em limites 3D.
- Registro de regiões e busca pela posição do jogador.
- Controlador ambiental para sol, atmosfera, iluminação de céu e neblina.
- Registro de pontos de interesse com identificadores estáveis.
- Testes de automação Unreal para limites de região e consulta de POIs.

Consulte Docs/OPEN_WORLD_PRODUCTION.md e Docs/WORLD_CONTENT_PIPELINE.md antes de criar o mapa de produção.

## Requisitos de desenvolvimento

- Unreal Engine 5.8 instalado.
- Visual Studio 2022 com workload de desenvolvimento C++ para jogos.
- Para Android: SDK, NDK e JDK compatíveis com a versão instalada do Unreal, conforme a documentação e o SetupAndroid do UE 5.8.

## Abrir o projeto

Abra APOCALYPSE.uproject pelo Unreal Engine 5.8 e permita a geração dos arquivos de projeto C++ quando solicitado.

## GitHub Actions

- validate.yml verifica o descritor, arquivos obrigatórios e integridade estrutural do código-fonte.
- android-build.yml exige um runner self-hosted Windows x64, administrado pelo dono do repositório, com Unreal Engine 5.8 e toolchain Android instalados.
- Configure os labels UE58 e Android e a variável UE_ROOT no serviço do runner.
- O workflow não instala nem licencia Unreal automaticamente. O empacotamento Android não é executado em eventos de Pull Request de forks.

## Próximas etapas

1. Criar e revisar o mapa World Partition L_World no Unreal Editor.
2. Construir terrenos, estradas, florestas, cidades e pontos de interesse com assets de produção.
3. Integrar efeitos visuais e sonoros de clima e transições de ambiente.
4. Executar os testes de automação dentro do Unreal Engine 5.8.
5. Medir streaming, memória e desempenho em Android real.
