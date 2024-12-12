#define STB_IMAGE_IMPLEMENTATION

#include <GLUT/glut.h>
#include "stb_image.h"
#include <cmath>

const int N = 80;

GLuint idsTextura[N];
float rotacaoBase = 0.0;
float rotacaoBraco = 0.0;
float rotacaoPerna = 0.0;
float rotacaoCabeca = 0.0;
float direcao = 1.0;

float angulo = 0.0;

float posicaoX = 0.0f; // Posição no eixo X
float posicaoZ = 0.0f; // Posição no eixo Z
float velocidade = 0.1f; // Velocidade do movimento

float cameraAngle = 0.0;
float cameraDistance = 20.0;

int mouseX, mouseY;
bool mousePressed = false;

int sombraTipo = 1;

float blocoX = 5.0f;  // Posição X do bloco
float blocoZ = 5.0f;  // Posição Z do bloco
float blocoY = 0.0f; // Altura inicial do bloco (no chão)
bool blocoExiste = true;  // Indica se o bloco ainda existe

bool blocoSeguindo = false; // Indica se o bloco está seguindo o personagem
float distanciaSegurar = 5.0f; // Distância máxima para pegar o bloco

bool bracoLevantado = false; // Indica se o braço está levantado
float rotacaoBracoSegurando = 90.0f; // Ângulo do braço ao segurar o bloco
float rotacaoBracoInicial = 0.0f; // Ângulo inicial do braço

//GLfloat luzAmbiente[] = {1.5, 0.5, 1.5, 1.0}; // Luz rosa
 GLfloat luzAmbiente[] = {3, 3, 3, 1.0};   // luz suave
 GLfloat luzDifusa[] = {1.0, 1.0, 1.0, 1.0}; // luz branca
 GLfloat luzPosicao[] = {0.9, 0.9, 0.5, 1.0}; // luz pontual

void configuraIluminacao()
{

 glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
 glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
 glLightfv(GL_LIGHT0, GL_POSITION, luzPosicao);
}

void desenhaBloco(float x, float y, float z) {
    if (!blocoExiste) return;  // Não desenha se o bloco foi "removido"

    glPushMatrix();
    glTranslatef(x, y, z);

    // Face frontal
    glBindTexture(GL_TEXTURE_2D, idsTextura[13]);  // Textura do bloco (índice 13)
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    glEnd();

    // Outras faces do cubo
    glBindTexture(GL_TEXTURE_2D, idsTextura[13]); // Usa a mesma textura para todas as faces
    glBegin(GL_QUADS);

    // Traseira
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, -1.0);

    // Superior
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);

    // Inferior
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0);

    // Laterais
    // Direita
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.0, 1.0);

    // Esquerda
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0);

    glEnd();
    glPopMatrix();
}

bool verificaColisao(float novoX, float novoZ) {
    if (!blocoExiste) return false; // Se o bloco não existe, não há colisão

    float blocoTamanho = 4.0f; // Tamanho do bloco (1 unidade para cada lado)
    
    // Verifica se o Steve está dentro dos limites do bloco
    return (novoX > blocoX - blocoTamanho / 2 && novoX < blocoX + blocoTamanho / 2 &&
            novoZ > blocoZ - blocoTamanho / 2 && novoZ < blocoZ + blocoTamanho / 2);
}


void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            mouseX = x;
            mouseY = y;
        } else if (state == GLUT_UP) {
            mousePressed = false;
        }
    }
    if (button == 3) { // Scroll up
        cameraDistance -= 1.0f;
        if (cameraDistance < 5.0f) cameraDistance = 5.0f; // Limite mínimo
    } else if (button == 4) { // Scroll down
        cameraDistance += 1.0f;
        if (cameraDistance > 50.0f) cameraDistance = 50.0f; // Limite máximo
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    if (mousePressed) {
        float deltaX = (x - mouseX) * 0.5f; // Ajuste para sensibilidade
        float deltaY = (y - mouseY) * 0.5f;

        cameraAngle += deltaX * 0.01f;
        if (cameraAngle > 360.0f) cameraAngle -= 360.0f;
        else if (cameraAngle < 0.0f) cameraAngle += 360.0f;

        mouseX = x;
        mouseY = y;

        glutPostRedisplay();
    }
}

