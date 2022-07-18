// principal.cpp
// Programa principal do jogo Centipede.
//
// The MIT License (MIT)
//
// Copyright (c) 2017 João Vicente Ferreira Lima, UFSM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <chrono>
#include <list>
#include <random>
#include <iostream>
#include <functional>
#include "tela.hpp"
#include "geom.hpp"

using namespace tela;
using namespace geom;

// programa de teste das funcoes geometricas
//
// o programa desenha algumas figuras
// se o mouse passar sobre alguma figura, ela muda de cor
// (se as funcoes ptemret e ptemcirc estiverem corretas)
// pode-se arrastar as figuras com o mouse (botao esquerdo pressionado)
//
// com o botao esquerdo pressionado, as figuras com intersecao mudam de cor
// (se as funcoes de intersecao estiverem funcionando)
//
// apertando a tecla q o programa termina

struct laser_t {
  float v;       /* velocidade */
  Retangulo ret; /* figura */
};

struct tiro_t {
  bool ativo;
  float v;   /* velocidade */
  Circulo  c; /* figura */
};

/* estados para o jogo */
enum Estado { nada, fim };

// Estrutura para controlar todos os objetos e estados do Jogo Centipede
struct Jogo {
  Estado estado;             // estado do jogo
  laser_t laser;             // laser
  std::list<tiro_t> tiros;   // tiros ativos
  std::list<Retangulo> centipede;  // blocos de centopeia
  std::list<Retangulo> caindo;  //blocos caindo
  Tela tela;                    // estrutura que controla a tela
  int tecla;
  int score=0, tam_centipede;                 // ultima tecla apertada pelo usuario
  float vel_geral;
  // inicia estruturas principais do jogo
  void Dificuldade(void){
    tela.bitmap=al_load_bitmap("centipede.bmp");
    while(true){

      tela.limpa();
      Cor vermelho = {1.0, 1.0, 1.0};
      tela.cor(vermelho);
      al_draw_bitmap(tela.bitmap,0,0,0);
      al_draw_text(tela.fonte, tela.ac_cor, 250, 200, ALLEGRO_ALIGN_LEFT, "Dificuldade");
      al_draw_text(tela.fonte, tela.ac_cor, 50, 330, ALLEGRO_ALIGN_LEFT, "1 para Facil");
      al_draw_text(tela.fonte, tela.ac_cor, 250, 330, ALLEGRO_ALIGN_LEFT, "2 para Medio");
      al_draw_text(tela.fonte, tela.ac_cor, 450, 330, ALLEGRO_ALIGN_LEFT, "3 para Dificil");
      tecla = tela.tecla();
      tela.mostra();
      tela.espera(30);
      if(tecla == ALLEGRO_KEY_1){
        tam_centipede=50;
        vel_geral=5;
        break;
      }
      if(tecla == ALLEGRO_KEY_2){
        tam_centipede=60;
        vel_geral=8;
        break;
      }
      if(tecla == ALLEGRO_KEY_3){
        tam_centipede=75;
        vel_geral=10;
        break;
      }
    }
    al_destroy_bitmap(tela.bitmap);
    laser_inicia();
    estado = Estado::nada;
    centipede.clear();
    caindo.clear();
    // inicia jogo com apenas 1 retangulo
    float x=0;
    for(int i=0; i<tam_centipede; i++){

      Retangulo r1 = {{0-x, 0}, {40, 40}};
      r1.cent= al_load_bitmap("cel.bmp");
      centipede.push_back(r1);
      x+=40;
    }
    tela.bitmap=al_load_bitmap("pixel.bmp");
    score=0;
  }

  void inicia(void) {
    tela.inicia(600, 400, "Centipede!");
    Dificuldade();
  }

