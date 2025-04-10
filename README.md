# FPS_Game

Um jogo de tiro em primeira pessoa simples, feito com OpenGL, GLFW e GLEW.

## 📦 Requisitos

Certifique-se de estar usando **WSL 2 com Ubuntu 20.04 ou superior**, e que sua distribuição tem acesso a interface gráfica via WSLg (recomendado) ou usando um servidor X (como o VcXsrv).

### Dependências

Execute o seguinte comando para instalar tudo:

```bash
sudo apt update && sudo apt install -y \
  g++ cmake make git \
  libglfw3-dev libglew-dev libglm-dev libassimp-dev libsoil-dev \
  libxinerama-dev libxcursor-dev libxi-dev libxrandr-dev libx11-dev
```

## 💻 Como rodar

1. Clone o projeto:

```bash
git clone https://github.com/MrPinabutter/FPS_Game.git
cd FPS_Game
```

2. Compile o jogo:

```bash
g++ -std=c++11 -o FPS_Game game.cpp \
  -lglfw -lGLEW -lGL -lGLU -lassimp -lSOIL
```

> ⚠️ Caso você receba erros de arquivos `.cpp` ou `.h` ausentes, verifique se todos os arquivos do projeto estão corretamente clonados.

3. Execute o jogo:

```bash
./FPS_Game
```

> 🧠 Dica: se estiver usando servidor X (como VcXsrv), exporte a variável DISPLAY:

```bash
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
```

## 🕹️ Controles

| Tecla        | Ação                      |
| ------------ | ------------------------- |
| `W A S D`    | Movimento                 |
| `Espaço`     | Pular                     |
| `Shift`      | Correr                    |
| `R`          | Recuperar balas           |
| `F`          | Ligar/Desligar a lanterna |
| `Mouse`      | Olhar                     |
| `Clique Esq` | Atirar                    |
| `ESC`        | Fechar o jogo             |

## 📂 Recursos

- **Modelos 3D**: `res/models/*.obj`
- **Texturas**: `res/images/*.png`
- **Shaders**: `res/shaders/*.vs` e `.frag`

## 🛠️ Notas Técnicas

- A lógica de movimentação agora utiliza **deltaTime**, o que torna a jogabilidade consistente em diferentes máquinas.
- Certifique-se de que os arquivos `.obj`, `.png` e shaders estejam nos diretórios corretos conforme usados no código.
- Caso a janela não abra, verifique se seu sistema está com suporte a gráficos via WSLg (Windows 11+) ou se o servidor X está rodando.

## 📸 Screenshots

_(Adicione aqui capturas de tela do jogo em execução se quiser)_

## 🧑‍💻 Autor

Feito por [MrPinabutter](https://github.com/MrPinabutter)  
Modificações e melhorias por [Você 😎]