void carregarTextura(const char* nomeArquivo, int indice) {
    int largura, altura, canais;
    unsigned char* dados = stbi_load(nomeArquivo, &largura, &altura, &canais, 0);

    if (!dados) {
        printf("Erro ao carregar textura: %s\n", nomeArquivo);
        exit(1);
    }

    glGenTextures(1, &idsTextura[indice]);
    glBindTexture(GL_TEXTURE_2D, idsTextura[indice]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, (canais == 4) ? GL_RGBA : GL_RGB,
                 largura, altura, 0, (canais == 4) ? GL_RGBA : GL_RGB,
                 GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);
}

void inicializa() {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    // Habilita a iluminação
    glEnable(GL_LIGHTING);

    // Ativa a luz 0
    glEnable(GL_LIGHT0);

    carregarTextura("texturas/cabeca.jpg", 0);
    carregarTextura("texturas/braco.png", 1);
    carregarTextura("texturas/corpo.png", 2);
    carregarTextura("texturas/perna.png", 3);
    carregarTextura("texturas/cabecalado.png", 4);
    carregarTextura("texturas/cabecacima.png", 5);
    carregarTextura("texturas/cabecatras.png", 6);
    carregarTextura("texturas/corpolados.png", 7);
    carregarTextura("texturas/mao.png", 8);
    carregarTextura("texturas/bracolado.png", 9);
    carregarTextura("texturas/pe.png", 10);
    carregarTextura("texturas/quadril.png", 11);
    carregarTextura("texturas/pernalado.png", 12);
    carregarTextura("texturas/stone.jpeg", 13);

}

void desenhaCabeca(float largura, float altura, float profundidade) {
    glPushMatrix();

    // Face frontal - Rosto
     glRotatef(rotacaoCabeca, 0.0, 1.0, 0.0); // Rotação da cabeça no eixo Y

    // Face frontal - Rosto
    glBindTexture(GL_TEXTURE_2D, idsTextura[0]);  // Usa a textura do rosto
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-largura, -altura, profundidade);
    glTexCoord2f(-1.0, 0.0); glVertex3f(largura, -altura, profundidade);
    glTexCoord2f(-1.0, -1.0); glVertex3f(largura, altura, profundidade);
    glTexCoord2f(0.0, -1.0); glVertex3f(-largura, altura, profundidade);
    glEnd();

    // Face traseira
    glBindTexture(GL_TEXTURE_2D, idsTextura[6]);  // Textura para a parte de trás
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-largura, -altura, -profundidade);
    glTexCoord2f(-1.0, 0.0); glVertex3f(largura, -altura, -profundidade);
    glTexCoord2f(-1.0, -1.0); glVertex3f(largura, altura, -profundidade);
    glTexCoord2f(0.0, -1.0); glVertex3f(-largura, altura, -profundidade);
    glEnd();

    // Face superior
    glBindTexture(GL_TEXTURE_2D, idsTextura[5]);  // Textura para o topo
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-largura, altura, -profundidade);
    glTexCoord2f(1.0, 0.0); glVertex3f(largura, altura, -profundidade);
    glTexCoord2f(1.0, 1.0); glVertex3f(largura, altura, profundidade);
    glTexCoord2f(0.0, 1.0); glVertex3f(-largura, altura, profundidade);
    glEnd();

    // Face inferior
    glBindTexture(GL_TEXTURE_2D, idsTextura[4]);  // Textura para a base
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-largura, -altura, -profundidade);
    glTexCoord2f(1.0, 0.0); glVertex3f(largura, -altura, -profundidade);
    glTexCoord2f(1.0, 1.0); glVertex3f(largura, -altura, profundidade);
    glTexCoord2f(0.0, 1.0); glVertex3f(-largura, -altura, profundidade);
    glEnd();

    // Face direita
    glBindTexture(GL_TEXTURE_2D, idsTextura[4]);  // Textura para os lados
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(largura, -altura, -profundidade);
    glTexCoord2f(1.0, 0.0); glVertex3f(largura, altura, -profundidade);
    glTexCoord2f(1.0, 1.0); glVertex3f(largura, altura, profundidade);
    glTexCoord2f(0.0, 1.0); glVertex3f(largura, -altura, profundidade);
    glEnd();

    // Face esquerda
    glBindTexture(GL_TEXTURE_2D, idsTextura[4]);  // Textura para os lados
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-largura, -altura, -profundidade);
    glTexCoord2f(1.0, 0.0); glVertex3f(-largura, altura, -profundidade);
    glTexCoord2f(1.0, 1.0); glVertex3f(-largura, altura, profundidade);
    glTexCoord2f(0.0, 1.0); glVertex3f(-largura, -altura, profundidade);
    glEnd();

    glPopMatrix();

}