  // atualiza o jogo, como se fosse um passo na execução
  void atualiza(void) {
    // le ultima tecla
    tecla = tela.tecla();
    // tecla Q termina
    if( tecla== ALLEGRO_KEY_P){
      
      while(true){
        tecla = tela.tecla();
        tela.limpa();
        al_draw_bitmap(tela.bitmap,0,0,0);
        al_draw_text(tela.fonte, tela.ac_cor, 250, 100, ALLEGRO_ALIGN_LEFT, "PAUSADO");
        desenha_figuras();
        tela.mostra();
        tela.espera(30);
        if(tecla== ALLEGRO_KEY_P)
          break;
      }
    }
    auto tet= caindo.begin();
    auto ret = centipede.begin();
    if (tecla != ALLEGRO_KEY_Q  && !ptemret(laser.ret.pos, (*ret) ) && centipede.size() != 0 && !ptemret(laser.ret.pos, (*tet)) ) {
      // faz o resto
      
      move_figuras();
      tela.limpa();
      al_draw_bitmap(tela.bitmap,0,0,0);
      desenha_figuras();
      tela.mostra();
      // espera 60 ms antes de atualizar a tela
      tela.espera(30);
    } else {
      if(centipede.size()!=0){
        tela.bitmap=al_load_bitmap("perdeu.bmp");//perdeu
      }else{
        tela.bitmap=al_load_bitmap("ganhou.bmp");//ganhou
      }
      while(true){
        tecla = tela.tecla();
        tela.limpa();
        al_draw_bitmap(tela.bitmap,0,0,0);
        al_draw_text(tela.fonte, tela.ac_cor, 150, 300, ALLEGRO_ALIGN_LEFT, "0 para rejogar");
        al_draw_text(tela.fonte, tela.ac_cor, 350, 300, ALLEGRO_ALIGN_LEFT, "1 para fechar");
        al_draw_textf(tela.fonte, tela.ac_cor,250, 200, ALLEGRO_ALIGN_LEFT, "SCORE: %i", score);
        tela.mostra();
        if(tecla == ALLEGRO_KEY_0){
          Dificuldade();
          break;
        }
        if(tecla == ALLEGRO_KEY_1){
          estado = Estado::fim;
          break;
        }
        tela.espera(30);
      }
      // troca estado do jogo para terminado
    }
  }

  // verifica se o jogo terminou ou não
  // - retorna TRUE quando  termina
  // - retorna FALSE caso contrário
  bool verifica_fim(void) {
    if (estado == Estado::nada)
      return false;
    else
      return true;
  }

  void finaliza(void) {
    // fecha a tela
    tela.finaliza();
  }

  // move o tiro (se existir) em certa velocidade
  void tiro_movimenta(void) {
    for(auto t = tiros.begin(); t != tiros.end(); /* nao precisa aqui */){
      (*t).c.centro.y -= (*t).v;
      /* saiu da tela */
      if ((*t).c.centro.y < 0.0) {
        t = tiros.erase(t);
      } else
        t++;
    }
  }

  // lanca tiro do laser
  void tiro_fogo(void) {
    tiro_t tiro;
    tiro.ativo = true;
    tiro.v = 20;
    tiro.c.centro.x = laser.ret.pos.x;
    tiro.c.centro.y = laser.ret.pos.y - 10;
    tiro.c.raio = 5;
    tiros.push_back(tiro);
  }

 // desenha o tiro, sendo que a cada desenha sua cor muda.
  void tiro_desenha(void) {
    if (tiros.empty() == false) {
      for(auto t = tiros.begin(); t != tiros.end(); t++){
        Cor outra = {1.0, 0.0, 0.0};
        tela.cor(outra);
        tela.circulo((*t).c);
      }
    }
  }

 // inicia o laser na posição inicial
  void laser_inicia(void) {
    Tamanho tam = tela.tamanho();
    laser.v = 0.0;
    laser.ret.pos.x = tam.larg / 2;
    laser.ret.pos.y = tam.alt - 20;
    laser.ret.tam.larg = 10;
    laser.ret.tam.alt = 20;
  }

  // laser lança um tiro
  void laser_atira() {
    if (tecla == ALLEGRO_KEY_F) {
      tiro_fogo();
    }
  }

  // altera velocidade do laser com as teclas A ou D
  void laser_altera_velocidade() {
    if (tecla == ALLEGRO_KEY_A) {
      /* altera velocidade mais a esquerda */
      laser.v -= 1;
    } else if (tecla == ALLEGRO_KEY_D) {
      /* altera velocidade mais a direita */
      laser.v += 1;
    }
  }

  // apenas movimenta o laser na tela se necessário
  void laser_move(void) {
    Tamanho tam = tela.tamanho();
    if (((laser.ret.pos.x + laser.v) < 0.0) ||
        ((laser.ret.pos.x + laser.v) > (tam.larg - 10)))
      laser.v = 0.0;
    laser.ret.pos.x += laser.v;
  }

  // desenha o laser
  void laser_desenha(void) {
    Cor vermelho = {1.0, 0.0, 0.0};
    tela.cor(vermelho);
    al_draw_textf(tela.fonte, tela.ac_cor, 0, 0, ALLEGRO_ALIGN_LEFT, "Score %i", score);
    tela.retangulo(laser.ret);
  }

