#include <Keypad.h>              // Biblioteca para teclado matricial
#include <LiquidCrystal_I2C.h>   // Biblioteca para LCD I2C

// Definições do LCD I2C
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES   2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Definições do teclado matricial 4x4
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {12, 14, 27, 26};     // Pinos conectados às linhas do teclado
byte colPins[COLS] = {25, 33, 32, 4};      // Pinos conectados às colunas do teclado

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variáveis de controle da senha
String senhaCorreta = "1234";   // Senha padrão
String senhaDigitada = "";      // Entrada do usuário
bool alterandoSenha = false;    // Flag para modo de alteração de senha
bool confirmandoSenha = false;  // Flag para confirmar senha atual antes de alterar
String novaSenha = "";          // Armazena nova senha durante alteração

// Pinos dos LEDs indicativos
const int ledVerde = 2;
const int ledVermelho = 16;

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, LOW);

  lcd.init();          // Inicializa LCD
  lcd.backlight();     // Liga backlight
  lcd.setCursor(0, 0);
  lcd.print("Digite a senha:");

  Serial.begin(115200); // Inicializa serial para debug
}

void loop() {
  char tecla = keypad.getKey(); // Lê tecla pressionada (se houver)
  if (tecla) {
    if (alterandoSenha) {
      alterarSenha(tecla);  // Função para processo de alteração de senha
    } else {
      switch (tecla) {
        case '#':           // Confirmar senha digitada
          validarAcesso();
          break;
        case '*':           // Apagar entrada atual
          senhaDigitada = "";
          atualizarDisplay("Digite a senha:");
          break;
        case 'C':           // Iniciar processo de alteração de senha
          iniciarAlteracaoSenha();
          break;
        default:            // Entrada de números na senha (máx 4 dígitos)
          if (senhaDigitada.length() < 4) {
            senhaDigitada += tecla;
            lcd.setCursor(senhaDigitada.length() - 1, 1);
            lcd.print('*');  // Mostra asterisco para cada dígito digitado
          }
          break;
      }
    }
  }
}

// Função para validar senha digitada
void validarAcesso() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (senhaDigitada == senhaCorreta) {
    lcd.print("Acesso Permitido");
    digitalWrite(ledVerde, HIGH);
    Serial.println("Acesso permitido");
  } else {
    lcd.print("Acesso Negado");
    digitalWrite(ledVermelho, HIGH);
    Serial.println("Acesso negado");
  }
  delay(3000);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledVermelho, LOW);
  senhaDigitada = "";
  atualizarDisplay("Digite a senha:");
}

// Inicia o modo de alteração de senha, pedindo confirmação da senha atual
void iniciarAlteracaoSenha() {
  atualizarDisplay("Confirme senha:");
  senhaDigitada = "";
  alterandoSenha = true;
  confirmandoSenha = true;
  novaSenha = "";
}

// Função para gerenciar o processo de alteração da senha
void alterarSenha(char tecla) {
  if (tecla == '#') {
    if (confirmandoSenha) {
      // Verifica se senha digitada para confirmação está correta
      if (senhaDigitada == senhaCorreta) {
        atualizarDisplay("Nova senha:");
        novaSenha = "";
        confirmandoSenha = false;
      } else {
        atualizarDisplay("Senha incorreta!");
        delay(2000);
        resetarEstado();
      }
    } else {
      // Finaliza alteração se nova senha tiver 4 dígitos
      if (novaSenha.length() == 4) {
        senhaCorreta = novaSenha;
        atualizarDisplay("Senha alterada!");
        delay(2000);
        resetarEstado();
      } else {
        atualizarDisplay("Senha invalida!");
        delay(2000);
        resetarEstado();
      }
    }
    senhaDigitada = "";
  } else {
    // Entrada de senha de confirmação ou nova senha, limitando 4 dígitos
    if (confirmandoSenha) {
      if (senhaDigitada.length() < 4) {
        senhaDigitada += tecla;
        lcd.setCursor(senhaDigitada.length() - 1, 1);
        lcd.print('*');
      }
    } else {
      if (novaSenha.length() < 4) {
        novaSenha += tecla;
        lcd.setCursor(novaSenha.length() - 1, 1);
        lcd.print('*');
      }
    }
  }
}

// Atualiza a mensagem do display LCD
void atualizarDisplay(String mensagem) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensagem);
}

// Reseta as flags e variáveis para o estado inicial do programa
void resetarEstado() {
  alterandoSenha = false;
  confirmandoSenha = false;
  senhaDigitada = "";
  novaSenha = "";
  atualizarDisplay("Digite a senha:");
}
