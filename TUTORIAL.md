# Tutorial de instalação — Martins Unreal Claude (UE 5.8)

Passo a passo para instalar o plugin em um projeto Unreal (ex.: CITY MARTINS).

> **Status:** o plugin foi adaptado para a UE 5.8 e conferido estaticamente contra os headers da 5.8.4, mas **ainda não foi compilado**. Se a compilação falhar, guarde o log (`Saved/Logs` ou a saída do build) para corrigirmos.

---

## 1. Pré-requisitos

| Item | Detalhe |
|------|---------|
| Unreal Engine | 5.8 (instalada ou compilada do código-fonte) |
| Visual Studio 2022 | Workload **Desenvolvimento de jogos com C++** e Windows SDK |
| Node.js | 18 ou superior (usado pela ponte MCP) |
| Git | Com suporte a submódulos |
| Claude Code CLI | Instalado e autenticado (passo 2) |

## 2. Instalar e autenticar o Claude Code

```bash
npm install -g @anthropic-ai/claude-code
claude auth login
claude --version
```

O plugin usa a autenticação do Claude Code; não precisa de chave de API separada.

## 3. Obter o plugin

Clone **fora** do projeto do jogo, por exemplo em `C:\Projetos`:

```bash
git clone --recurse-submodules https://github.com/SilvaneiMartins/MartinsUrealClaude.git
```

Se já clonou sem `--recurse-submodules`:

```bash
cd MartinsUrealClaude
git submodule update --init
```

Confira se a pasta `UnrealClaude/Resources/mcp-bridge` **não está vazia**. Se estiver, o submódulo não foi baixado; clone a ponte manualmente:

```bash
git clone https://github.com/SilvaneiMartins/ue5-mcp-bridge-martins.git UnrealClaude/Resources/mcp-bridge
```

## 4. Copiar o plugin para o projeto

> **Atenção (causa comum de erro):** não clone o repositório direto dentro de `Plugins/`. Copie **somente** a subpasta `UnrealClaude`, a que contém o arquivo `UnrealClaude.uplugin`.

O resultado final precisa ser exatamente:

```
CityMartins/
├── CityMartins.uproject
└── Plugins/
    └── UnrealClaude/
        ├── UnrealClaude.uplugin      <-- tem que estar aqui, sem pasta extra no meio
        ├── Source/
        ├── Resources/
        └── Config/
```

Em PowerShell, a partir da pasta do repositório clonado:

```powershell
$dest = "C:\Projetos\CityMartins\Plugins\MartinsUnrealClaude"
New-Item -ItemType Directory -Force "C:\Projetos\CityMartins\Plugins" | Out-Null
Copy-Item ".\MartinsUnrealClaude" $dest -Recurse -Force
```

Se o caminho ficar `Plugins/UnrealClaude/UnrealClaude/UnrealClaude.uplugin` (pasta repetida), está errado: mova o conteúdo um nível acima.

## 5. Instalar as dependências da ponte MCP

```bash
cd C:\Projetos\CityMartins\Plugins\UnrealClaude\Resources\mcp-bridge
npm install
```

Sem esse passo, o Claude não enxerga as tools de Blueprint e do editor.

## 6. Habilitar o plugin e gerar os arquivos do projeto

1. Feche o Unreal Editor e a Visual Studio.
2. Clique com o botão direito em `CityMartins.uproject` → **Generate Visual Studio project files**.
3. Confirme que o `.uproject` tem o plugin habilitado (o `.uplugin` já vem com `EnabledByDefault: true`; se quiser forçar):

```json
"Plugins": [
    { "Name": "UnrealClaude", "Enabled": true }
]
```

O plugin depende de **EnhancedInput** (obrigatório) e **EditorScriptingUtilities** (opcional). A Unreal os habilita automaticamente.

## 7. Compilar

**Opção A — pelo projeto (recomendada para desenvolvimento):**

1. Abra `CityMartins.sln` na Visual Studio.
2. Configuração **Development Editor**, plataforma **Win64**.
3. **Build** do projeto `CityMartins`.

**Opção B — empacotar só o plugin:**

```bat
"C:\Caminho\Para\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
  -Plugin="C:\Projetos\MARTINSUNREALCLAUDE\MartinsUrealClaude\UnrealClaude\UnrealClaude.uplugin" ^
  -Package="C:\Projetos\Build\UnrealClaude" -TargetPlatforms=Win64
```

Depois copie o resultado empacotado para `CityMartins/Plugins/UnrealClaude`.

Se a engine foi compilada do código-fonte, abra o `.sln` do **projeto** a partir da engine compilada (clique direito no `.uproject` → *Switch Unreal Engine version*).

## 8. Fazer o plugin aparecer na Unreal

Para aparecer em **Edit → Plugins**, a Unreal precisa encontrar um `.uplugin` válido e o módulo C++ precisa estar compilado.

**Requisitos**

- O projeto precisa ser **C++**. Um projeto só de Blueprint não compila o plugin. Para converter, crie qualquer classe em *Tools → New C++ Class*.
- A versão da engine do `.uproject` deve ser a **5.8**. Com `EngineVersion: 5.8.0` no plugin, a Unreal aceita qualquer 5.8.x. Se a engine foi compilada do código-fonte, associe o projeto a ela (clique direito no `.uproject` → *Switch Unreal Engine version*).