  // desenha todas as figuras e objetos na tela
  void desenha_figuras() {
    Cor azul = {0.2, 0.3, 0.8};
    // Cor vermelho = {1, 0.2, 0};
    // Cor verde = {0.2, 0.9, 0.6};
    // Cor preto = {0, 0, 0};

    // mudar cor da tela
    tela.cor(azul);
    // desenha os objetos do vetor
    for(auto ret=centipede.begin(); ret !=centipede.end();ret++){
      al_draw_bitmap((*ret).cent,(*ret).pos.x,(*ret).pos.y,0);
    }
    for(auto ret=caindo.begin(); ret !=caindo.end();ret++){
      al_draw_bitmap((*ret).cent,(*ret).pos.x,(*ret).pos.y,0);
    }
    /*for (Retangulo &ret : centipede) {
      tela.retangulo(ret);
    }*/

    // desenha laser e tiro
    laser_altera_velocidade();
    laser_atira();
    laser_desenha();
    tiro_desenha();
  }
  void remove_ultimos(int x){
    for(int i=0;i<x;i++){
      auto ret=centipede.rbegin();
      Retangulo r1 = {{(*ret).pos.x, (*ret).pos.y}, {40, 40}};
      r1.cent= al_load_bitmap("caindo.bmp");
      caindo.push_back(r1);
      centipede.pop_back();
    }
  }

  void tiro_verifica_interceptacao(void){
    // testa por uma colisão entre objetos e o tiro
    bool pegou;
    int cont = 0;
    if (tiros.empty() == false) {
      for( auto t = tiros.begin(); t != tiros.end(); ) {
        pegou=false;
        for (auto ret = centipede.begin(); ret != centipede.end();ret++ ) {
          if (intercr((*t).c, *ret)){
            cont++;
            score++;
            pegou=true;
          }
            // incrementa para o proximo elemento

        } // for centipede

        if(pegou){
          t= tiros.erase(t);
        }else{
          t++;
        }
      } 
      remove_ultimos(cont);// for tiros
    } // if tiros
  }

  // Esta função tem os seguintes passos:
  // 1 - movimenta todos os objetos do centipede
  // 2 - movimenta o laser e tiro
  // 3 - testa por colisões entre objetos e tiro do laser
  void move_figuras(void) {
    Tamanho tam = tela.tamanho();
    int cont=0;
    for(auto ret=caindo.begin(); ret != caindo.end(); ret++){
      (*ret).pos.y +=40;
        if((*ret).pos.y > 400){
          cont++;
        }
    }
    for(int i=0; i<cont; i++){
      caindo.pop_front();
    }

    // movimenta todos os retangulos
    for (auto ret = centipede.begin(); ret != centipede.end(); ret++) {

      if ((((*ret).pos.x - vel_geral) < 0.0) && ((*ret).direita==false)){
      (*ret).direita=true;
      //(*ret).pos.y += (*ret).tam.alt;
      (*ret).pos.x= 0;
      (*ret).pos.y +=40;
      if( centipede.begin()==ret){
        Retangulo r1 = {{(*ret).pos.x + 40, (*ret).pos.y}, {40, 40}};
        r1.direita=true;
        r1.cent= al_load_bitmap("cel.bmp");
        centipede.push_front(r1);
      }
    }else if ((((*ret).pos.x + vel_geral) > (tam.larg - 40)) && ((*ret).direita==true)) {
        
        (*ret).direita=false;
        //(*ret).pos.y += (*ret).tam.alt;
        (*ret).pos.y +=40;
        (*ret).pos.x= 560;

      }else{

        if((*ret).direita== true){
          (*ret).pos.x += vel_geral;
        }else{
          (*ret).pos.x -= vel_geral;
        }
      }
    al_draw_bitmap((*ret).cent,(*ret).pos.x,(*ret).pos.y,0);
    }
    // move o laser
    laser_move();
    // movimenta tiros
    tiro_movimenta();
    // verifica se algum tiro pegou um bloco
    tiro_verifica_interceptacao();
  }

  void legenda(void){
    std::cout << "Pressione: " << std::endl;
    std::cout << " - 'a' ou 'd' para mover " << std::endl;
    std::cout << " - 'f' para atirar " << std::endl;
    std::cout << " - 'q' sair" << std::endl;
  }
};

int main(int argc, char **argv) {
  Jogo jogo;

  jogo.inicia();
  jogo.legenda();
  while (jogo.verifica_fim() == false) {
    jogo.atualiza();
  }
  jogo.finaliza();
  return 0;
}