void desenhaTronco() {
    // Face frontal
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]); // Textura do tronco frontal
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.5, 0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.5, 0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.5, 0.5);
    glEnd();

    // Face traseira
    glBindTexture(GL_TEXTURE_2D, idsTextura[7]); // Textura do tronco traseiro
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.5, -0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.5, -0.5);
    glEnd();

    // Face superior
    glBindTexture(GL_TEXTURE_2D, idsTextura[7]); // Textura do topo do tronco
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.5, 0.5);
    glEnd();

    // Face inferior
    glBindTexture(GL_TEXTURE_2D, idsTextura[8]); // Textura da base do tronco
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.5, 0.5);
    glEnd();

    // Face direita
    glBindTexture(GL_TEXTURE_2D, idsTextura[7]); // Textura do lado direito do tronco
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, -1.5, 0.5);
    glEnd();

    // Face esquerda
    glBindTexture(GL_TEXTURE_2D, idsTextura[7]); // Textura do lado esquerdo do tronco
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, 1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.5, 0.5);
    glEnd();
}


void desenhaPerna(float lado) {
    glPushMatrix();
    glTranslatef(lado * 0.5, -1.5, 0.0); // Posiciona o ponto de articulação no quadril
    glRotatef(rotacaoPerna * lado, 1.0, 0.0, 0.0);  // Rotação no eixo X (para subir e descer)
    glTranslatef(0.0, -1.5, 0.0); // Move a perna para baixo a partir do quadril

    // Face frontal
    glBindTexture(GL_TEXTURE_2D, idsTextura[3]); // Textura da face frontal da perna
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, 0.5);
    glTexCoord2f(-1.0, 0.0); glVertex3f(0.5, -1.5, 0.5);
    glTexCoord2f(-1.0, -1.0); glVertex3f(0.5, 1.5, 0.5);
    glTexCoord2f(0.0, -1.0); glVertex3f(-0.5, 1.5, 0.5);
    glEnd();

    // Face traseira
    glBindTexture(GL_TEXTURE_2D, idsTextura[3]); // Textura da face traseira da perna
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, -0.5);
    glTexCoord2f(-1.0, 0.0); glVertex3f(0.5, -1.5, -0.5);
    glTexCoord2f(-1.0, -1.0); glVertex3f(0.5, 1.5, -0.5);
    glTexCoord2f(0.0, -1.0); glVertex3f(-0.5, 1.5, -0.5);
    glEnd();

    // Face superior
    glBindTexture(GL_TEXTURE_2D, idsTextura[11]); // Textura do topo da perna
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 1.5, 0.5);
    glEnd();

    // Face inferior
    glBindTexture(GL_TEXTURE_2D, idsTextura[10]); // Textura da base da perna
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -1.5, 0.5);
    glEnd();

    // Face direita
    glBindTexture(GL_TEXTURE_2D, idsTextura[12]); // Textura do lado direito da perna
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -1.5, -0.5);
    glTexCoord2f(-1.0, 0.0); glVertex3f(0.5, 1.5, -0.5);
    glTexCoord2f(-1.0, -1.0); glVertex3f(0.5, 1.5, 0.5);
    glTexCoord2f(0.0, -1.0); glVertex3f(0.5, -1.5, 0.5);
    glEnd();

    // Face esquerda
    glBindTexture(GL_TEXTURE_2D, idsTextura[12]); // Textura do lado esquerdo da perna
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, -0.5);
    glTexCoord2f(-1.0, 0.0); glVertex3f(-0.5, 1.5, -0.5);
    glTexCoord2f(-1.0, -1.0); glVertex3f(-0.5, 1.5, 0.5);
    glTexCoord2f(0.0, -1.0); glVertex3f(-0.5, -1.5, 0.5);
    glEnd();

    glPopMatrix();
}