**Passos**

1. Confirme que o arquivo está em `CityMartins/Plugins/UnrealClaude/UnrealClaude.uplugin` (passo 4).
2. Gere os arquivos do projeto (passo 6) e compile (passo 7). Isso cria `Plugins/UnrealClaude/Binaries/`.
3. Se você abrir o `.uproject` sem compilar, a Unreal pergunta *"The following modules are missing or built with a different engine version... Would you like to rebuild?"*. Responda **Sim**.
4. No editor, vá em **Edit → Plugins** e procure por **"Martins Unreal Claude"**:
   - Ele fica na categoria **Editor**. A busca às vezes não encontra, então role a lista da categoria.
   - A caixa **Enabled** já vem marcada (`EnabledByDefault: true`). Se estiver desmarcada, marque e reinicie o editor.
5. Depois de carregado, o painel fica em **Tools → Claude Assistant**.

**Se o plugin não aparecer**

| Verifique | Como |
|-----------|------|
| Pasta aninhada demais | Confirme que não existe `Plugins/UnrealClaude/UnrealClaude/...` |
| Erro de leitura do `.uplugin` | Veja o **Output Log** ou `Saved/Logs/CityMartins.log` e procure por `UnrealClaude` ou `uplugin`. Uma vírgula ou aspas erradas fazem o plugin sumir da lista |
| Falha ao compilar | O plugin só carrega depois que o módulo compila. Guarde o log do build |
| Cache antigo | Apague `Intermediate/` e `Binaries/` do projeto e do plugin, gere os arquivos do projeto de novo e recompile |

**Como confirmar que carregou:** no Output Log, filtre por `LogUnrealClaude`. Quando o MCP sobe, aparece `MCP Server started on http://localhost:3000`.

---

## 9. Abrir e testar

1. Abra o projeto no Unreal Editor.
2. Menu **Tools → Claude Assistant**.
3. Envie uma mensagem simples, por exemplo: `Olá, você está me vendo?`
4. Teste o servidor MCP (com o editor aberto):

```bash
curl http://localhost:3000/mcp/status
```

Deve retornar um JSON com informações do projeto. No **Output Log**, procure por `LogUnrealClaude`:
- `MCP Server started on http://localhost:3000`
- `Registered X MCP tools`

## 10. (Opcional) Usar o Claude Code fora do editor

Na raiz do projeto do jogo, crie ou edite `.mcp.json`:

```json
{
  "mcpServers": {
    "unrealclaude": {
      "command": "node",
      "args": ["Plugins/UnrealClaude/Resources/mcp-bridge/index.js"],
      "env": { "UNREAL_MCP_URL": "http://localhost:3000" }
    }
  }
}
```

Reinicie o Claude Code e mantenha o editor aberto, porque o servidor MCP vive dentro dele.

## 11. (Opcional) Contexto do seu jogo

Crie um `CLAUDE.md` na raiz do projeto com arquitetura e padrões de código do CITY MARTINS. O plugin o inclui automaticamente no contexto. Há um modelo em `UnrealClaude/Resources/CLAUDE.md.example`.

---

## Configurações úteis

Em **Project Settings → Plugins → Unreal Claude**:

- **Auto-approve script execution** (padrão: desligado). Liga a execução de scripts sem pedir permissão. Use apenas em máquina e projeto confiáveis.

As tools de script e console (`execute_script`, `run_console_command`, …) ficam ocultas por padrão na ponte MCP. Veja o README para habilitá-las.

---

## Solução de problemas

| Sintoma | Causa provável | O que fazer |
|---------|----------------|-------------|
| O projeto deixa de compilar depois de adicionar o plugin | Repositório inteiro clonado em `Plugins/`, ou pasta `UnrealClaude` duplicada | Refazer o passo 4 e gerar os arquivos do projeto de novo |
| `Plugin 'UnrealClaude' failed to load` / erro de JSON | `.uplugin` inválido | Garantir que está na versão corrigida deste repositório |
| Erro de `#include` ao compilar | Cópia antiga do plugin | Copiar de novo a partir deste repositório |
| `Claude CLI not found` | CLI fora do PATH | `claude --version`, `where claude`, reiniciar o editor |
| `Authentication required` | Sem login | `claude auth login` |
| Tools MCP não aparecem | `npm install` não foi feito, ou submódulo vazio | Passos 3 e 5, e reiniciar o editor |
| Porta 3000 ocupada | Outro serviço usando a porta | Liberar a porta 3000 |
| Respostas muito lentas ou travadas | Muitos plugins globais do Claude Code, ou arquivos no OneDrive/Dropbox | Desabilitar plugins globais e manter o projeto fora de pastas sincronizadas |

## Atualizando para novas versões da Unreal

O plugin lê a versão da engine em tempo de compilação, então o texto do prompt se ajusta sozinho. Para uma versão nova:

1. Altere `EngineVersion` em `UnrealClaude.uplugin`.
2. Recompile e corrija eventuais erros de API.
3. Gere os arquivos do projeto novamente.