void desenhaBraco(float lado) {
    glPushMatrix();
    glTranslatef(lado * 1.5, 1.5, 0.0); // Posiciona o ponto de articulação no ombro
    if (bracoLevantado) {
        glRotatef(rotacaoBracoSegurando * -1, 1.0, 0.0, 0.0);  // Rotação fixa ao segurar o bloco
    } else {
        glRotatef(rotacaoBraco * lado, 1.0, 0.0, 0.0);  // Rotação dinâmica para movimento
    }    
    glTranslatef(0.0, -1.5, 0.0); // Move o braço para baixo a partir do ombro

    // Face frontal
    glBindTexture(GL_TEXTURE_2D, idsTextura[1]); // Textura da face frontal do braço
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, 0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -1.5, 0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 1.5, 0.5);
    glEnd();

    // Face traseira
    glBindTexture(GL_TEXTURE_2D, idsTextura[1]); // Textura da face traseira do braço
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 1.5, -0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 1.5, -0.5);
    glEnd();

    // Face superior
    glBindTexture(GL_TEXTURE_2D, idsTextura[7]); // Textura do topo do braço
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, 1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, 1.5, 0.5);
    glEnd();

    // Face inferior
    glBindTexture(GL_TEXTURE_2D, idsTextura[8]); // Textura da base do braço
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, -0.5);
    glTexCoord2f(1.0, 0.0); glVertex3f(0.5, -1.5, -0.5);
    glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -1.5, 0.5);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -1.5, 0.5);
    glEnd();

    // Face direita
    glBindTexture(GL_TEXTURE_2D, idsTextura[9]); // Textura do lado direito do braço
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.5, -1.5, -0.5);
    glTexCoord2f(-1.0, 0.0); glVertex3f(0.5, 1.5, -0.5);
    glTexCoord2f(-1.0, -1.0); glVertex3f(0.5, 1.5, 0.5);
    glTexCoord2f(0.0, -1.0); glVertex3f(0.5, -1.5, 0.5);
    glEnd();

    // Face esquerda
    glBindTexture(GL_TEXTURE_2D, idsTextura[9]); // Textura do lado esquerdo do braço
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, -1.5, -0.5);
    glTexCoord2f(-1.0, 0.0); glVertex3f(-0.5, 1.5, -0.5);
    glTexCoord2f(-1.0, -1.0); glVertex3f(-0.5, 1.5, 0.5);
    glTexCoord2f(0.0, -1.0); glVertex3f(-0.5, -1.5, 0.5);
    glEnd();

    glPopMatrix();
}

void desenhaBoneco() {
    glPushMatrix();

    // Cabeça
    glTranslatef(0.0, 4.0, 0.0);
    desenhaCabeca(1.0, 1.0, 1.0);

    // Tronco
    glTranslatef(0.0, -2.5, 0.0);
    desenhaTronco();

    // Braços (agora no tronco)
    desenhaBraco(1.0);  // Direito
    desenhaBraco(-1.0); // Esquerdo

    // Pernas
    desenhaPerna(1.0);  // Direita (afastada para o lado direito)
    desenhaPerna(-1.0); // Esquerda (afastada para o lado esquerdo)


    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float cameraX = cameraDistance * sin(cameraAngle);
    float cameraZ = cameraDistance * cos(cameraAngle);
    gluLookAt(cameraX, 10.0, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    configuraIluminacao();

    if (sombraTipo == 0)
        glShadeModel(GL_FLAT);  // sombreamento plano (flat)

    if (sombraTipo == 1)
        glShadeModel(GL_SMOOTH);  // sombreamento suave (Gouraud)

    // Atualiza a posição do bloco se ele estiver seguindo
    // Atualiza a posição do bloco
    if (blocoSeguindo) {
        float anguloRadianos = -rotacaoBase * M_PI / 180.0f; // Converte o ângulo para radianos
        blocoX = posicaoX - 3.0f * sin(anguloRadianos); // Ajusta a posição X baseado no ângulo
        blocoZ = posicaoZ + 3.0f * cos(anguloRadianos); // Ajusta a posição Z baseado no ângulo
        blocoY = 3.0f; // Eleva o bloco enquanto ele está sendo segurado
    } else {
        blocoY = 0.0f; // Bloco no chão
    }

    desenhaBloco(blocoX, blocoY, blocoZ); // Desenha o bloco com a altura dinâmica

    glPushMatrix();
    glTranslatef(posicaoX, 0.0, posicaoZ); // Movimenta o boneco com base na posição
    glRotatef(rotacaoBase, 0.0, 1.0, 0.0);

     // Atualiza a rotação do braço
    // Atualiza a rotação do braço apenas se estiver levantado
    if (bracoLevantado) {
        rotacaoBraco = rotacaoBracoSegurando; // Braço levantado
    }

    desenhaBoneco();
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int largura, int altura) {
    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)largura / (GLfloat)altura, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void resetarConfiguracoes() {
    // Reseta as configurações do boneco
    rotacaoBase = 0.0;
    rotacaoBraco = 0.0;
    rotacaoPerna = 0.0;
    rotacaoCabeca = 0.0;
    direcao = 1.0;

    // Reseta a posição do boneco
    posicaoX = 0.0f;
    posicaoZ = 0.0f;

    // Reseta a câmera
    cameraAngle = 0.0;
    cameraDistance = 20.0;

    // Reseta a posição e estado do bloco
    blocoX = 5.0f; // Posição inicial do bloco no eixo X
    blocoZ = 5.0f; // Posição inicial do bloco no eixo Z
    blocoSeguindo = false; // Bloco deixa de seguir o personagem

    // Reseta o estado do braço
    bracoLevantado = false;
    rotacaoBraco = rotacaoBracoInicial;

    carregarTextura("texturas/stone.jpeg", 13);

    luzAmbiente[0] = 3;
    luzAmbiente[1] = 3;
    luzAmbiente[2] = 3;

    // Atualiza a tela
    glutPostRedisplay();
}


void teclado(unsigned char tecla, int x, int y) {
    float anguloRadianos = -rotacaoBase * M_PI / 180.0f;

    // Calcula o próximo movimento
    float proximoX = posicaoX;
    float proximoZ = posicaoZ;

    switch (tecla) {
        case 'w': // Move para frente
         if (!bracoLevantado) { // Somente move o braço se não estiver segurando o bloco
            if (direcao > 0) {
                if (rotacaoBraco < 70.0)
                    rotacaoBraco += 7.0;
                
            } else {
                if (rotacaoBraco > -70.0)
                    rotacaoBraco -= 7.0; 
            }
        }
        if (direcao > 0) {
            if (rotacaoPerna < 45.0)
                rotacaoPerna += 5.0;
            else
                direcao = -1.0;
        }
        else {
            if (rotacaoPerna > -45.0)
                rotacaoPerna -= 5.0;
            else
                direcao = 1.0;
        }

        proximoX -= velocidade * sin(anguloRadianos);
        proximoZ += velocidade * cos(anguloRadianos);

        break;

        case 's': // Move para trás
            if (!bracoLevantado) { // Somente move o braço se não estiver segurando o bloco
                if (direcao > 0) {
                    if (rotacaoBraco < 70.0)
                        rotacaoBraco += 7.0;
                } else {
                    if (rotacaoBraco > -70.0)
                        rotacaoBraco -= 7.0;
    
                }
            }

            if (direcao > 0) {
                if (rotacaoPerna < 45.0)
                    rotacaoPerna += 5.0;
                else
                    direcao = -1.0;
            }
            else {
                if (rotacaoPerna > -45.0)
                    rotacaoPerna -= 5.0;
                else
                    direcao = 1.0;
            }
                
            proximoX += velocidade * sin(anguloRadianos);
            proximoZ -= velocidade * cos(anguloRadianos);
            
            break;

        case 'a': // Gira para a esquerda
            rotacaoBase += 5.0;
            glutPostRedisplay();
            return;

        case 'd': // Gira para a direita
            rotacaoBase -= 5.0;
            glutPostRedisplay();
            return;

        case 'r': // Reseta as configurações
            resetarConfiguracoes();
            return;

        case 'p': // Segura ou solta o bloco
            if (!blocoSeguindo) {
                float distancia = sqrt(pow(posicaoX - blocoX, 2) + pow(posicaoZ - blocoZ, 2));
                if (distancia < distanciaSegurar) {
                    blocoSeguindo = true; // Segura o bloco
                    bracoLevantado = true; // Levanta o braço
                    blocoY = 3.0f; // Eleva o bloco
                }
            } else {
                blocoSeguindo = false; // Solta o bloco
                bracoLevantado = false; // Abaixa o braço
                blocoY = 0.0f; // Retorna o bloco ao chão
                rotacaoBraco = 0.0; // Reseta a posição do braço
            }
            glutPostRedisplay();
            return;
        case 'z': 
            if (rotacaoCabeca > -45.0)  // Limite de rotação para a esquerda
                rotacaoCabeca -= 5.0;
            break;
        case 'x': 
            if (rotacaoCabeca < 45.0)  // Limite de rotação para a direita
                rotacaoCabeca += 5.0;
            break;

        case '+': // +  
            luzAmbiente[0] += 0.1; // Incrementa componente vermelha
            luzAmbiente[1] += 0.1; // Incrementa componente verde
            luzAmbiente[2] += 0.1; // Incrementa componente azul
            glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
            break;  
        case '-': // -
            luzAmbiente[0] -= 0.1;
            luzAmbiente[1] -= 0.1;
            luzAmbiente[2] -= 0.1;
            glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
            break;
        case 27: // Encerra o programa
            exit(0);
    }

    // Verifica se há colisão com o bloco
    if (!verificaColisao(proximoX, proximoZ)) {
        posicaoX = proximoX; // Atualiza posição se não houver colisão
        posicaoZ = proximoZ;
    }

    glutPostRedisplay(); // Atualiza a tela
}


void menu(int opcao)
{
 switch (opcao)
 {
  case 1: // Diamante
    carregarTextura("texturas/diamante.jpg", 13);
    break;
  case 2: // Madeira  
    carregarTextura("texturas/madeira.png", 13);
    break;
  case 3: // Areia
    carregarTextura("texturas/areia.jpeg", 13);
    break;
  case 4: // livro
    carregarTextura("texturas/tijolo.jpeg", 13);
    break;
  case 5: // +  
    luzAmbiente[0] = 3; // Incrementa componente vermelha
    luzAmbiente[1] = 3; // Incrementa componente verde
    luzAmbiente[2] = 3; // Incrementa componente azul
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    break;  
  case 6: // -
    luzAmbiente[0] -= luzAmbiente[0];
    luzAmbiente[1] -= luzAmbiente[1];
    luzAmbiente[2] -= luzAmbiente[2];
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    break;
  case 7: // +  
    luzAmbiente[0] = 1.5; // Incrementa componente vermelha
    luzAmbiente[1] = 0.5; // Incrementa componente verde
    luzAmbiente[2] = 1.5; // Incrementa componente azul
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    break;  
  case 8: // resetar
    resetarConfiguracoes();
    break;
 } 

 glutPostRedisplay();
}

void atualiza(int valor)
{
 angulo += 1.0;

 if (angulo > 360)
   angulo = 0;

 glutPostRedisplay();
 glutTimerFunc(16, atualiza, 0);   
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Modelo Hierárquico 3D com Texturas");

    inicializa();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);

    int submenu = glutCreateMenu(menu);
    glutAddMenuEntry("Diamante", 1);
    glutAddMenuEntry("Madeira", 2);
    glutAddMenuEntry("Areia", 3);
    glutAddMenuEntry("tijolo", 4);

    int submenuIluminacao = glutCreateMenu(menu);
    glutAddMenuEntry("Ligar luz", 5);
    glutAddMenuEntry("Apagar luz", 6);

    glutCreateMenu(menu);
    glutAddSubMenu("Blocos", submenu);
    glutAddSubMenu("Iluminação", submenuIluminacao);
    glutAddMenuEntry("Luz rosa", 7);
    glutAddMenuEntry("Reset", 8);
   

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);


    glutTimerFunc(1000, atualiza, 0);   
  
    glutMainLoop();
    return 0;
}